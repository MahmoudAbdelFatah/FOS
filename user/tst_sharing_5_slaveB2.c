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
	uint32 *z;
	z = sget(sys_getparentenvid(),"z");
	cprintf("Slave B2 env used z (getSharedObject)\n");

	cprintf("Slave B2 please be patient ...\n");

	env_sleep(9000);
	int freeFrames = sys_calculate_free_frames() ;

	sfree(z);
	cprintf("Slave B2 env removed z\n");

	if ((sys_calculate_free_frames() - freeFrames) !=  4) panic("wrong free: frames removed not equal 4 !, correct frames to be removed are 4:\nfrom the env: 1 table + 1 frame for z\nframes_storage of z: should be cleared now\n");

	//to ensure that the other environments completed successfully
	if (gettst()!=2) panic("test failed");

	cprintf("Step B completed successfully!!\n\n\n");
	cprintf("Congratulations!! Test of freeSharedObjects [5] completed successfully!!\n\n\n");

	return;
}
