/* ready.c - ready */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lab1.h>

/*------------------------------------------------------------------------
 * ready  --  make a process eligible for CPU service
 *------------------------------------------------------------------------
 */
int ready(int pid, int resch)
{
	register struct	pentry	*pptr;

	if (isbadpid(pid))
		return(SYSERR);

	pptr = &proctab[pid];

	if(getschedclass() == LINUXSCHED) {
		pptr->pstate = W4NE;
	} else {
		pptr->pstate = PRREADY;
		insert(pid,rdyhead,pptr->pprio);
	}
	
	// PA1
	// If LINUXSCHED is being used than a new process which is ready to run
	// is not allowed to run until the next epoch, so there is no need to
	// call resched(). Unless the current process is the null process.
	if (resch && (getschedclass() != LINUXSCHED || currpid == NULLPROC))
		resched();
	return(OK);
}
