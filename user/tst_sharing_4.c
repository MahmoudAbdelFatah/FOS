// Test the free of shared variables (create_shared_memory)
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

	cprintf("************************************************\n");
	cprintf("MAKE SURE to have a FRESH RUN for this test\n(i.e. don't run any program/test before it)\n");
	cprintf("************************************************\n\n\n");

	int Mega = 1024*1024;
	int kilo = 1024;
	int envID = sys_getenvid();
	cprintf("STEP A: checking free of a shared object ... \n");
	{
		uint32 *x;
		int freeFrames = sys_calculate_free_frames() ;
		x = smalloc("x", PAGE_SIZE, 1);
		if (x != (uint32*)USER_HEAP_START) panic("Returned address is not correct. check the setting of it and/or the updating of the shared_mem_free_address");
		if ((freeFrames - sys_calculate_free_frames()) !=  1+1+2) panic("Wrong allocation: make sure that you allocate the required space in the user environment and add its frames to frames_storage");

		sfree(x);
		if ((freeFrames - sys_calculate_free_frames()) ==  0+0+2) panic("Wrong free: make sure that you free the shared object by calling free_share_object()");
		else if ((freeFrames - sys_calculate_free_frames()) !=  0) panic("Wrong free: revise your freeSharedObject logic");
	}
	cprintf("Step A completed successfully!!\n\n\n");


	cprintf("STEP B: checking free of 2 shared objects ... \n");
	{
		uint32 *x, *z ;
		int freeFrames = sys_calculate_free_frames() ;
		z = smalloc("z", PAGE_SIZE, 1);
		x = smalloc("x", PAGE_SIZE, 1);

		if(x == NULL) panic("Wrong free: make sure that you free the shared object by calling free_share_object()");

		if ((freeFrames - sys_calculate_free_frames()) !=  2+1+4) panic("Wrong previous free: make sure that you correctly free shared object before (Step A)");

		sfree(z);
		if ((freeFrames - sys_calculate_free_frames()) !=  1+1+2) panic("Wrong free: check your logic");

		sfree(x);
		if ((freeFrames - sys_calculate_free_frames()) !=  0) panic("Wrong free: check your logic");

	}
	cprintf("Step B completed successfully!!\n\n\n");

	cprintf("STEP C: checking range of loop during free... \n");
	{
		uint32 *w, *u;
		int freeFrames = sys_calculate_free_frames() ;
		w = smalloc("w", 3 * PAGE_SIZE+1, 1);
		u = smalloc("u", PAGE_SIZE, 1);

		if ((freeFrames - sys_calculate_free_frames()) != 5+1+4) panic("Wrong allocation: make sure that you allocate the required space in the user environment and add its frames to frames_storage");

		sfree(w);
		if ((freeFrames - sys_calculate_free_frames()) !=  1+1+2) panic("Wrong free: check your logic");

		uint32 *o;
		o = smalloc("o", 2 * PAGE_SIZE-1,1);

		if ((freeFrames - sys_calculate_free_frames()) != 3+1+4) panic("Wrong allocation: make sure that you allocate the required space in the user environment and add its frames to frames_storage");

		sfree(o);
		if ((freeFrames - sys_calculate_free_frames()) !=  1+1+2) panic("Wrong free: check your logic");

		sfree(u);
		if ((freeFrames - sys_calculate_free_frames()) !=  0) panic("Wrong free: check your logic");


		//Checking boundaries of page tables
		freeFrames = sys_calculate_free_frames() ;
		w = smalloc("w", 3 * Mega - 1*kilo, 1);
		u = smalloc("u", 7 * Mega - 1*kilo, 1);
		o = smalloc("o", 2 * Mega + 1*kilo, 1);

		if ((freeFrames - sys_calculate_free_frames()) != 3073+4+7) panic("Wrong allocation: make sure that you allocate the required space in the user environment and add its frames to frames_storage");

		sfree(o);
		if ((freeFrames - sys_calculate_free_frames()) !=  2560+3+5) panic("Wrong free: check your logic");

		sfree(w);
		if ((freeFrames - sys_calculate_free_frames()) !=  1792+3+3) panic("Wrong free: check your logic");

		sfree(u);
		if ((freeFrames - sys_calculate_free_frames()) !=  0) panic("Wrong free: check your logic");
	}
	cprintf("Step C completed successfully!!\n\n\n");

	cprintf("Congratulations!! Test of freeSharedObjects [4] completed successfully!!\n\n\n");

	return;
}
