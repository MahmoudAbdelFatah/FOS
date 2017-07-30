// Air reservation
// Clerk program
#include <inc/lib.h>
#include <user/air.h>

void
_main(void)
{
	int parentenvID = sys_getparentenvid();

	// Get the shared variables from the main program ***********************************

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

	struct Customer * customers = sget(parentenvID, _customers);

	int* flight1Counter = sget(parentenvID, _flight1Counter);
	int* flight2Counter = sget(parentenvID, _flight2Counter);

	int* flight1BookedCounter = sget(parentenvID, _flightBooked1Counter);
	int* flight2BookedCounter = sget(parentenvID, _flightBooked2Counter);

	int* flight1BookedArr = sget(parentenvID, _flightBooked1Arr);
	int* flight2BookedArr = sget(parentenvID, _flightBooked2Arr);

	int* cust_ready_queue = sget(parentenvID, _cust_ready_queue);

	int* queue_out = sget(parentenvID, _queue_out);
	//cprintf("address of queue_out = %d\n", queue_out);
	// *********************************************************************************

	while(1==1)
	{
		int custId;
		//wait for a customer
		sys_waitSemaphore(parentenvID, _cust_ready);

		//dequeue the customer info
		sys_waitSemaphore(parentenvID, _custQueueCS);
		{
			//cprintf("*queue_out = %d\n", *queue_out);
			custId = cust_ready_queue[*queue_out];
			*queue_out = *queue_out +1;
		}
		sys_signalSemaphore(parentenvID, _custQueueCS);

		//try reserving on the required flight
		int custFlightType = customers[custId].flightType;
		//cprintf("custId dequeued = %d, ft = %d\n", custId, customers[custId].flightType);

		switch (custFlightType)
		{
		case 1:
		{
			//Check and update Flight1
			sys_waitSemaphore(parentenvID, _flight1CS);
			{
				if(*flight1Counter > 0)
				{
					*flight1Counter = *flight1Counter - 1;
					customers[custId].booked = 1;
					flight1BookedArr[*flight1BookedCounter] = custId;
					*flight1BookedCounter =*flight1BookedCounter+1;
				}
				else
				{

				}
			}
			sys_signalSemaphore(parentenvID, _flight1CS);
		}

		break;
		case 2:
		{
			//Check and update Flight2
			sys_waitSemaphore(parentenvID, _flight2CS);
			{
				if(*flight2Counter > 0)
				{
					*flight2Counter = *flight2Counter - 1;
					customers[custId].booked = 1;
					flight2BookedArr[*flight2BookedCounter] = custId;
					*flight2BookedCounter =*flight2BookedCounter+1;
				}
				else
				{

				}
			}
			sys_signalSemaphore(parentenvID, _flight2CS);
		}
		break;
		case 3:
		{
			//Check and update Both Flights
			sys_waitSemaphore(parentenvID, _flight1CS); sys_waitSemaphore(parentenvID, _flight2CS);
			{
				if(*flight1Counter > 0 && *flight2Counter >0 )
				{
					*flight1Counter = *flight1Counter - 1;
					customers[custId].booked = 1;
					flight1BookedArr[*flight1BookedCounter] = custId;
					*flight1BookedCounter =*flight1BookedCounter+1;

					*flight2Counter = *flight2Counter - 1;
					customers[custId].booked = 1;
					flight2BookedArr[*flight2BookedCounter] = custId;
					*flight2BookedCounter =*flight2BookedCounter+1;

				}
				else
				{

				}
			}
			sys_signalSemaphore(parentenvID, _flight2CS); sys_signalSemaphore(parentenvID, _flight1CS);
		}
		break;
		default:
			panic("customer must have flight type\n");
		}

		//signal finished
		char prefix[30]="cust_finished";
		char id[5]; char sname[50];
		ltostr(custId, id);
		strcconcat(prefix, id, sname);
		sys_signalSemaphore(parentenvID, sname);

		//signal the clerk
		sys_signalSemaphore(parentenvID, _clerk);
	}
}
