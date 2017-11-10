#pragma once

#define VK_USE_PLATFORM_WIN32_KHR

#include <string>
#include <iostream>
#include <vector>
#include <mutex>
#include <vulkan\vulkan.h>
#include "Vulkan Engine Exception.h"
#include "POSIXAllocator.h"

const uint16_t MAX_DEFAULT_ARRAY_SIZE = 10;
const uint16_t MAX_BUFFER_ARRAY_SIZE = MAX_DEFAULT_ARRAY_SIZE;
const uint16_t MAX_IMAGE_ARRAY_SIZE = MAX_DEFAULT_ARRAY_SIZE;
const uint16_t MAX_IMAGE_VIEW_ARRAY_SIZE = MAX_DEFAULT_ARRAY_SIZE;
const uint16_t MAX_DEVICE_MEMORY_ALLOCATION_ARRAY_SIZE = MAX_DEFAULT_ARRAY_SIZE;
const uint16_t MAX_SPARSE_IMAGE_ARRAY_SIZE = MAX_DEFAULT_ARRAY_SIZE;
const uint16_t MAX_SPARSE_IMAGE_MEMORY_REQUIREMENTS_ARRAY_SIZE = MAX_DEFAULT_ARRAY_SIZE;
const uint16_t MAX_SPARSE_IMAGE_FORMAT_PROPERTIES_ARRAY_SIZE = MAX_DEFAULT_ARRAY_SIZE;

