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
	cprintf("Slave B1 env used x (getSharedObject)\n");

	cprintf("Slave B1 please be patient ...\n");

	env_sleep(6000);
	int freeFrames = sys_calculate_free_frames() ;

	sfree(x);
	cprintf("Slave B1 env removed x\n");

	if ((sys_calculate_free_frames() - freeFrames) !=  4) panic("B1 wrong free: frames removed not equal 4 !, correct frames to be removed are 4:\nfrom the env: 1 table and 1 for frame of x\nframes_storage of x: should be cleared now\n");

	//To indicate that it's completed successfully
	inctst();
	return;
}

