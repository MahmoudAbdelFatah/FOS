// Scenario that tests the usage of shared variables
#include <inc/lib.h>

void
_main(void)
{
	/*[1] CREATE SHARED VARIABLE & INITIALIZE IT*/
	int *X = smalloc("X", sizeof(int) , 1) ;
	*X = 5 ;

	/*[2] SPECIFY WHETHER TO USE SEMAPHORE OR NOT*/
	cprintf("Do you want to use semaphore (y/n)? ") ;
	char select = getchar() ;
	cputchar(select);
	cputchar('\n');

	/*[3] SHARE THIS SELECTION WITH OTHER PROCESSES*/
	int *useSem = smalloc("useSem", sizeof(int) , 0) ;
	*useSem = 0 ;
	if (select == 'Y' || select == 'y')
		*useSem = 1 ;

	if (*useSem == 1)
	{
		sys_createSemaphore("T", 0);
	}

	/*[4] CREATE AND RUN ProcessA & ProcessB*/

	//Create the check-finishing counter
	int *numOfFinished = smalloc("finishedCount", sizeof(int), 1) ;
	*numOfFinished = 0 ;

	//Create the 2 processes
	int32 envIdProcessA = sys_create_env("midterm_a", (myEnv->page_WS_max_size));
	int32 envIdProcessB = sys_create_env("midterm_b", (myEnv->page_WS_max_size));

	//Run the 2 processes
	sys_run_env(envIdProcessA);
	sys_run_env(envIdProcessB);

	/*[5] BUSY-WAIT TILL FINISHING BOTH PROCESSES*/
	while (*numOfFinished != 2) ;

	/*[6] PRINT X*/
	cprintf("Final value of X = %d\n", *X);

	return;
}
