// Test the free of shared variables
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

	uint32 *x;
	x = sget(sys_getparentenvid(),"x");
	int freeFrames = sys_calculate_free_frames() ;

	cprintf("Slave env used x (getSharedObject)\n");

	sfree(x);
	cprintf("Slave env removed x\n");

	int diff = (sys_calculate_free_frames() - freeFrames);
	if (diff != 1) panic("wrong free: frames removed not equal 1 !, correct frames to be removed is 1:\nfrom the env: 1 table for x\nframes_storage: not cleared yet\n");

	//to ensure that this environment is completed successfully
	inctst();

	return;
}
