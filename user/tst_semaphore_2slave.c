// Test the use of semaphores to allow multiprograms to enter the CS at same time
// Slave program: enter the shop, leave it and signal the master program

#include <inc/lib.h>

void
_main(void)
{
	int id = sys_getenvindex();

	int32 parentenvID = sys_getparentenvid();
	//cprintf("Cust %d: outside the shop\n", id);

	sys_waitSemaphore(parentenvID, "shopCapacity") ;
		cprintf("Cust %d: inside the shop\n", id) ;
		env_sleep(1000) ;
	sys_signalSemaphore(parentenvID, "shopCapacity") ;

	cprintf("Cust %d: exit the shop\n", id);
	sys_signalSemaphore(parentenvID, "depend") ;
	return;
}
