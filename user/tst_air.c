// Air reservation problem
// Master program
//TODOTA MAKE SURE THAT MAX_SEMAPHORES macro equals at least 100
#include <inc/lib.h>
#include <user/air.h>
int find(int* arr, int size, int val);

void
_main(void)
{
	int envID = sys_getenvid();

	// *************************************************************************************************
	/// Shared Variables Region ************************************************************************
	// *************************************************************************************************

	int numOfCustomers = 15;
	int flight1Customers = 3;
	int flight2Customers = 8;
	int flight3Customers = 4;

	int flight1NumOfTickets = 8;
	int flight2NumOfTickets = 15;

	char _customers[] = "customers";
	char _custCounter[] = "custCounter";
	char _flight1Counter[] = "flight1Counter";
	char _flight2Counter[] = "flight2Counter";
	char _flightBooked1Counter[] = "flightBooked1Counter";
	char _flightBooked2Counter[] = "flightBooked2Counter";
	char _flightBooked1Arr[] = "flightBooked1Arr";
	char _flightBooked2Arr[] = "flightBooked2Arr";
	char _cust_ready_queue[] = "cust_ready_queue";
	char _queue_in[] = "queue_in";
	char _queue_out[] = "queue_out";

	char _cust_ready[] = "cust_ready";
	char _custQueueCS[] = "custQueueCS";
	char _flight1CS[] = "flight1CS";
	char _flight2CS[] = "flight2CS";

	char _clerk[] = "clerk";
	char _custCounterCS[] = "custCounterCS";
	char _custTerminated[] = "custTerminated";

	char _taircl[] = "taircl";
	char _taircu[] = "taircu";

	struct Customer * custs;
	custs = smalloc(_customers, sizeof(struct Customer)*numOfCustomers, 1);
	//sys_createSharedObject("customers", sizeof(struct Customer)*numOfCustomers, 1, (void**)&custs);


	{
		int f1 = 0;
		for(;f1<flight1Customers; ++f1)
		{
			custs[f1].booked = 0;
			custs[f1].flightType = 1;
		}

		int f2=f1;
		for(;f2<f1+flight2Customers; ++f2)
		{
			custs[f2].booked = 0;
			custs[f2].flightType = 2;
		}

		int f3=f2;
		for(;f3<f2+flight3Customers; ++f3)
		{
			custs[f3].booked = 0;
			custs[f3].flightType = 3;
		}
	}

	int* custCounter = smalloc(_custCounter, sizeof(int), 1);
	*custCounter = 0;

	int* flight1Counter = smalloc(_flight1Counter, sizeof(int), 1);
	*flight1Counter = flight1NumOfTickets;

	int* flight2Counter = smalloc(_flight2Counter, sizeof(int), 1);
	*flight2Counter = flight2NumOfTickets;

	int* flight1BookedCounter = smalloc(_flightBooked1Counter, sizeof(int), 1);
	*flight1BookedCounter = 0;

	int* flight2BookedCounter = smalloc(_flightBooked2Counter, sizeof(int), 1);
	*flight2BookedCounter = 0;

	int* flight1BookedArr = smalloc(_flightBooked1Arr, sizeof(int)*flight1NumOfTickets, 1);
	int* flight2BookedArr = smalloc(_flightBooked2Arr, sizeof(int)*flight2NumOfTickets, 1);

	int* cust_ready_queue = smalloc(_cust_ready_queue, sizeof(int)*numOfCustomers, 1);

	int* queue_in = smalloc(_queue_in, sizeof(int), 1);
	*queue_in = 0;

	int* queue_out = smalloc(_queue_out, sizeof(int), 1);
	*queue_out = 0;

	// *************************************************************************************************
	/// Semaphores Region ******************************************************************************
	// *************************************************************************************************
	sys_createSemaphore(_flight1CS, 1);
	sys_createSemaphore(_flight2CS, 1);

	sys_createSemaphore(_custCounterCS, 1);
	sys_createSemaphore(_custQueueCS, 1);

	sys_createSemaphore(_clerk, 3);

	sys_createSemaphore(_cust_ready, 0);

	sys_createSemaphore(_custTerminated, 0);

	int s=0;
	for(s=0; s<numOfCustomers; ++s)
	{
		char prefix[30]="cust_finished";
		char id[5]; char sname[50];
		ltostr(s, id);
		strcconcat(prefix, id, sname);
		sys_createSemaphore(sname, 0);
	}

	// *************************************************************************************************
	// start all clerks and customers ******************************************************************
	// *************************************************************************************************

	//3 clerks
	uint32 envId;
	envId = sys_create_env(_taircl, (myEnv->page_WS_max_size));
	sys_run_env(envId);

	envId = sys_create_env(_taircl, (myEnv->page_WS_max_size));
	sys_run_env(envId);

	envId = sys_create_env(_taircl, (myEnv->page_WS_max_size));
	sys_run_env(envId);

	//customers
	int c;
	for(c=0; c< numOfCustomers;++c)
	{
		envId = sys_create_env(_taircu, (myEnv->page_WS_max_size));
		sys_run_env(envId);
	}

	//wait until all customers terminated
	for(c=0; c< numOfCustomers;++c)
	{
		sys_waitSemaphore(envID, _custTerminated);
	}

	env_sleep(1500);

	//print out the results
	int b;
	for(b=0; b< (*flight1BookedCounter);++b)
	{
		cprintf("cust %d booked flight 1, originally ordered %d\n", flight1BookedArr[b], custs[flight1BookedArr[b]].flightType);
	}

	for(b=0; b< (*flight2BookedCounter);++b)
	{
		cprintf("cust %d booked flight 2, originally ordered %d\n", flight2BookedArr[b], custs[flight2BookedArr[b]].flightType);
	}

	//check out the final results and semaphores
	{
		int f1 = 0;
		for(;f1<flight1Customers; ++f1)
		{
			if(find(flight1BookedArr, flight1NumOfTickets, f1) != 1)
			{
				panic("Error, wrong booking for user %d\n", f1);
			}
		}

		int f2=f1;
		for(;f2<f1+flight2Customers; ++f2)
		{
			if(find(flight2BookedArr, flight2NumOfTickets, f2) != 1)
			{
				panic("Error, wrong booking for user %d\n", f2);
			}
		}

		int f3=f2;
		for(;f3<f2+flight3Customers; ++f3)
		{
			if(find(flight1BookedArr, flight1NumOfTickets, f3) != 1 || find(flight2BookedArr, flight2NumOfTickets, f3) != 1)
			{
				panic("Error, wrong booking for user %d\n", f3);
			}
		}

		assert(sys_getSemaphoreValue(envID, _flight1CS) == 1);
		assert(sys_getSemaphoreValue(envID, _flight2CS) == 1);

		assert(sys_getSemaphoreValue(envID, _custCounterCS) ==  1);
		assert(sys_getSemaphoreValue(envID, _custQueueCS) ==  1);

		assert(sys_getSemaphoreValue(envID, _clerk) == 3);

		assert(sys_getSemaphoreValue(envID, _cust_ready) == -3);

		assert(sys_getSemaphoreValue(envID, _custTerminated) ==  0);

		int s=0;
		for(s=0; s<numOfCustomers; ++s)
		{
			char prefix[30]="cust_finished";
			char id[5]; char cust_finishedSemaphoreName[50];
			ltostr(s, id);
			strcconcat(prefix, id, cust_finishedSemaphoreName);
			assert(sys_getSemaphoreValue(envID, cust_finishedSemaphoreName) ==  0);
		}

		cprintf("Congratulations, All reservations are successfully done... have a nice flight :)\n");
	}

}


int find(int* arr, int size, int val)
{

	int result = 0;

	int i;
	for(i=0; i<size;++i )
	{
		if(arr[i] == val)
		{
			result = 1;
			break;
		}
	}

	return result;
}
