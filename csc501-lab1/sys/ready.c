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
	
  
	if (resch)
		resched();
	return(OK);
}
