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

	cprintf("************************************************\n");
	cprintf("MAKE SURE to have a FRESH RUN for this test\n(i.e. don't run any program/test before it)\n");
	cprintf("************************************************\n\n\n");

	int envID = sys_getenvid();

	cprintf("STEP A: checking free of shared object using 2 environments... \n");
	{
		uint32 *x;
		int32 envIdSlave1 = sys_create_env("tshr5slave", (myEnv->page_WS_max_size));
		int32 envIdSlave2 = sys_create_env("tshr5slave", (myEnv->page_WS_max_size));

		int freeFrames = sys_calculate_free_frames() ;
		x = smalloc("x", PAGE_SIZE, 1);
		cprintf("Master env created x (1 page) \n");
		if (x != (uint32*)USER_HEAP_START) panic("Returned address is not correct. check the setting of it and/or the updating of the shared_mem_free_address");
		if ((freeFrames - sys_calculate_free_frames()) !=  1+1+2) panic("Wrong allocation: make sure that you allocate the required space in the user environment and add its frames to frames_storage");

		//to check that the slave environments completed successfully
		rsttst();

		sys_run_env(envIdSlave1);
		sys_run_env(envIdSlave2);

		cprintf("please be patient ...\n");
		env_sleep(3000);

		//to ensure that the slave environments completed successfully
		if (gettst()!=2) panic("test failed");

		sfree(x);
		cprintf("Master env removed x (1 page) \n");
		int diff = (sys_calculate_free_frames() - freeFrames);
		if ( diff !=  0) panic("Wrong free: revise your freeSharedObject logic\n");
	}
	cprintf("Step A completed successfully!!\n\n\n");

	cprintf("STEP B: checking free of 2 shared objects ... \n");
	{
		uint32 *x, *z ;
		int32 envIdSlaveB1 = sys_create_env("tshr5slaveB1", (myEnv->page_WS_max_size));
		int32 envIdSlaveB2 = sys_create_env("tshr5slaveB2", (myEnv->page_WS_max_size));

		z = smalloc("z", PAGE_SIZE, 1);
		cprintf("Master env created z (1 page) \n");

		x = smalloc("x", PAGE_SIZE, 1);
		cprintf("Master env created x (1 page) \n");

		rsttst();

		sys_run_env(envIdSlaveB1);
		sys_run_env(envIdSlaveB2);

		env_sleep(4000); //give slaves time to catch the shared object before removal

		int freeFrames = sys_calculate_free_frames() ;

		sfree(z);
		cprintf("Master env removed z\n");

		sfree(x);
		cprintf("Master env removed x\n");

		int diff = (sys_calculate_free_frames() - freeFrames);

		if (diff !=  1) panic("Wrong free: frames removed not equal 1 !, correct frames to be removed are 1:\nfrom the env: 1 table\nframes_storage of z & x: should NOT cleared yet (still in use!)\n");

		//To indicate that it's completed successfully
		inctst();


	}


	return;
}
