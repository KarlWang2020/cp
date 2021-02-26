#ifndef _CONT_FRAME_POOL_H_                   // include file only once
#define _CONT_FRAME_POOL_H_




class ContFramePool {
    
private:
    /* -- DEFINE YOUR CONT FRAME POOL DATA STRUCTURE(s) HERE. */
    unsigned char * bitmap;        // We implement the simple frame pool with a bitmap
    unsigned int    nFreeFrames;   //
    unsigned long   base_frame_no; // Where does the frame pool start in phys mem?
    unsigned long   nframes;       // Size of the frame pool
    unsigned long   info_frame_no; // Where do we store the management information?
    unsigned long n_info_frames;
    void mark_inaccessible(unsigned long _frame_no);
public:

    static const unsigned int FRAME_SIZE = 4096; 

    ContFramePool(unsigned long _base_frame_no,
                  unsigned long _n_frames,
                  unsigned long _info_frame_no,
                  unsigned long _n_info_frames);

    
    unsigned long get_frames(unsigned int _n_frames);
 
    
    void mark_inaccessible(unsigned long _base_frame_no,
                           unsigned long _n_frames);
 
    
    static void release_frames(unsigned long _first_frame_no);
 
    
    static unsigned long needed_info_frames(unsigned long _n_frames);


};
struct Node {
	ContFramePool *currentPool;
	Node* next;
};
#endif
