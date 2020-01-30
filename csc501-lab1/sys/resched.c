/* resched.c  -  resched */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lab1.h>

// which scheduler is bing used currently
LOCAL int activeScheduler = DEFAULTSCHED;
// used for random scheduler
LOCAL int readyQPriorityTotal = 0;

// *** curr = pointer to process
// *** currPrio = Priority of current process being pointed to by curr
// *** rn = random number generated using rn
int curr, currPrio, rn;

unsigned long currSP;	/* REAL sp of current process */
extern int ctxsw(int, int, int, int);
/*-----------------------------------------------------------------------
 * resched  --  reschedule processor to highest priority ready process
 *
 * Notes:	Upon entry, currpid gives current process id.
 *		Proctab[currpid].pstate gives correct NEXT state for
 *			current process if other than PRREADY.
 *------------------------------------------------------------------------
 */
int resched()
{
  readyQPriorityTotal = 0;
  
	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */
  
  if(activeScheduler == RANDOMSCHED) {// random scheduler
    // get the PCB for currently running process
    optr = &proctab[currpid];
    
    // put the old process back into the ready queue
    if (optr->pstate == PRCURR) {
		  optr->pstate = PRREADY;
		  insert(currpid,rdyhead,optr->pprio);
	  }
    
    // get total of priority in ready queue
    curr = q[rdytail].qprev;
    currPrio = q[curr].qkey;
    while(currPrio) {
      readyQPriorityTotal += currPrio;
      curr = q[curr].qprev;
      currPrio = q[curr].qkey;
    }
    
    // if there is no other process in the ready queue except null process switch to null process
    if(!readyQPriorityTotal) {
      currpid = NULLPROC;
      // remove null process from the ready queue
      nptr = &proctab[getfirst(rdyhead)];
      nptr->pstate = PRCURR;
    } else {
      // generate a random number between (inclusive) : 0 <--> (readyQPriorityTotal - 1)
      rn = rand()%(readyQPriorityTotal-1);  
      
      // get tail entry of ready queue
      curr = q[rdytail].qprev;
      currPrio = q[curr].qkey;
      
      // if current priotity is non-zero (not null process) and greater than the random number generated
      // move to next process in the queue.
      while(currPrio && rn >= currPrio) {
        rn -= currPrio;
        curr = q[curr].qprev;
        currPrio = q[curr].qkey;
      }
      
      // make it the current process
      currpid = curr;
      nptr = &proctab[curr];
      nptr->pstate = PRCURR;
      // remove process from the ready queue and
      dequeue(curr);
     
    }
    
    #ifdef	RTCLOCK
  	preempt = QUANTUM;		/* reset preemption counter	*/
    #endif
    
  } else if(activeScheduler == LINUXSCHED) {//linux like scheduler
    //TODO
  } else {//default scheduler
  
    /* no switch needed if current process priority higher than next*/
  	if (((optr = &proctab[currpid])->pstate == PRCURR) && (lastkey(rdytail)<optr->pprio)) {
  		return(OK);
  	}
  	
  	/* force context switch */
  	if (optr->pstate == PRCURR) {
  		optr->pstate = PRREADY;
  		insert(currpid,rdyhead,optr->pprio);
  	}
  
  	/* remove highest priority process at end of ready list */
  	nptr = &proctab[ (currpid = getlast(rdytail)) ];
  	nptr->pstate = PRCURR;		/* mark it currently running	*/
    #ifdef	RTCLOCK
  	preempt = QUANTUM;		/* reset preemption counter	*/
    #endif
  }
	
	
	ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
	
	/* The OLD process returns here when resumed. */
	return OK;
}

void setschedclass(int sched_class)
{
  if(sched_class == RANDOMSCHED || sched_class == LINUXSCHED){
    activeScheduler = sched_class;
  }
}

int getschedclass()
{
  return activeScheduler;
}

extern void incPrioTot(int prio)
{
  readyQPriorityTotal += prio;
}

extern void decPrioTot(int prio)
{
  readyQPriorityTotal += prio;
}

