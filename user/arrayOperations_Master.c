// Scenario that tests the usage of shared variables
#include <inc/lib.h>

void InitializeAscending(int *Elements, int NumOfElements);
void InitializeDescending(int *Elements, int NumOfElements);
void InitializeSemiRandom(int *Elements, int NumOfElements);
uint32 CheckSorted(int *Elements, int NumOfElements);
void ArrayStats(int *Elements, int NumOfElements, int *mean, int *var);

void
_main(void)
{
	/*[1] CREATE SHARED ARRAY*/
	int ret;
	char Chose;
	char Line[30];
	//2012: lock the interrupt
	sys_disable_interrupt();
		cprintf("\n");
		cprintf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
		cprintf("!!!   ARRAY OOERATIONS   !!!\n");
		cprintf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
		cprintf("\n");

		readline("Enter the number of elements: ", Line);

		//Create the shared array & its size
		int *arrSize = smalloc("arrSize", sizeof(int) , 0) ;
		*arrSize = strtol(Line, NULL, 10) ;
		int NumOfElements = *arrSize;
		int *Elements = smalloc("arr", sizeof(int) * NumOfElements , 0) ;

		cprintf("Chose the initialization method:\n") ;
		cprintf("a) Ascending\n") ;
		cprintf("b) Descending\n") ;
		cprintf("c) Semi random\n");
		do
		{
			cprintf("Select: ") ;
			Chose = getchar() ;
			cputchar(Chose);
			cputchar('\n');
		} while (Chose != 'a' && Chose != 'b' && Chose != 'c');

	//2012: unlock the interrupt
	sys_enable_interrupt();

	int  i ;
	switch (Chose)
	{
	case 'a':
		InitializeAscending(Elements, NumOfElements);
		break ;
	case 'b':
		InitializeDescending(Elements, NumOfElements);
		break ;
	case 'c':
		InitializeSemiRandom(Elements, NumOfElements);
		break ;
	default:
		InitializeSemiRandom(Elements, NumOfElements);
	}

	//Create the check-finishing counter
	int numOfSlaveProgs = 3 ;
	int *numOfFinished = smalloc("finishedCount", sizeof(int), 1) ;
	*numOfFinished = 0 ;

	/*[2] RUN THE SLAVES PROGRAMS*/
	int32 envIdQuickSort = sys_create_env("slave_qs", (myEnv->page_WS_max_size));
	int32 envIdMergeSort = sys_create_env("slave_ms", (myEnv->page_WS_max_size));
	int32 envIdStats = sys_create_env("slave_stats", (myEnv->page_WS_max_size));
	sys_run_env(envIdQuickSort);
	sys_run_env(envIdMergeSort);
	sys_run_env(envIdStats);

	/*[3] BUSY-WAIT TILL FINISHING THEM*/
	while (*numOfFinished != numOfSlaveProgs) ;

	/*[4] GET THEIR RESULTS*/
	int *quicksortedArr = NULL;
	int *mergesortedArr = NULL;
	int *mean = NULL;
	int *var = NULL;
	int *min = NULL;
	int *max = NULL;
	int *med = NULL;
	quicksortedArr = sget(envIdQuickSort, "quicksortedArr") ;
	mergesortedArr = sget(envIdMergeSort, "mergesortedArr") ;
	mean = sget(envIdStats, "mean") ;
	var = sget(envIdStats,"var") ;
	min = sget(envIdStats,"min") ;
	max = sget(envIdStats,"max") ;
	med = sget(envIdStats,"med") ;

	/*[5] VALIDATE THE RESULTS*/
	uint32 sorted = CheckSorted(quicksortedArr, NumOfElements);
	if(sorted == 0) panic("The array is NOT quick-sorted correctly") ;
	sorted = CheckSorted(mergesortedArr, NumOfElements);
	if(sorted == 0) panic("The array is NOT merge-sorted correctly") ;
	int correctMean, correctVar ;
	ArrayStats(Elements, NumOfElements, &correctMean , &correctVar);
	int correctMin = quicksortedArr[0];
	int last = NumOfElements-1;
	int middle = (NumOfElements-1)/2;
	int correctMax = quicksortedArr[last];
	int correctMed = quicksortedArr[middle];
	//cprintf("Array is correctly sorted\n");
	//cprintf("mean = %d, var = %d\nmin = %d, max = %d, med = %d\n", *mean, *var, *min, *max, *med);
	//cprintf("mean = %d, var = %d\nmin = %d, max = %d, med = %d\n", correctMean, correctVar, correctMin, correctMax, correctMed);

	if(*mean != correctMean || *var != correctVar|| *min != correctMin || *max != correctMax || *med != correctMed)
		panic("The array STATS are NOT calculated correctly") ;

	cprintf("Congratulations!! Scenario of Using the Shared Variables [Create & Get] completed successfully!!\n\n\n");

	return;
}


uint32 CheckSorted(int *Elements, int NumOfElements)
{
	uint32 Sorted = 1 ;
	int i ;
	for (i = 0 ; i < NumOfElements - 1; i++)
	{
		if (Elements[i] > Elements[i+1])
		{
			Sorted = 0 ;
			break;
		}
	}
	return Sorted ;
}

void InitializeAscending(int *Elements, int NumOfElements)
{
	int i ;
	for (i = 0 ; i < NumOfElements ; i++)
	{
		(Elements)[i] = i ;
	}

}

void InitializeDescending(int *Elements, int NumOfElements)
{
	int i ;
	for (i = 0 ; i < NumOfElements ; i++)
	{
		Elements[i] = NumOfElements - i - 1 ;
	}

}

void InitializeSemiRandom(int *Elements, int NumOfElements)
{
	int i ;
	int Repetition = NumOfElements / 3 ;
	for (i = 0 ; i < NumOfElements ; i++)
	{
		Elements[i] = i % Repetition ;
		//cprintf("Elements[%d] = %d\n",i, Elements[i]);
	}

}

void ArrayStats(int *Elements, int NumOfElements, int *mean, int *var)
{
	int i ;
	*mean =0 ;
	for (i = 0 ; i < NumOfElements ; i++)
	{
		*mean += Elements[i];
	}
	*mean /= NumOfElements;
	*var = 0;
	for (i = 0 ; i < NumOfElements ; i++)
	{
		*var += (Elements[i] - *mean)*(Elements[i] - *mean);
	}
	*var /= NumOfElements;
}
