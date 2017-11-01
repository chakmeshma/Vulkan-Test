#pragma once

#include <vulkan\vulkan.h>
#include <iostream>

class POSIXAllocator
{
public:
	inline operator VkAllocationCallbacks() const
	{
		VkAllocationCallbacks result;

		result.pUserData = (void*)this;
		result.pfnAllocation = Allocation;
		result.pfnReallocation = Reallocation;
		result.pfnFree = Free;
		result.pfnInternalAllocation = nullptr;
		result.pfnInternalFree = nullptr;

		return result;
	};
private:
	void* POSIXAllocator::Allocation(size_t size, size_t alignment, VkSystemAllocationScope allocationScope);

	static void* VKAPI_CALL POSIXAllocator::Allocation(
		void*                                       pUserData,
		size_t                                      size,
		size_t                                      alignment,
		VkSystemAllocationScope                     allocationScope);

	void* POSIXAllocator::Reallocation(
		void*                                       pOriginal,
		size_t                                      size,
		size_t                                      alignment,
		VkSystemAllocationScope                     allocationScope);
	static void* VKAPI_CALL POSIXAllocator::Reallocation(
		void*                                       pUserData,
		void*                                       pOriginal,
		size_t                                      size,
		size_t                                      alignment,
		VkSystemAllocationScope                     allocationScope);

	void POSIXAllocator::Free(void* pMemory);

	static void VKAPI_CALL POSIXAllocator::Free(
		void*                                       pUserData,
		void*                                       pMemory);
};