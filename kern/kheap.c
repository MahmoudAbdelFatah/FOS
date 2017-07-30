#include <inc/memlayout.h>
#include <kern/kheap.h>
#include <kern/memory_manager.h>

//NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)

//=================================================================================//
//============================ REQUIRED FUNCTION ==================================//
//=================================================================================//
uint32 firstFreeVAInKHeap = KERNEL_HEAP_START ;
struct kheap_data
{
	uint32 virtual_start;
	int size;
};

 int32 kheap_data_arr[((KERNEL_HEAP_MAX-KERNEL_HEAP_START))/PAGE_SIZE];

void* kmalloc(unsigned int size)
{
		//TODO: [PROJECT 2017 - [1] Kernel Heap] kmalloc()
		// Write your code here, remove the panic and write your code
		//panic("kmalloc() is not implemented yet...!!");
		//NOTE: Allocation is continuous increasing virtual address
		//NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)
		//refer to the project presentation and documentation for details
         uint32 reference  = firstFreeVAInKHeap;
		 size = ROUNDUP(size , PAGE_SIZE) ;
		 int num_pages = size / PAGE_SIZE ;
		if (KERNEL_HEAP_MAX-firstFreeVAInKHeap<size)
			{
			   return NULL ;
			}
	     for (int i=0;i<num_pages;i++)
	     {
	       struct Frame_Info * frames_info_ptr =NULL;
           int ret = allocate_frame(&frames_info_ptr);
           if(ret!= E_NO_MEM)
           {
        	// cprintf("%x \n ", to_physical_address(&frames_info_ptr));
        	   map_frame(ptr_page_directory, frames_info_ptr, (void *)firstFreeVAInKHeap,PERM_WRITEABLE);
        	   firstFreeVAInKHeap+=PAGE_SIZE;
           }

	     }

	//TODO: [PROJECT 2017 - BONUS1] Implement a Kernel allocation strategy
	// Instead of the continuous allocation/deallocation, implement both
	// FIRST FIT and NEXT FIT strategies
	// use "isKHeapPlacementStrategyFIRSTFIT() ..." functions to check the current strategy
	//change this "return" according to your answer
	int index =  (reference - KERNEL_HEAP_START)/ PAGE_SIZE ;
 	kheap_data_arr[index] = num_pages;
 	return (void *)reference;
}

void kfree(void* virtual_address)
{
	//TODO: [PROJECT 2017 - [1] Kernel Heap] kfree()
	// Write your code here, remove the panic and write your code
	uint32 * table_ptr ;
	int index =  ((int32 ) virtual_address - KERNEL_HEAP_START)/ PAGE_SIZE ;
			for(int j=0 ; j< kheap_data_arr[index] ; j++)
			{
				uint32 va = ((int32)virtual_address+(j*PAGE_SIZE)) ;
				unmap_frame(ptr_page_directory, (void*) va);
		        get_page_table(ptr_page_directory,(void*)va,&table_ptr);
 			}
	//you need to get the size of the given allocation using its address
	//refer to the project presentation and documentation for details
}

unsigned int kheap_virtual_address(unsigned int physical_address)
{
	//TODO: [PROJECT 2017 - [1] Kernel Heap] kheap_virtual_address()
	// Write your code here, remove the panic and write your code
	//panic("kheap_virtual_address() is no; implemented yet...!!");
	 	//return the virtual address corresponding to given physical_address
	//fcprintf("%x \n " , physical_address);
	for(uint32 i = KERNEL_HEAP_START ; i< firstFreeVAInKHeap ; i+=PAGE_SIZE)
	{
		struct Frame_Info* frame_info = NULL;
		uint32* ptr_page_table;
		frame_info = get_frame_info(ptr_page_directory, (void*)i, &ptr_page_table);
		if(frame_info != NULL)
		{
			uint32 pa = to_physical_address(frame_info);
			if(pa == physical_address)
				return i;
		}
	}
	/*
	int cnt=0;
	for(uint32 i = KERNEL_HEAP_START ; i< KERNEL_HEAP_MAX ; i+=PAGE_SIZE)
		{
			//struct Frame_Info* frame_info = NULL;
			uint32* ptr_page_table;
			get_page_table(ptr_page_directory, (void*)i, &ptr_page_table);
			if(ptr_page_table != NULL)
			{
				if((ptr_page_table[PTX(i)] >> 12)*PAGE_SIZE == physical_address)
				{
					return i;
				}
			}
		}*/
	//refer to the project presentation and documentation for details

	//change this "return" according to your answer
    //cprintf("it exit from here with -1");
	return 0;
}
unsigned int kheap_physical_address(unsigned int virtual_address)
{
	//TODO: [PROJECT 2017 - [1] Kernel Heap] kheap_physical_address()
	// Write your code here, remove the panic and write your code
	//panic("kheap_physical_address() is not implemented yet...!!");
	uint32* ptr_page_table = NULL;
	get_page_table(ptr_page_directory, (void*)virtual_address, &ptr_page_table);
	//return the physical address corresponding to given virtual_address
	//refer to the project presentation and documentation for details
	//change this "return" according to your answer
	return (ptr_page_table[PTX(virtual_address)]>>12) * PAGE_SIZE;
}


//=================================================================================//
//============================== BONUS FUNCTION ===================================//
//=================================================================================//
// krealloc():

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to kmalloc().
//	A call with new_size = zero is equivalent to kfree().

void *krealloc(void *virtual_address, uint32 new_size)
{
	//TODO: [PROJECT 2017 - BONUS2] Kernel Heap Realloc
	// Write your code here, remove the panic and write your code
	return NULL;
	panic("krealloc() is not implemented yet...!!");

}
void unmapAllFrames(int i)
{
	 while (i-- >=0)
		{
		   firstFreeVAInKHeap-=PAGE_SIZE ;
		   unmap_frame(ptr_page_directory,(void *)firstFreeVAInKHeap);
		}
}
