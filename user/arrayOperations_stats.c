#include <inc/lib.h>

//Functions Declarations
void Swap(int *Elements, int First, int Second);
void ArrayStats(int *Elements, int NumOfElements, int *mean, int *var, int *min, int *max, int *med);
int KthElement(int *Elements, int NumOfElements, int k);
int QSort(int *Elements,int NumOfElements, int startIndex, int finalIndex, int kIndex);

void _main(void)
{
	int32 envID = sys_getenvid();
	int32 parentenvID = sys_getparentenvid();

	int ret;
	/*[1] GET SHARED VARs*/
	//Get the shared array & its size
	int *numOfElements = NULL;
	int *sharedArray = NULL;
	sharedArray = sget(parentenvID,"arr") ;
	numOfElements = sget(parentenvID,"arrSize") ;

	//Get the check-finishing counter
	int *finishedCount = NULL;
	finishedCount = sget(parentenvID,"finishedCount") ;

	/*[2] DO THE JOB*/
	int mean;
	int var ;
	int min ;
	int max ;
	int med ;

	//take a copy from the original array
	int *tmpArray;
	tmpArray = smalloc("tmpArr", sizeof(int) * *numOfElements, 0) ;
	int i ;
	for (i = 0 ; i < *numOfElements ; i++)
	{
		tmpArray[i] = sharedArray[i];
	}

	ArrayStats(tmpArray ,*numOfElements, &mean, &var, &min, &max, &med);
	cprintf("Stats Calculations are Finished!!!!\n") ;

	/*[3] SHARE THE RESULTS & DECLARE FINISHING*/
	int *shMean, *shVar, *shMin, *shMax, *shMed;
	shMean = smalloc("mean", sizeof(int), 0) ; *shMean = mean;
	shVar = smalloc("var", sizeof(int), 0) ; *shVar = var;
	shMin = smalloc("min", sizeof(int), 0) ; *shMin = min;
	shMax = smalloc("max", sizeof(int), 0) ; *shMax = max;
	shMed = smalloc("med", sizeof(int), 0) ; *shMed = med;

	(*finishedCount)++ ;

}



///Kth Element
int KthElement(int *Elements, int NumOfElements, int k)
{
	return QSort(Elements, NumOfElements, 0, NumOfElements-1, k-1) ;
}


int QSort(int *Elements,int NumOfElements, int startIndex, int finalIndex, int kIndex)
{
	if (startIndex >= finalIndex) return Elements[finalIndex];

	int pvtIndex = RAND(startIndex, finalIndex) ;
	Swap(Elements, startIndex, pvtIndex);

	int i = startIndex+1, j = finalIndex;

	while (i <= j)
	{
		while (i <= finalIndex && Elements[startIndex] >= Elements[i]) i++;
		while (j > startIndex && Elements[startIndex] <= Elements[j]) j--;

		if (i <= j)
		{
			Swap(Elements, i, j);
		}
	}

	Swap( Elements, startIndex, j);

	if (kIndex == j)
		return Elements[kIndex] ;
	else if (kIndex < j)
		return QSort(Elements, NumOfElements, startIndex, j - 1, kIndex);
	else
		return QSort(Elements, NumOfElements, i, finalIndex, kIndex);
}

void ArrayStats(int *Elements, int NumOfElements, int *mean, int *var, int *min, int *max, int *med)
{
	int i ;
	*mean =0 ;
	*min = 0x7FFFFFFF ;
	*max = 0x80000000 ;
	for (i = 0 ; i < NumOfElements ; i++)
	{
		(*mean) += Elements[i];
		if (Elements[i] < (*min))
		{
			(*min) = Elements[i];
		}
		if (Elements[i] > (*max))
		{
			(*max) = Elements[i];
		}
	}

	(*med) = KthElement(Elements, NumOfElements, NumOfElements/2);

	(*mean) /= NumOfElements;
	(*var) = 0;
	for (i = 0 ; i < NumOfElements ; i++)
	{
		(*var) += (Elements[i] - (*mean))*(Elements[i] - (*mean));
	}
	(*var) /= NumOfElements;
}

///Private Functions
void Swap(int *Elements, int First, int Second)
{
	int Tmp = Elements[First] ;
	Elements[First] = Elements[Second] ;
	Elements[Second] = Tmp ;
}



