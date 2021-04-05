/*
 File: scheduler.C

 Author:
 Date  :

 */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "scheduler.H"
#include "thread.H"
#include "console.H"
#include "utils.H"
#include "assert.H"
#include "simple_keyboard.H"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* METHODS FOR CLASS   S c h e d u l e r  */
/*--------------------------------------------------------------------------*/
Scheduler::Scheduler() {
    head->next = NULL;
    head->prev = NULL;
    head->thread = NULL;

    tail->next = NULL;
    tail->prev = NULL;
    tail->thread = NULL;

    thread_count = 0;
    Console::puts("Constructed Scheduler.\n");
}

void Scheduler::yield() {

    if(thread_count == 0) return;
    ListNode * after_head = head->next;
    Thread * yielded_thread = head->thread;

    head->next = NULL;
    after_head->prev = NULL;

    head = after_head;

    thread_count=thread_count-1;
    Thread::dispatch_to(yielded_thread);
}

void Scheduler::resume(Thread * _thread) {

    Machine::enable_interrupts();
    add(_thread);
}

void Scheduler::add(Thread * _thread) {
    ListNode * added = new ListNode();
    added->thread = _thread; added->prev = NULL; added->next = NULL;
    ListNode* past_tail=NULL;

    if(thread_count == 0) {
        head = added; tail = added;
    }
    else {
        past_tail=tail;
        tail=added;
        past_tail->next=tail;
        tail->prev=past_tail;

    }
    thread_count=thread_count+1;
}

void Scheduler::terminate(Thread * _thread) {
    if(thread_count == 0) return;

    ListNode * termin = head;
    ListNode * after_head=NULL;
    ListNode *  before_tail=NULL;
    ListNode * before_temp=NULL;
    ListNode * after_temp=NULL;
    int cursor=0;
    while(termin->thread != _thread || termin != NULL)
    {

        termin=termin->next;
        cursor++;

    }

    if(termin == NULL) return;


    if(termin == head) {
        after_head=head->next;
        termin->next=NULL;
        head->prev=NULL;
        delete termin; //same as: delete head;
        head=after_head;

    }
    else if(termin->next == NULL) {
        before_tail = termin->prev;
        before_tail->next=NULL;
        delete termin;
        tail = before_tail;
        tail->next = NULL;
    }

    else {
        before_temp=termin->next;
        after_temp=termin->prev;
        before_temp->next=after_temp;
        after_temp->prev=before_temp;
        delete termin;
    }
    thread_count=thread_count-1;

}
ListNode * Scheduler::current_head() {
    return head;
}