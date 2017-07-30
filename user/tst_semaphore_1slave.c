// Test the use of semaphores for critical section & dependency
// Slave program: enter critical section, print it's ID, exit and signal the master program
#include <inc/lib.h>

void
_main(void)
{
	int32 parentenvID = sys_getparentenvid();
	int id = sys_getenvindex();
	cprintf("%d: before the critical section\n", id);

	sys_waitSemaphore(parentenvID, "cs1") ;
		cprintf("%d: inside the critical section\n", id) ;
		cprintf("my ID is %d\n", id);
		int sem1val = sys_getSemaphoreValue(parentenvID, "cs1");
		if (sem1val > 0)
			panic("Error: more than 1 process inside the CS... please review your semaphore code again...");
		env_sleep(1000) ;
	sys_signalSemaphore(parentenvID, "cs1") ;

	cprintf("%d: after the critical section\n", id);
	sys_signalSemaphore(parentenvID, "depend1") ;
	return;
}
