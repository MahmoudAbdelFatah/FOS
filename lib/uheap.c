
#include <inc/lib.h>

// malloc()
//	This function use FIRST FIT strategy to allocate space in heap
//  with the given size and return void pointer to the start of the allocated space

//	To do this, we need to switch to the kernel, allocate the required space
//	in Page File then switch back to the user again.
//
//	We can use sys_allocateMem(uint32 virtual_address, uint32 size); which
//		switches to the kernel mode, calls allocateMem(struct Env* e, uint32 virtual_address, uint32 size) in
//		"memory_manager.c", then switch back to the user mode here
//	the allocateMem function is empty, make sure to implement it.


//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//
struct user_heap_info{
	uint32 address;
	//int empty; //if it is empty then empty=0 else empty=1
	//int start_of_free_addr;
	uint32 size;
};

//struct user_heap_info user_heap[(USER_HEAP_MAX-USER_HEAP_START)/PAGE_SIZE];
bool user_heap[(USER_HEAP_MAX-USER_HEAP_START)/PAGE_SIZE]={0};
struct user_heap_info alloc_pages[((USER_HEAP_MAX-USER_HEAP_START)/PAGE_SIZE)];
int idx=0;
/**
 * returns
 * 	1) NULL if there is no space to allocate
 * 	2) VA to the start of the 1st frame in the empty space that has
 * 		Continuous (size) to allocate
 */

void* first_fit_strategy(uint32 size){
	//free_pages_idx=0;
	int n=(USER_HEAP_MAX-USER_HEAP_START)/PAGE_SIZE;
	size = ROUNDUP(size, PAGE_SIZE)/PAGE_SIZE;
	int cnt=0;
	uint32 start_of_free_page;
	bool flag=0;
	for(int i=0; i<n; i++) {
		if(user_heap[i]==0 && flag) {
			cnt++;

		} else if (user_heap[i]==0 && !flag) {
			start_of_free_page = USER_HEAP_START + i*PAGE_SIZE;
			flag=1;
			cnt++;
		}
		else {
			flag=0;
			cnt=0;
		}
		if(cnt >= size )
			return (void*) start_of_free_page;
	}
	return NULL;
}

void* malloc(uint32 size)
{
	//TODO: [PROJECT 2017 - [5] User Heap] malloc() [User Side]
	// Write your code here, remove the panic and write your code
	//panic("malloc() is not implemented yet...!!");

	// Steps:
	//	1) Implement FIRST FIT strategy to search the heap for suitable space
	//		to the required allocation size (space should be on 4 KB BOUNDARY)
	//	 Else,
	//
	//This function should find the space of the required range
	// ******** ON 4KB BOUNDARY ******************* //
	uint32* returned_address;
	//Use sys_isUHeapPlacementStrategyFIRSTFIT() to check the current strategy
	if(sys_isUHeapPlacementStrategyFIRSTFIT())
		returned_address = first_fit_strategy(size);

	//	2) if no suitable space found, return NULL
	if(returned_address == NULL)
		return NULL;

	//	3) Call sys_allocateMem to invoke the Kernel for allocation
	sys_allocateMem((uint32)returned_address, size);
	alloc_pages[idx].address = (uint32)returned_address;
	alloc_pages[idx].size = size;
	uint32 mx_size = ((uint32) returned_address - USER_HEAP_START)/ PAGE_SIZE;
	uint32 sz = ROUNDUP(size, PAGE_SIZE)/PAGE_SIZE;
	for(int i=0; i<sz; i++) {
		user_heap[i+mx_size]=1;
	}
	idx++;
	// 	4) Return pointer containing the virtual address of allocated space,
	return (void*) returned_address;
}

void* smalloc(char *sharedVarName, uint32 size, uint8 isWritable)
{
	//TODO: [PROJECT 2017 - [6] Shared Variables: Creation] smalloc() [User Side]
	// Write your code here, remove the panic and write your code
	//panic("smalloc() is not implemented yet...!!");
	// Steps:
	//	1) Implement FIRST FIT strategy to search the heap for suitable space
	//		to the required allocation size (space should be on 4 KB BOUNDARY)
	//	2) if no suitable space found, return NULL
	//	 Else,
	//	3) Call sys_createSharedObject(...) to invoke the Kernel for allocation of shared variable
	//		sys_createSharedObject(): if succeed, it returns the ID of the created variable. Else, it returns -ve
	//	4) If the Kernel successfully creates the shared variable, return its virtual address
	//	   Else, return NULL
	//This function should find the space of the required range
	// ******** ON 4KB BOUNDARY ******************* //
	//Use sys_isUHeapPlacementStrategyFIRSTFIT() to check the current strategy
	//change this "return" according to your answer
	uint32* VASpace_start = NULL;
	if(sys_isUHeapPlacementStrategyFIRSTFIT()){
		VASpace_start= first_fit_strategy(size);
			if(VASpace_start == NULL)
				return NULL;
	}
	int ID = sys_createSharedObject(sharedVarName,size,isWritable,VASpace_start);

	alloc_pages[idx].address = (uint32) VASpace_start;
	alloc_pages[idx].size = size;
	uint32 max_sz = ((uint32) (VASpace_start) - USER_HEAP_START) / PAGE_SIZE;
	uint32 j = 0;
	size = ROUNDUP(size,PAGE_SIZE) / PAGE_SIZE;
	for (int i=0; i < size; i++)
		user_heap[max_sz+i] = 1;
	idx++;

	return ID<0 ? NULL : (void*) VASpace_start;

}

