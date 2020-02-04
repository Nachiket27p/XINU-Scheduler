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
// use for linux like scheduler to keep track remaining epoch
int epoch = 0;

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
    optr = &proctab[currpid];
    //kprintf("lnx old: %d\n", currpid);
    // if the process instiating the rescheduling than change its state
    // to ready and place back in ready queue because it should ready to
    // run at all times regardless of epoch.
    if(currpid == NULLPROC) {
      //kprintf("null proc entered\n");
      optr->pstate = PRREADY;
      insert(currpid, rdyhead, optr->pprio + optr->pcounter);

    } else if(preempt <= 0 && optr->pstate == PRCURR) {
      //kprintf("0 & current\n");
      //optr->pcounter = preempt;
      // if the process has used up its quantum than
      // change the sate of the process in pcb (proctab) to 'W4NE'
      // only set state to 'W4NE' if the process state is 'PRCURR'
      optr->pstate = W4NE;

    } else {
      //kprintf("positive preempt : %d\n", preempt);
      // update the amount of quantum the process remaining using the preempt value
      optr->pcounter = preempt;
      // if the status of the process is still current than place back
      // into ready queue with a new goodness value
      if(optr->pstate == PRCURR) {
        //int goodness = optr->pprio + optr->pcounter;
        optr->pstate = PRREADY;
        insert(currpid, rdyhead, optr->pprio + optr->pcounter);
      }
    }
    
    // sum up the amount of quantum left by adding up the counter (remaining quantum)
    // of all processes in the ready queue
    epoch = 0;
    curr = q[rdytail].qprev;
    while(q[curr].qkey) {
      //kprintf("%d, ", curr);
      epoch += proctab[curr].pcounter;
      curr = q[curr].qprev;
    }
    //kprintf("\n");

    // if ther is a process with remaining quantum in the queue
    // dequeu the process with the highes quantum
    if(epoch) {
      //kprintf("non-zero epoch : %d\n", epoch);
      currpid = getlast(rdytail);
      nptr = &proctab[currpid];
      nptr->pstate = PRCURR;
      preempt = nptr->pcounter;

    } else {
      //kprintf("zero epoch\n");
      // either a new epoch needs to being or there are no processes that can run
      // so the null process will be scheduled.
      
      // first iterate through the proc table and check
      // if there are processes waiting for a new epoch
      int pid;
      struct	pentry	*procIter;
      for(pid = 1; pid < NPROC; pid++) {
          procIter = &proctab[pid];
          if(procIter->pstate == W4NE) {
            procIter->pquantum = (procIter->pcounter/2) + procIter->pprio;
            procIter->pcounter = procIter->pquantum;
            procIter->pstate = PRREADY;
            insert(pid, rdyhead, procIter->pprio + procIter->pcounter);
            epoch += procIter->pcounter;
          }
      }

      // now check if the epoch value is non zero, if so than processes have
      // been added to the ready queue
      if(epoch) {
        // get the process with the highest goodness and schedule it
        currpid = getlast(rdytail);
        nptr = &proctab[currpid];
        preempt = nptr->pcounter;
        nptr->pstate = PRCURR;
      } else {
        // if epoc was zero than nothign was added to the ready queue so
        // the null process must be executed.
        // remove null process from the ready queue and run it
        currpid = getfirst(rdyhead);
        nptr = &proctab[currpid];
        nptr->pstate = PRCURR;

        #ifdef	RTCLOCK
        preempt = QUANTUM;		/* reset preemption counter	*/
        #endif
      }
    }

    //TODO
  } else {//default scheduler
    /* no switch needed if current process priority higher than next*/
  	if (((optr = &proctab[currpid])->pstate == PRCURR) && (lastkey(rdytail)<optr->pprio)) {
  		return(OK);
  	}
  	
  	/* force context switch */
  	if (optr->pstate == PRCURR) {
      //kprintf("def old : %d\n", currpid);
  		optr->pstate = PRREADY;
  		insert(currpid, rdyhead, optr->pprio);
  	}
  
  	/* remove highest priority process at end of ready list */
  	nptr = &proctab[ (currpid = getlast(rdytail)) ];
    //kprintf("def new: %d\n", currpid);
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

