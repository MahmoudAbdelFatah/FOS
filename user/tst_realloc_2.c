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

	//Bypass the PAGE FAULT on <MOVB immediate, reg> instruction by setting its length
	//and continue executing the remaining code
	sys_bypassPageFault(3);

	//[3] Test Re-allocation
	{
		/*CASE1: Re-allocate with size  = 0*/

		char *byteArr = (char *) ptr_allocations[0];

		//Reallocate with size = 0 [delete it]
		freeFrames = sys_calculate_free_frames() ;
		ptr_allocations[0] = realloc(ptr_allocations[0], 0);

		//[1] test return address & re-allocated space
		if ((uint32) ptr_allocations[0] != 0) panic("Wrong start address for the re-allocated space...it should return NULL!");
		if ((sys_calculate_free_frames() - freeFrames) != 256) panic("Wrong re-allocation");

		//[2] test memory access
		byteArr[0] = 10;
		if (sys_rcr2() != (uint32)&(byteArr[0])) panic("successful access to re-allocated space with size 0!! it should not be succeeded");
		byteArr[(1*Mega-kilo)/sizeof(char) - 1] = 10;
		if (sys_rcr2() != (uint32)&(byteArr[(1*Mega-kilo)/sizeof(char) - 1])) panic("successful access to reallocated space of size 0!! it should not be succeeded");


		/*CASE2: Re-allocate with address = NULL*/

		//new allocation with size = 2.5 MB, should be placed in 3rd hole
		freeFrames = sys_calculate_free_frames() ;
		ptr_allocations[10] = realloc(NULL, 2*Mega + 510*kilo);
		if ((uint32) ptr_allocations[10] <  (USER_HEAP_START + 8*Mega) || (uint32) ptr_allocations[10] > (USER_HEAP_START + 8*Mega + PAGE_SIZE)) panic("Wrong start address for the allocated space... ");
		if ((freeFrames - sys_calculate_free_frames()) != 640) panic("Wrong re-allocation");

		//Fill it with data
		int *intArr = (int*) ptr_allocations[10];
		int lastIndexOfInt1 = (2*Mega + 510*kilo)/sizeof(int) - 1;

		int i = 0;
		for (i=0; i < lastIndexOfInt1 ; i++)
		{
			intArr[i] = i ;
		}

		//[2] test memory access
		for (i=0; i < lastIndexOfInt1 ; i++)
		{
			cnt++;
			if (intArr[i] != i) panic("Wrong re-allocation: stored values are wrongly changed!");
		}

		/*CASE3: Re-allocate in the existing internal fragment (no additional pages are required)*/

		//Reallocate last allocation with 1 extra KB [should be placed in the existing 2 KB internal fragment]
		freeFrames = sys_calculate_free_frames() ;
		ptr_allocations[10] = realloc(ptr_allocations[10], 2*Mega + 510*kilo + kilo);

		//[1] test return address & re-allocated space
		if ((uint32) ptr_allocations[10] < (USER_HEAP_START + 8*Mega) || (uint32) ptr_allocations[10] > (USER_HEAP_START + 8*Mega + PAGE_SIZE)) panic("Wrong start address for the re-allocated space... ");
		if ((freeFrames - sys_calculate_free_frames()) != 0) panic("Wrong re-allocation");

		//[2] test memory access
		int lastIndexOfInt2 = (2*Mega + 510*kilo + kilo)/sizeof(int) - 1;

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
		free(ptr_allocations[10]);
		if ((sys_calculate_free_frames() - freeFrames) != 640) panic("Wrong free of the re-allocated space");


		/*CASE4: Re-allocate that can NOT fit in any free fragment*/

		//Fill 3rd allocation with data
		intArr = (int*) ptr_allocations[2];
		lastIndexOfInt1 = (1*Mega)/sizeof(int) - 1;

		i = 0;
		for (i=0; i < lastIndexOfInt1 ; i++)
		{
			intArr[i] = i ;
		}

		//Reallocate it to large size that can't be fit in any free segment
		freeFrames = sys_calculate_free_frames() ;
		ptr_allocations[2] = realloc(ptr_allocations[2], (USER_HEAP_MAX - USER_HEAP_START - 13*Mega));

		//cprintf("%x\n", ptr_allocations[2]);

		//[1] test return address & re-allocated space
		if ((uint32) ptr_allocations[2] < (USER_HEAP_START + 2*Mega) || (uint32) ptr_allocations[2] > (USER_HEAP_START + 2*Mega + PAGE_SIZE)) panic("Wrong start address for the re-allocated space... ");
		if ((freeFrames - sys_calculate_free_frames()) != 0) panic("Wrong re-allocation");

		//[2] test memory access
		for (i=0; i < lastIndexOfInt1 ; i++)
		{
			cnt++;
			if (intArr[i] != i) panic("Wrong re-allocation: stored values are wrongly changed!");
		}

		//[3] test freeing it after expansion
		freeFrames = sys_calculate_free_frames() ;
		free(ptr_allocations[2]);
		if ((sys_calculate_free_frames() - freeFrames) != 256) panic("Wrong free of the re-allocated space");


		/*CASE5: Re-allocate that only fit in the 1st segment*/

		//[1] create 4 MB hole at beginning of the heap
		//Take 2 MB from currently 3 MB hole at beginning of the heap
		freeFrames = sys_calculate_free_frames() ;
		ptr_allocations[10] = malloc(2*Mega-kilo);
		if ((uint32) ptr_allocations[10] < (USER_HEAP_START) || (uint32) ptr_allocations[10] > (USER_HEAP_START + PAGE_SIZE)) panic("Wrong start address for the allocated space... ");
		if ((freeFrames - sys_calculate_free_frames()) != 512 ) panic("Wrong allocation: ");

		//remove 1 MB allocation between 1 MB hole and 2 MB hole to create 4 MB hole
		freeFrames = sys_calculate_free_frames() ;
		free(ptr_allocations[3]);
		if ((sys_calculate_free_frames() - freeFrames) != 256) panic("Wrong free: ");

		//[2] break down the remaining of the heap into 3 MB segments
		/*for (i = USER_HEAP_START + 14*Mega ; i < USER_HEAP_MAX; i += 3*Mega)
		{
			//allocate 1 page after each 3 MB
			sys_allocateMem(i, PAGE_SIZE) ;
		}*/

		malloc(5*Mega-kilo);

		//Fill last 3MB allocation with data
		intArr = (int*) ptr_allocations[7];
		lastIndexOfInt1 = (3*Mega-kilo)/sizeof(int) - 1;

		i = 0;
		for (i=0; i < lastIndexOfInt1 ; i++)
		{
			intArr[i] = i ;
		}

		//Reallocate it to 4 MB, so that it can only fit at the 1st fragment
		freeFrames = sys_calculate_free_frames() ;
		ptr_allocations[7] = realloc(ptr_allocations[7], 4*Mega-kilo);

		//[1] test return address & re-allocated space
		if ((uint32) ptr_allocations[7] < (USER_HEAP_START + 2*Mega) || (uint32) ptr_allocations[7] > (USER_HEAP_START + 2*Mega + PAGE_SIZE)) panic("Wrong start address for the re-allocated space... ");
		if ((freeFrames - sys_calculate_free_frames()) != 256 - 1) panic("Wrong re-allocation");

		//[2] test memory access
		lastIndexOfInt2 = (4*Mega-kilo)/sizeof(int) - 1;
		intArr = (int*) ptr_allocations[7];
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
		free(ptr_allocations[7]);
		if ((sys_calculate_free_frames() - freeFrames) != 1024) panic("Wrong free of the re-allocated space");

	}

	//set it to 0 again to cancel the bypassing option
	sys_bypassPageFault(0);

	cprintf("Congratulations!! test realloc [2] completed successfully.\n");

	return;
}
