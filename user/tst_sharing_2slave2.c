// Test the creation of shared variables and using them
// Slave program2: Get 2 shared variables, edit the writable one, and attempt to edit the readOnly one
#include <inc/lib.h>

void
_main(void)
{
	//Initial test to ensure it works on "PLACEMENT" not "REPLACEMENT"
	{
		uint8 fullWS = 1;
		for (int i = 0; i < myEnv->page_WS_max_size; ++i)
		{
			if (myEnv->__uptr_pws[i].empty)
			{
				fullWS = 0;
				break;
			}
		}
		if (fullWS) panic("Please increase the WS size");
	}

	int32 parentenvID = sys_getparentenvid();
	uint32 *x, *z;

	//GET: z then x, opposite to creation order (x then y then z)
	//So, addresses here will be different from the OWNER addresses
	sys_disable_interrupt();
	int freeFrames = sys_calculate_free_frames() ;
	z = sget(parentenvID,"z");
	if (z != (uint32*)(USER_HEAP_START + 0 * PAGE_SIZE)) panic("Get(): Returned address is not correct. make sure that you align the allocation on 4KB boundary");
	if ((freeFrames - sys_calculate_free_frames()) !=  1) panic("Get(): Wrong sharing- make sure that you share the required space in the current user environment using the correct frames (from frames_storage)");
	sys_enable_interrupt();

	sys_disable_interrupt();
	freeFrames = sys_calculate_free_frames() ;
	x = sget(parentenvID,"x");
	if (x != (uint32*)(USER_HEAP_START + 1 * PAGE_SIZE)) panic("Get(): Returned address is not correct. make sure that you align the allocation on 4KB boundary");
	if ((freeFrames - sys_calculate_free_frames()) !=  0) panic("Get(): Wrong sharing- make sure that you share the required space in the current user environment using the correct frames (from frames_storage)");
	sys_enable_interrupt();

	if (*x != 10) panic("Get(): Shared Variable is not created or got correctly") ;

	//Edit the writable object
	*z = 30;
	if (*z != 30) panic("Get(): Shared Variable is not created or got correctly") ;

	//Attempt to edit the ReadOnly object, it should panic
	cprintf("Attempt to edit the ReadOnly object @ va = %x\n", x);
	*x = 100;

	panic("Test FAILED! it should panic early and not reach this line of code") ;
	return;
}