void* sget(int32 ownerEnvID, char *sharedVarName)
{
	//TODO: [PROJECT 2017 - [6] Shared Variables: Get] sget() [User Side]
	// Write your code here, remove the panic and write your code
	//panic("sget() is not implemented yet...!!");

	// Steps:
	//	1) Get the size of the shared variable (use sys_getSizeOfSharedObject())
	uint32 size = sys_getSizeOfSharedObject(ownerEnvID,sharedVarName);
	//	2) If not exists, return NULL
	if(size== E_SHARED_MEM_NOT_EXISTS)
		return NULL;
	//	3) Implement FIRST FIT strategy to search the heap for suitable space
	//		to share the variable (should be on 4 KB BOUNDARY)
	uint32* returned_address;
	if(sys_isUHeapPlacementStrategyFIRSTFIT()){
	//	4) if no suitable space found, return NULL
	//	 Else,
			returned_address = first_fit_strategy(size);
			if(returned_address== NULL)
				return NULL;
	}
	//	5) Call sys_getSharedObject(...) to invoke the Kernel for sharing this variable
	//		sys_getSharedObject(): if succeed, it returns the ID of the shared variable. Else, it returns -ve
		int SOID = sys_getSharedObject(ownerEnvID,sharedVarName,returned_address);
		alloc_pages[idx].address = (uint32) returned_address;
		alloc_pages[idx].size = size;
		uint32 max_sz = ((uint32) (returned_address) - USER_HEAP_START) / PAGE_SIZE;
		size = ROUNDUP(size,PAGE_SIZE) / PAGE_SIZE;
		for (int i=0; i < size; i++)
			user_heap[max_sz+i] = 1;
		idx++;
		if(SOID >=0){
			return returned_address;
		}
		return NULL;


	//	6) If the Kernel successfully share the variable, return its virtual address
	//	   Else, return NULL
	//

	//This function should find the space for sharing the variable
	// ******** ON 4KB BOUNDARY ******************* //

	//Use sys_isUHeapPlacementStrategyFIRSTFIT() to check the current strategy

	//change this "return" according to your answer
	//return 0;
}

// free():
//	This function frees the allocation of the given virtual_address
//	To do this, we need to switch to the kernel, free the pages AND "EMPTY" PAGE TABLES
//	from page file and main memory then switch back to the user again.
//
//	We can use sys_freeMem(uint32 virtual_address, uint32 size); which
//		switches to the kernel mode, calls freeMem(struct Env* e, uint32 virtual_address, uint32 size) in
//		"memory_manager.c", then switch back to the user mode here
//	the freeMem function is empty, make sure to implement it.
void free(void* virtual_address)
{
	//TODO: [PROJECT 2017 - [5] User Heap] free() [User Side]
	// Write your code here, remove the panic and write your code
	//panic("free() is not implemented yet...!!");

	//you should get the size of the given allocation using its address
	//you need to call sys_freeMem()
	//refer to the project presentation and documentation for details
	//virtual_address = ROUNDDOWN(virtual_address, PAGE_SIZE);
	//int idx=-1;
	//int n=(USER_HEAP_MAX-USER_HEAP_START)/PAGE_SIZE;

	uint32 sz;
	for(int i=0; i<idx; i++) {
		if(alloc_pages[i].address == (uint32)virtual_address ) {
			sz = alloc_pages[i].size;
			sys_freeMem((uint32)virtual_address,sz);
			alloc_pages[i].address=alloc_pages[idx-1].address;
			alloc_pages[i].size=alloc_pages[idx-1].size;
			idx--;
			break;
		}
	}
	uint32 mx_size = ((uint32) virtual_address - USER_HEAP_START)/ PAGE_SIZE;
	sz = ROUNDUP(sz, PAGE_SIZE)/PAGE_SIZE;
	for(int i=0; i<sz; i++) {
		user_heap[i+mx_size] =0;
	}

}


//==================================================================================//
//============================== BONUS FUNCTIONS ===================================//
//==================================================================================//

//=============
// [1] sfree():
//=============
//	This function frees the shared variable at the given virtual_address
//	To do this, we need to switch to the kernel, free the pages AND "EMPTY" PAGE TABLES
//	from main memory then switch back to the user again.
//
//	use sys_freeSharedObject(...); which switches to the kernel mode,
//	calls freeSharedObject(...) in "shared_memory_manager.c", then switch back to the user mode here
//	the freeSharedObject() function is empty, make sure to implement it.

void sfree(void* virtual_address)
{
	//TODO: [PROJECT 2017 - BONUS4] Free Shared Variable [User Side]
	// Write your code here, remove the panic and write your code
	panic("sfree() is not implemented yet...!!");

	//	1) you should find the ID of the shared variable at the given address
	//	2) you need to call sys_freeSharedObject()

}


//===============
// [2] realloc():
//===============

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to malloc().
//	A call with new_size = zero is equivalent to free().

//  Hint: you may need to use the sys_moveMem(uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
//		which switches to the kernel mode, calls moveMem(struct Env* e, uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
//		in "memory_manager.c", then switch back to the user mode here
//	the moveMem function is empty, make sure to implement it.

void *realloc(void *virtual_address, uint32 new_size)
{
	//TODO: [PROJECT 2017 - BONUS3] User Heap Realloc [User Side]
	// Write your code here, remove the panic and write your code
	panic("realloc() is not implemented yet...!!");

}
