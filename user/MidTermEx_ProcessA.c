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
	int Y ;
	//random delay
	delay = RAND(2000, 10000);
	env_sleep(delay);
//	cprintf("delay = %d\n", delay);

	Y = (*X) * 2 ;

	//random delay
	delay = RAND(2000, 10000);
	env_sleep(delay);
//	cprintf("delay = %d\n", delay);

	(*X) = Y ;

	//random delay
	delay = RAND(2000, 10000);
	env_sleep(delay);
//	cprintf("delay = %d\n", delay);

	if (*useSem == 1)
	{
		sys_signalSemaphore(parentenvID, "T") ;
	}

	/*[3] DECLARE FINISHING*/
	(*finishedCount)++ ;

}
