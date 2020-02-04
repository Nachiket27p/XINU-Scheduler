
#define DEFAULTSCHED 0 
#define RANDOMSCHED 1 
#define LINUXSCHED 2

extern void setschedclass(int sched_class);
extern int getschedclass();

extern int epoch;

extern void incPrioTot(int prio);
extern void decPrioTot(int prio);
