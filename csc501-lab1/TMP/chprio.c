/* chprio.c - chprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include <lab1.h>

/*------------------------------------------------------------------------
 * chprio  --  change the scheduling priority of a process
 *------------------------------------------------------------------------
 */
SYSCALL chprio(int pid, int newprio)
{
	STATWORD ps;    
	struct	pentry	*pptr;

	disable(ps);
	if (isbadpid(pid) || newprio<=0 || (pptr = &proctab[pid])->pstate == PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	pptr->pprio = newprio;

	if(getschedclass() == RANDOMSCHED) {
		// update ready queue by removing and inserting process
		int curr = q[rdytail].qprev;
		// find the process whose priority is being updated
		while( curr != pid )
			curr = q[curr].qprev;
		// remove and insert back into queue to update its position in the queue
		dequeue(curr);
		insert(pid, rdyhead, newprio);
	}
  
	restore(ps);
	return(newprio);
}
