#include <inc/lib.h>

void _main(void)
{	
	
	
	

	int Mega = 1024*1024;
	int kilo = 1024;
	void* ptr_allocations[20] = {0};
	int freeFrames ;
	//[1] Allocate all
	{
		//Allocate 1 MB
		freeFrames = sys_calculate_free_frames() ;
		ptr_allocations[0] = malloc(1*Mega-kilo);
		if ((uint32) ptr_allocations[0] <  (USER_HEAP_START)) panic("Wrong start address for the allocated space... ");
		if ((freeFrames - sys_calculate_free_frames()) != 256+1 ) panic("Wrong allocation: ");

		//Allocate 1 MB
		freeFrames = sys_calculate_free_frames() ;
		ptr_allocations[1] = malloc(1*Mega-kilo);
		if ((uint32) ptr_allocations[1] <  (USER_HEAP_START + 1*Mega)) panic("Wrong start address for the allocated space... ");
		if ((freeFrames - sys_calculate_free_frames()) != 256 ) panic("Wrong allocation: ");

		//Allocate 1 MB
		freeFrames = sys_calculate_free_frames() ;
		ptr_allocations[2] = malloc(1*Mega-kilo);
		if ((uint32) ptr_allocations[2] <  (USER_HEAP_START + 2*Mega)) panic("Wrong start address for the allocated space... ");
		if ((freeFrames - sys_calculate_free_frames()) != 256 ) panic("Wrong allocation: ");

		//Allocate 1 MB
		freeFrames = sys_calculate_free_frames() ;
		ptr_allocations[3] = malloc(1*Mega-kilo);
		if ((uint32) ptr_allocations[3] <  (USER_HEAP_START + 3*Mega)) panic("Wrong start address for the allocated space... ");
		if ((freeFrames - sys_calculate_free_frames()) != 256 ) panic("Wrong allocation: ");

		//Allocate 2 MB
		freeFrames = sys_calculate_free_frames() ;
		ptr_allocations[4] = malloc(2*Mega-kilo);
		if ((uint32) ptr_allocations[4] <  (USER_HEAP_START + 4*Mega)) panic("Wrong start address for the allocated space... ");
		if ((freeFrames - sys_calculate_free_frames()) != 512 + 1) panic("Wrong allocation: ");

		//Allocate 2 MB
		freeFrames = sys_calculate_free_frames() ;
		ptr_allocations[5] = malloc(2*Mega-kilo);
		if ((uint32) ptr_allocations[5] <  (USER_HEAP_START + 6*Mega)) panic("Wrong start address for the allocated space... ");
		if ((freeFrames - sys_calculate_free_frames()) != 512) panic("Wrong allocation: ");

		//Allocate 3 MB
		freeFrames = sys_calculate_free_frames() ;
		ptr_allocations[6] = malloc(3*Mega-kilo);
		if ((uint32) ptr_allocations[6] <  (USER_HEAP_START + 8*Mega)) panic("Wrong start address for the allocated space... ");
		if ((freeFrames - sys_calculate_free_frames()) != 768 + 1) panic("Wrong allocation: ");

		//Allocate 3 MB
		freeFrames = sys_calculate_free_frames() ;
		ptr_allocations[7] = malloc(3*Mega-kilo);
		if ((uint32) ptr_allocations[7] <  (USER_HEAP_START + 11*Mega)) panic("Wrong start address for the allocated space... ");
		if ((freeFrames - sys_calculate_free_frames()) != 768 + 1) panic("Wrong allocation: ");
	}

	//[2] Free some to create holes
	{
		//1 MB Hole
		freeFrames = sys_calculate_free_frames() ;
		free(ptr_allocations[1]);
		if ((sys_calculate_free_frames() - freeFrames) != 256) panic("Wrong free: ");

		//2 MB Hole
		freeFrames = sys_calculate_free_frames() ;
		free(ptr_allocations[4]);
		if ((sys_calculate_free_frames() - freeFrames) != 512) panic("Wrong free: ");

		//3 MB Hole
		freeFrames = sys_calculate_free_frames() ;
		free(ptr_allocations[6]);
		if ((sys_calculate_free_frames() - freeFrames) != 768) panic("Wrong free: ");
	}
	int cnt = 0;

	//[3] Test Re-allocation
	{
		/*CASE1: Re-allocate that's fit in the same location*/

		//Allocate 512 KB - should be placed in 1st hole
		freeFrames = sys_calculate_free_frames() ;
		ptr_allocations[8] = malloc(512*kilo - kilo);
		if ((uint32) ptr_allocations[8] <  (USER_HEAP_START + 1*Mega) || (uint32) ptr_allocations[8] > (USER_HEAP_START + 1*Mega + PAGE_SIZE)) panic("Wrong start address for the allocated space... ");
		if ((freeFrames - sys_calculate_free_frames()) != 128) panic("Wrong allocation: ");

		//Fill it with data
		int *intArr = (int*) ptr_allocations[8];
		int lastIndexOfInt1 = ((512)*kilo)/sizeof(int) - 1;

		int i = 0;
		for (i=0; i < lastIndexOfInt1 ; i++)
		{
			intArr[i] = i ;
		}

		//Reallocate it [expanded in the same place]
		freeFrames = sys_calculate_free_frames() ;
		ptr_allocations[8] = realloc(ptr_allocations[8], 512*kilo + 256*kilo - kilo);

		//[1] test return address & re-allocated space
		if ((uint32) ptr_allocations[8] < (USER_HEAP_START + 1*Mega) || (uint32) ptr_allocations[8] > (USER_HEAP_START + 1*Mega + PAGE_SIZE)) panic("Wrong start address for the re-allocated space... ");
		if ((freeFrames - sys_calculate_free_frames()) != 64) panic("Wrong re-allocation");


		//[2] test memory access
		int lastIndexOfInt2 = ((512+256)*kilo)/sizeof(int) - 1;

		for (i=lastIndexOfInt1; i < lastIndexOfInt2 ; i++)
		{
			intArr[i] = i ;
		}

		for (i=0; i < lastIndexOfInt2 ; i++)
		{
			cnt++;
			if (intArr[i] != i) panic("Wrong re-allocation: stored values are wrongly changed!");
		}

		//[3] test freeing it after expansion
		freeFrames = sys_calculate_free_frames() ;
		free(ptr_allocations[8]);
		if ((sys_calculate_free_frames() - freeFrames) != 192) panic("Wrong free of the re-allocated space");


		/*CASE2: Re-allocate that's not fit in the same location*/

		//Allocate 1.5 MB - should be placed in 2nd hole
		freeFrames = sys_calculate_free_frames() ;
		ptr_allocations[8] = malloc(1*Mega + 512*kilo - kilo);
		if ((uint32) ptr_allocations[8] <  (USER_HEAP_START + 4*Mega) || (uint32) ptr_allocations[8] > (USER_HEAP_START + 4*Mega + PAGE_SIZE)) panic("Wrong start address for the allocated space... ");
		if ((freeFrames - sys_calculate_free_frames()) != 384) panic("Wrong allocation: ");

		//Fill it with data
		intArr = (int*) ptr_allocations[8];
		lastIndexOfInt1 = (1*Mega + 512*kilo)/sizeof(int) - 1;

		i = 0;
		for (i=0; i < lastIndexOfInt1 ; i++)
		{
			intArr[i] = i ;
		}

		//Reallocate it to 2.5 MB [should be moved to next hole]
		freeFrames = sys_calculate_free_frames() ;
		ptr_allocations[8] = realloc(ptr_allocations[8], 1*Mega + 512*kilo + 1*Mega - kilo);

		//[1] test return address & re-allocated space
		if ((uint32) ptr_allocations[8] < (USER_HEAP_START + 8*Mega) || (uint32) ptr_allocations[8] > (USER_HEAP_START + 8*Mega + PAGE_SIZE)) panic("Wrong start address for the re-allocated space... ");
		if ((freeFrames - sys_calculate_free_frames()) != 256) panic("Wrong re-allocation");

		//[2] test memory access
		lastIndexOfInt2 = (2*Mega + 512*kilo)/sizeof(int) - 1;
		intArr = (int*) ptr_allocations[8];

		for (i=lastIndexOfInt1; i < lastIndexOfInt2 ; i++)
		{
			intArr[i] = i ;
		}

		for (i=0; i < lastIndexOfInt2 ; i++)
		{
			cnt++;
			if (intArr[i] != i) panic("Wrong re-allocation: stored values are wrongly changed!");
		}

		//[3] test freeing it after expansion
		freeFrames = sys_calculate_free_frames() ;
		free(ptr_allocations[8]);
		if ((sys_calculate_free_frames() - freeFrames) != 640) panic("Wrong free of the re-allocated space");

		/*CASE3: Re-allocate that's not fit in the same location*/

		//Fill it with data
		intArr = (int*) ptr_allocations[0];
		lastIndexOfInt1 = (1*Mega)/sizeof(int) - 1;

		i = 0;
		for (i=0; i < lastIndexOfInt1 ; i++)
		{
			intArr[i] = i ;
		}

		//Reallocate it to 4 MB [should be moved to last hole]
		freeFrames = sys_calculate_free_frames() ;
		ptr_allocations[0] = realloc(ptr_allocations[0], 1*Mega + 3*Mega - kilo);

		//[1] test return address & re-allocated space
		if ((uint32) ptr_allocations[0] < (USER_HEAP_START + 14*Mega) || (uint32) ptr_allocations[0] > (USER_HEAP_START + 14*Mega + PAGE_SIZE)) panic("Wrong start address for the re-allocated space... ");
		if ((freeFrames - sys_calculate_free_frames()) != 768 + 1) panic("Wrong re-allocation");

		//[2] test memory access
		lastIndexOfInt2 = (4*Mega)/sizeof(int) - 1;
		intArr = (int*) ptr_allocations[0];
		for (i=lastIndexOfInt1; i < lastIndexOfInt2 ; i++)
		{
			intArr[i] = i ;
		}

		for (i=0; i < lastIndexOfInt2 ; i++)
		{
			cnt++;
			if (intArr[i] != i) panic("Wrong re-allocation: stored values are wrongly changed!");
		}

		//[3] test freeing it after expansion
		freeFrames = sys_calculate_free_frames() ;
		free(ptr_allocations[0]);
		if ((sys_calculate_free_frames() - freeFrames) != 1024+1) panic("Wrong free of the re-allocated space");

	}

	cprintf("Congratulations!! test realloc [1] completed successfully.\n");

	return;
}