class VulkanEngine {
public:
	VulkanEngine(HINSTANCE hInstance, HWND windowHandle, VulkanEngine** ppUnstableInstance);
	~VulkanEngine();
	void getInstanceExtensions();
	void showDeviceExtensions();
	static std::string getVersionString(uint32_t versionBitmask);
private:
	uint32_t instanceExtensionsCount = 0;
	POSIXAllocator* instanceCrationAllocator;
	POSIXAllocator* deviceCreationAllocator;
	POSIXAllocator* deviceTerminationAllocator;
	POSIXAllocator* instanceTerminationAllocator;
	POSIXAllocator* bufferCreationAllocator;
	POSIXAllocator* imageCreationAllocator;
	POSIXAllocator* imageViewCreationAllocator;
	POSIXAllocator* imageViewTerminationAllocator;
	POSIXAllocator* imageTerminationAllocator;
	POSIXAllocator* bufferTerminationAllocator;
	POSIXAllocator* memoryAllocationAllocator;
	POSIXAllocator* memoryDeallocationAllocator;
	POSIXAllocator* sparseImageCreationAllocator;
	POSIXAllocator* sparseImageTerminationAllocator;
	POSIXAllocator* surfaceCreationAllocator;
	POSIXAllocator* swapchainCreationAllocator;
	POSIXAllocator* swapchainTerminationAllocator;
	VkAllocationCallbacks instanceCrationCallbacks;
	VkAllocationCallbacks deviceCreationCallbacks;
	VkAllocationCallbacks deviceTerminationCallbacks;
	VkAllocationCallbacks instanceTerminationCallbacks;
	VkAllocationCallbacks bufferCreationCallbacks;
	VkAllocationCallbacks imageCreationCallbacks;
	VkAllocationCallbacks imageViewCreationCallbacks;
	VkAllocationCallbacks imageViewTerminationCallbacks;
	VkAllocationCallbacks imageTerminationCallbacks;
	VkAllocationCallbacks bufferTerminationCallbacks;
	VkAllocationCallbacks memoryAllocationCallbacks;
	VkAllocationCallbacks memoryDeallocationCallbacks;
	VkAllocationCallbacks sparseImageCreationCallbacks;
	VkAllocationCallbacks sparseImageTerminationCallbacks;
	VkAllocationCallbacks surfaceCreationCallbacks;
	VkAllocationCallbacks swapchainCreationCallbacks;
	VkAllocationCallbacks swapchainTerminationCallbacks;
	VkInstance instance;
	std::mutex mtxLogicalDeviceHandle;
	std::mutex mtxMemoryHandle;
	std::mutex mtxBufferHandle;
	VkDevice logicalDevices[1];
	VkDeviceQueueCreateInfo deviceQueueCreateInfo = {};
	VkInstanceCreateInfo instanceCreateInfo = {};
	VkApplicationInfo appInfo = {};
	uint32_t numberOfSupportedDevices = 1;
	VkPhysicalDevice physicalDevices[1];
	VkPhysicalDeviceProperties deviceProperties = {};
	VkPhysicalDeviceFeatures supportedDeviceFeatures = {};
	VkPhysicalDeviceFeatures desiredDeviceFeatures = {};
	uint64_t totalHeapMemorySize = 0;
	VkPhysicalDeviceMemoryProperties deviceMemoryProperties = {};
	uint32_t numQueueFamilies = 0;
	uint32_t queueCount = 0;
	uint32_t graphicQueueFamilyIndex = -1;
	uint32_t graphicQueueFamilyNumQueue = 0;
	std::vector<VkQueueFamilyProperties> queueFamilyProperties;
	VkDeviceCreateInfo logicalDeviceCreateInfo;
	uint32_t layerPropertiesCount = -1;
	std::vector<VkLayerProperties> layerProperties;
	uint32_t deviceExtensionsCount = 0;
	std::vector<VkExtensionProperties> deviceExtensions;
	std::vector<VkExtensionProperties> instanceExtensions;
	VkBufferCreateInfo bufferCreateInfo = {};
	VkBuffer *buffers;// = new VkBuffer[MAX_BUFFER_ARRAY_SIZE];
	VkImageFormatProperties imageFormatProperties = {};
	VkFormatProperties formatProperties = {};
	VkImageCreateInfo imageCreateInfo = {};
	VkImage *images;// = new VkImage[MAX_IMAGE_ARRAY_SIZE];
	VkImageViewCreateInfo imageViewCreateInfo = {};
	VkImageView *imageViews;// = new VkImageView[MAX_IMAGE_VIEW_ARRAY_SIZE];
	uint32_t memoryTypeCount = 0;
	VkMemoryAllocateInfo deviceMemoryAllocateInfo = {};
	VkDeviceMemory* memories;// = new VkDeviceMemory[MAX_DEVICE_MEMORY_ALLOCATION_ARRAY_SIZE];
	uint32_t deviceLocalVisibleMemoryTypeIndex = -1;
	VkDeviceSize memoryCommittedBytesCount = 0;
	VkMappedMemoryRange memoryFlushRange = {};
	VkMemoryRequirements imageMemoryRequirements = {};
	VkImage *sparseImages;// VkImage[MAX_SPARSE_IMAGE_ARRAY_SIZE];
	VkSparseImageMemoryRequirements *sparseImageMemoryRequirements;// = new VkSparseImageMemoryRequirements[MAX_SPARSE_IMAGE_MEMORY_REQUIREMENTS_ARRAY_SIZE];
	uint32_t sparseMemoryRequirementsCount = 0;
	VkImageCreateInfo sparseImageCreateInfo = { };
	VkSparseImageFormatProperties *physicalDeviceSparseImageFormatProperties;// = new VkSparseImageFormatProperties[MAX_SPARSE_IMAGE_FORMAT_PROPERTIES_ARRAY_SIZE];
	uint32_t physicalDeviceSparseImageFormatPropertiesCount = 0;
	VkFormat imageFormat = VK_FORMAT_R32G32B32A32_UINT;
	std::string imageFormatString = "unsigned int R32G32B32A32";
	VkQueue queue = { };
	HINSTANCE hInstance;
	HWND windowHandle;
	VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
	VkSurfaceKHR surface;
	VkSwapchainKHR swapchain;
	VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
	VkSurfaceCapabilitiesKHR surfaceCapabilities = {};
	VkBool32 physicalDeviceSurfaceSupported = VK_FALSE;
	uint32_t surfaceSupportedFormatsCount = 0;
	VkSurfaceFormatKHR *surfaceSupportedFormats;
	uint32_t surfaceSupportedPresentModesCount = 0;
	VkPresentModeKHR *surfaceSupportedPresentModes;
	uint32_t swapchainImagesCount = 0;
	VkImage *swapchainImages;
	VkPresentInfoKHR presentInfo = {};
	VkFence acquireNextImageIndexFence = {};
	VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
	VkShaderModule shaderModule = {};
	VkComputePipelineCreateInfo computePipelineCreateInfo = {};
	VkPipeline pipeline = {};
	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
	VkDescriptorSetLayout descriptorSetLayout = {};
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
	VkPipelineLayout pipelineLayout = {};
	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
	VkDescriptorPool descriptorPool = {};
	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
	VkDescriptorSet descriptorSet = {};
	VkDeviceSize bufferSize = 1024;
	uint64_t memoryAllocationSize = bufferSize;

	void init();
	void createInstance();
	void getInstanceLayers();
	void getDeviceLayers();
	void enumeratePhysicalDevices();
	void getPhysicalDevicePropertiesAndFeatures();
	void terminate();
	void createLogicalDevice();
	void getPhysicalDeviceImageFormatProperties();
	void getPhysicalDeviceSparseImageFormatProperties();
	void createBuffers();
	void createImage();
	void createImageView();
	void createSparseImage();
	void allocateDeviceMemories();
	void getImageMemoryRequirements();
	void bindBufferMemory();
	void getQueue();
	void getQueueFamilyPresentationSupport();
	void createSurface();
	void createSwapchain();
	void waitToDraw();
	void present(uint32_t swapchainPresentImageIndex);
	void createShaderModule();
	void createComputePipeline();
	void createPipelineLayout();
	void createDescriptorSetPool();
	void allocateDescriptorSet();
	void updateDescriptorSet();
	void compute();
};