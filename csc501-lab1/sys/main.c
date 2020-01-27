/* user.c - main */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <q.h>

/*------------------------------------------------------------------------
 *  main  --  user main program
 *------------------------------------------------------------------------
 */
int main()
{
	kprintf("\n\nHello World, Xinu lives\n\n");
  
  struct	qent	*curptr = &q[rdytail];
  struct	qent	*curptr1 = &q[rdyhead];
  kprintf("%d\t %d\n", curptr->qkey, curptr1->qkey);
  
  curptr = curptr1->qnext;
  kprintf("%d\n", curptr->qkey);
  
  curptr = curptr1->qnext;
  kprintf("%d\n", curptr->qkey);
  
  curptr = curptr1->qnext;
  kprintf("%d\n", curptr->qkey);
  
	return 0;
}
