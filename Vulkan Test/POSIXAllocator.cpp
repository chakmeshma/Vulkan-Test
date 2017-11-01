#include "POSIXAllocator.h"



// Declare the allocator callbacks as static member functions.
//static void* VKAPI_CALL sAllocation(void* pUserData,
//	size_t size,
//	size_t alignment,
//	VkSystemAllocationScope allocationScope);

//static void* VKAPI_CALL sReallocation(void* pUserData,
//	void* pOriginal,
//	size_t size,
//	size_t alignment,
//	VkSystemAllocationScope allocationScope);

//static void VKAPI_CALL sFree(void* pUserData,
//	void* pMemory);

// Now declare the nonstatic member functions that will actually perform
// the allocations.

void* POSIXAllocator::Allocation(size_t size, size_t alignment, VkSystemAllocationScope allocationScope) {
	return _aligned_malloc(size, alignment);
}

void* VKAPI_CALL POSIXAllocator::Allocation(
	void*                                       pUserData,
	size_t                                      size,
	size_t                                      alignment,
	VkSystemAllocationScope                     allocationScope)
{
	return static_cast<POSIXAllocator*>(pUserData)->Allocation(size,
		alignment,
		allocationScope);
}

void* POSIXAllocator::Reallocation(
	void*                                       pOriginal,
	size_t                                      size,
	size_t                                      alignment,
	VkSystemAllocationScope                     allocationScope)
{
	return _aligned_realloc(pOriginal, size, alignment);
}
void* VKAPI_CALL POSIXAllocator::Reallocation(
	void*                                       pUserData,
	void*                                       pOriginal,
	size_t                                      size,
	size_t                                      alignment,
	VkSystemAllocationScope                     allocationScope)
{

	return static_cast<POSIXAllocator*>(pUserData)->Reallocation(pOriginal,
		size,
		alignment,
		allocationScope);
}

void POSIXAllocator::Free(void* pMemory)
{
	_aligned_free(pMemory);
}

void VKAPI_CALL POSIXAllocator::Free(
	void*                                       pUserData,
	void*                                       pMemory)
{
	return static_cast<POSIXAllocator*>(pUserData)->Free(pMemory);
}