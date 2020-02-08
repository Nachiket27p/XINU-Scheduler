/* contains definitions for different schedulers implemented in lab 1 */

// used to keep track of which scheduler is bing used.
#define DEFAULTSCHED 0 
#define RANDOMSCHED 1 
#define LINUXSCHED 2

// extern functions ot set and get the current scheduler being used.
extern void setschedclass(int sched_class);
extern int getschedclass();

// made variable initialized in clkint ctr1000 extern to use in for
// random scheduler seed
extern unsigned long	ctr1000;
