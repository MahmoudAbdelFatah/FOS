// Test the creation of shared variables and using them
// Master program: create the shared variables, initialize them and run slaves
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
	uint32 *x, *y, *z ;

	//x: Readonly
	int freeFrames = sys_calculate_free_frames() ;
	x = smalloc("x", 4, 0);
	if (x != (uint32*)USER_HEAP_START) panic("Create(): Returned address is not correct. make sure that you align the allocation on 4KB boundary");
	if ((freeFrames - sys_calculate_free_frames()) !=  1+1+2) panic("Create(): Wrong allocation- make sure that you allocate the required space in the user environment and add its frames to frames_storage");

	//y: Readonly
	freeFrames = sys_calculate_free_frames() ;
	y = smalloc("y", 4, 0);
	if (y != (uint32*)(USER_HEAP_START + 1 * PAGE_SIZE)) panic("Create(): Returned address is not correct. make sure that you align the allocation on 4KB boundary");
	if ((freeFrames - sys_calculate_free_frames()) !=  1+0+2) panic("Create(): Wrong allocation- make sure that you allocate the required space in the user environment and add its frames to frames_storage");

	//z: Writable
	freeFrames = sys_calculate_free_frames() ;
	z = smalloc("z", 4, 1);
	if (z != (uint32*)(USER_HEAP_START + 2 * PAGE_SIZE)) panic("Create(): Returned address is not correct. make sure that you align the allocation on 4KB boundary");
	if ((freeFrames - sys_calculate_free_frames()) !=  1+0+2) panic("Create(): Wrong allocation- make sure that you allocate the required space in the user environment and add its frames to frames_storage");

	*x = 10 ;
	*y = 20 ;

	int id1, id2, id3;
	id1 = sys_create_env("shr2Slave1", (myEnv->page_WS_max_size));
	id2 = sys_create_env("shr2Slave1", (myEnv->page_WS_max_size));
	id3 = sys_create_env("shr2Slave1", (myEnv->page_WS_max_size));

	//sys_disable_interrupt();
	//to check that the slave environments completed successfully
	rsttst();
	//sys_enable_interrupt();

	sys_run_env(id1);
	sys_run_env(id2);
	sys_run_env(id3);

	env_sleep(12000) ;

	//to ensure that the slave environments completed successfully
	if (gettst()!=3) panic("test failed");


	if (*z != 30)
		panic("Error!! Please check the creation (or the getting) of shared variables!!\n\n\n");
	else
		cprintf("Congratulations!! Test of Shared Variables [Create & Get] [2] completed successfully!!\n\n\n");

	cprintf("Now, ILLEGAL MEM ACCESS should be occur, due to attempting to write a ReadOnly variable\n\n\n");

	id1 = sys_create_env("shr2Slave2", (myEnv->page_WS_max_size));

	env_sleep(3000) ;

	sys_run_env(id1);

	return;
}
