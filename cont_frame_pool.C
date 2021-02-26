
#include "cont_frame_pool.H"
#include "console.H"
#include "utils.H"

#include<cassert>


//used as a mark
Node *pool_head;

ContFramePool::ContFramePool(unsigned long _base_frame_no,
                             unsigned long _n_frames,
                             unsigned long _info_frame_no,
                             unsigned long _n_info_frames)
:base_frame_no(_base_frame_no),
n_frames(_n_frames),
info_frame_no(_info_frame_no),
n_info_frames(_n_info_frames){

    nFreeFrames = _n_frames;



   unsigned long switch_frame_no;
   switch(info_frame_no)
   {
       case 0: switch_frame_no=base_frame_no;
       default: switch_frame_no=info_frame_no;
   }
   bitmap=(unsigned char * )(switch_frame_no*FRAME_SIZE);



    for(int i = 0; i * 4 < nframes; i++) {
        bitmap[i] = 0xFF;
    }



    for (int i = _info_frame_no,
         int _info_end_frame_no= _info_frame_no+ _n_info_frames ;
         i < _info_end_frame_no;
         i++) {
        int idx_inf = (i - _info_frame_no) / 4;

        bitmap[idx_inf] ^= (1 << 7) >> ((i - _info_frame_no) % 4);

    }
    nFreeFrames -= _n_info_frames;


    if(info_frame_no == 0){
        bitmap[0] = 0x7F;
        nFreeFrames--;
    }

    Node* the_node;
    Node* the_next = pool_head->next;
    the_node->currentPool = this;


    the_node->next=the_next;
    pool_head->next=the_node;

    Console::puts("new Frame Pool\n");
}

unsigned long ContFramePool::get_frames(unsigned int _n_frames)
{

    assert(nFreeFrames >= _n_frames)
    int valid= 1;
    unsigned long header = base_frame_no;
    int total_n_frames = _n_frames;


    for(int i = base_frame_no, unsigned long end_frame_no=base_frame_no+ nframes ;
        i < end_frame_no;
        i++)

    {
        unsigned long diff= i-base_frame_no;
       unsigned  char mask = (1 << 7) >> (diff % 4);
       unsigned int idx = diff / 4;

        valid = bitmap[idx] & mask; //variable valid indicated  if the frame is used or not
        if(valid){
            if (total_n_frames == _n_frames) { //new consecutive allocation started
                header = i;
            }
            total_n_frames--;

            if (total_n_frames == 0) { //all need frames can be allocated

                mark_inaccessible(header, _n_frames);
                unsigned long diff2= header-base_frame_no;

                unsigned char temp_mask2 = (1<<7) >> (diff2 % 4);

                bitmap[diff2 / 4] ^= temp_mask2;
                this->nFreeFrames -= _n_frames;
                return header;
            }
        }
        else {
            total_n_frames=_n_frames; // can't allocate consecutive frames, so it's reset
        }
    }

    return 0;
}

void ContFramePool::mark_inaccessible(unsigned long _base_frame_no,
                                      unsigned long _n_frames)
{

     unsigned long _end_frame_no=_base_frame_no+_n_frames;

    for(int i = _base_frame_no; i < _end_frame_no; i++){

        unsigned long diff = i- base_frame_no;
        unsigned int idx = (diff / 4);
        unsigned char mask =  (1<<7) >> (diff % 4);

        if ((bitmap[idx] & mask) != 0)
        assert(false); // can't be allocated


        bitmap[idx] ^= mask;
        nFreeFrames=nFreeFrame-1;

    }
}


void ContFramePool::release_frames(unsigned long _first_frame_no)
{


    for(Node * node=pool_head->next, ContFramePool* current=node->currentPool;
       node!=NULL;
       node=node->next
       )
    {
    current_Pool=node->currentPool;
    unsigned long bfn=current->base_frame_no;
    unsigned long nfs=current->nframes;
    if(_first_frame_no>=bfm && _first_frame_no< bfn+nfs)
        break;

    }

     ContFramePool current_pool= *(node->currentPool);

    unsigned long diff=_first_frame_no-current_pool.base_frame_no;
    unsigned char mask_re_head =  (1<<3) >> (diff % 4) ;
    unsigned int idx_re_head = diff / 4;
    current_pool.bitmap[idx_re_head] |= mask_re_head;

    unsigned long end_frame_no=current_pool.base_frame_no+ current_pool.nframes;
    for(int i = _first_frame_no;
        i <end_frame_no;
        i++) {

        unsigned int diff2 = i - current_pool.base_frame_no;
        unsigned char mask_re = (1<<7) >> (diff2 % 4);
        unsigned char mask_head=(1<<3) >> (diff2 % 4);
        unsigned int idx_re = (diff2 / 4);

        if(current_pool.bitmap[idx_re] & mask_head == 0 ||current_pool.bitmap[idx_re] & mask_re != 0 )
            break;



        current_pool.bitmap[idx_re] |= mask_re;
    }

}

unsigned long ContFramePool::needed_info_frames(unsigned long _n_frames)
{
    unsigned  long res= _n_frames/ 0x1000<<2;
    if((_n_frames % (0x1000<<2))>0)
        res+1;

    return res;


}
