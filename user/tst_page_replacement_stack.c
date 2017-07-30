/********************************************************** */
/* MAKE SURE PAGE_WS_MAX_SIZE = 6 */
/************************************************************/

#include <inc/lib.h>

void _main(void)
{
	char arr[PAGE_SIZE*10];

	uint32 kilo = 1024;

	
//	cprintf("envID = %d\n",envID);

	
	

	int freePages = sys_calculate_free_frames();
	int usedDiskPages = sys_pf_calculate_allocated_pages();

	int i ;
	for (i = 0 ; i < PAGE_SIZE*10 ; i+=PAGE_SIZE/2)
		arr[i] = -1 ;


	cprintf("checking REPLACEMENT fault handling of STACK pages... \n");
	{
		for (i = 0 ; i < PAGE_SIZE*10 ; i+=PAGE_SIZE/2)
			if( arr[i] != -1) panic("modified stack page(s) not restored correctly");

		if( (sys_pf_calculate_allocated_pages() - usedDiskPages) !=  9) panic("Unexpected extra/less pages have been added to page file");

		if( (freePages - (sys_calculate_free_frames() + sys_calculate_modified_frames())) != 0 ) panic("Extra memory are wrongly allocated... It's REplacement: expected that no extra frames are allocated");
	}

	cprintf("Congratulations: stack pages created, modified and read successfully!\n\n");


	return;
}

