#include <inc/lib.h>

void _main(void)
{
	int32 parentenvID = sys_getparentenvid();
	int delay;

	/*[1] GET SHARED VARIABLE, SEMAPHORE SEL, check-finishing counter*/
	int *X = sget(parentenvID, "X") ;
	int *useSem = sget(parentenvID, "useSem") ;
	int *finishedCount = sget(parentenvID, "finishedCount") ;

	/*[2] DO THE JOB*/
	int Z ;
	if (*useSem == 1)
	{
		sys_waitSemaphore(parentenvID, "T") ;
	}

	//random delay
	delay = RAND(2000, 10000);
	env_sleep(delay);
//	cprintf("delay = %d\n", delay);

	Z = (*X) + 1 ;

	//random delay
	delay = RAND(2000, 10000);
	env_sleep(delay);
//	cprintf("delay = %d\n", delay);

	(*X) = Z ;

	//random delay
	delay = RAND(2000, 10000);
	env_sleep(delay);
//	cprintf("delay = %d\n", delay);

	/*[3] DECLARE FINISHING*/
	(*finishedCount)++ ;

}
