// Air reservation
// Customer program
#include <inc/lib.h>
#include <user/air.h>

void
_main(void)
{
	int32 parentenvID = sys_getparentenvid();

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

	// Get the shared variables from the main program ***********************************

	struct Customer * customers = sget(parentenvID, _customers);

	int* custCounter = sget(parentenvID, _custCounter);

	int* cust_ready_queue = sget(parentenvID, _cust_ready_queue);

	int* queue_in = sget(parentenvID, _queue_in);

	// *********************************************************************************

	int custId, flightType;
	sys_waitSemaphore(parentenvID, _custCounterCS);
	{
		custId = *custCounter;
		//cprintf("custCounter= %d\n", *custCounter);
		*custCounter = *custCounter +1;
	}
	sys_signalSemaphore(parentenvID, _custCounterCS);

	//wait on one of the clerks
	sys_waitSemaphore(parentenvID, _clerk);

	//enqueue the request
	flightType = customers[custId].flightType;
	sys_waitSemaphore(parentenvID, _custQueueCS);
	{
		cust_ready_queue[*queue_in] = custId;
		*queue_in = *queue_in +1;
	}
	sys_signalSemaphore(parentenvID, _custQueueCS);

	//signal ready
	sys_signalSemaphore(parentenvID, _cust_ready);

	//wait on finished
	char prefix[30]="cust_finished";
	char id[5]; char sname[50];
	ltostr(custId, id);
	strcconcat(prefix, id, sname);
	sys_waitSemaphore(parentenvID, sname);

	//print the customer status
	if(customers[custId].booked == 1)
	{
		cprintf("cust %d: finished (BOOKED flight %d) \n", custId, flightType);
	}
	else
	{
		cprintf("cust %d: finished (NOT BOOKED) \n", custId);
	}

	//customer is terminated
	sys_signalSemaphore(parentenvID, _custTerminated);

	return;
}
