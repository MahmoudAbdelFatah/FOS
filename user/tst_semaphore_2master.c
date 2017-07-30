// Test the use of semaphores to allow multiprograms to enter the CS at same time
// Master program: take user input, create the semaphores, run slaves and wait them to finish
#include <inc/lib.h>

void
_main(void)
{
	int envID = sys_getenvid();
	char line[256] ;
	readline("Enter total number of customers: ", line) ;
	int totalNumOfCusts = strtol(line, NULL, 10);
	readline("Enter shop capacity: ", line) ;
	int shopCapacity = strtol(line, NULL, 10);

	sys_createSemaphore("shopCapacity", shopCapacity);
	sys_createSemaphore("depend", 0);

	int i = 0 ;
	int id ;
	for (; i<totalNumOfCusts; i++)
	{
		id = sys_create_env("sem2Slave", (myEnv->page_WS_max_size));
		sys_run_env(id) ;
	}

	for (i = 0 ; i<totalNumOfCusts; i++)
	{
		sys_waitSemaphore(envID, "depend") ;
	}
	int sem1val = sys_getSemaphoreValue(envID, "shopCapacity");
	int sem2val = sys_getSemaphoreValue(envID, "depend");
	if (sem2val == 0 && sem1val == shopCapacity)
		cprintf("Congratulations!! Test of Semaphores [2] completed successfully!!\n\n\n");
	else
		cprintf("Error: wrong semaphore value... please review your semaphore code again...");

	return;
}
