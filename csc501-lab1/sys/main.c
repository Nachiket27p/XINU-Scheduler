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
  
  int	head = q[rdyhead].qnext;
  head = q[head].qnext;
  kprintf("tail dummy ptr : %d\n", head); // tail dummy node
  kprintf("tail dummy     : %d\n", q[head].qkey); // tail dummy node
  
  int tail = q[rdytail].qprev;
  kprintf("null proc ptr  : %d\n", tail); // null process entry
  kprintf("null prio      : %d\n", q[tail].qkey); // null process entry
  
  tail = q[tail].qprev;
  kprintf("dummy head ptr : %d\n", tail); // head dummy node
  kprintf("dummy head     : %d\n", q[tail].qkey); // head dummy node
    
  
	return 0;
}
