#include "resource.h"
#include "Vulkan Engine.h"

#pragma comment(lib, "vulkan-1.lib")

VulkanEngine** ppUnstableInstance_img = NULL;

VulkanEngine::VulkanEngine(HINSTANCE hInstance, HWND windowHandle, VulkanEngine** ppUnstableInstance) {
	*ppUnstableInstance = this;
	ppUnstableInstance_img = ppUnstableInstance;

	this->hInstance = hInstance;
	this->windowHandle = windowHandle;

	init();
}

VulkanEngine::~VulkanEngine() {
	terminate();
}

void VulkanEngine::getInstanceExtensions() {
	vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionsCount, nullptr);

	instanceExtensions.resize(instanceExtensionsCount);

	vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionsCount, instanceExtensions.data());

	std::cout << "Instance Extensions:\n";

	for (uint32_t i = 0; i < instanceExtensionsCount; i++) {
		std::cout << "\t" << instanceExtensions[i].extensionName << "(" << std::to_string(instanceExtensions[i].specVersion) << ")\n";
	}
}

void VulkanEngine::showDeviceExtensions() {
	vkEnumerateDeviceExtensionProperties(physicalDevices[0], nullptr, &deviceExtensionsCount, nullptr);

	deviceExtensions.resize(deviceExtensionsCount);

	vkEnumerateDeviceExtensionProperties(physicalDevices[0], nullptr, &deviceExtensionsCount, deviceExtensions.data());

	std::cout << "Device Extensions:\n";

	for (int i = 0; i < deviceExtensionsCount; i++) {
		std::cout << "\t" << deviceExtensions[i].extensionName << "(" << std::to_string(deviceExtensions[0].specVersion) << ")\n";
	}
}

std::string VulkanEngine::getVersionString(uint32_t versionBitmask) {
	char versionString[128];

	uint32_t uMajorAPIVersion = versionBitmask >> 22;
	uint32_t uMinorAPIVersion = ((versionBitmask << 10) >> 10) >> 12;
	uint32_t uPatchAPIVersion = (versionBitmask << 20) >> 20;

	int majorAPIVersion = uMajorAPIVersion;
	int minorAPIVersion = uMinorAPIVersion;
	int patchAPIVersion = uPatchAPIVersion;

	sprintf_s(versionString, 128, "%d.%d.%d", majorAPIVersion, minorAPIVersion, patchAPIVersion);

	return versionString;
}

void VulkanEngine::init() {
	getInstanceExtensions();
	getInstanceLayers();

	instanceCrationAllocator = new POSIXAllocator();
	deviceCreationAllocator = new POSIXAllocator();
	deviceTerminationAllocator = new POSIXAllocator();
	instanceTerminationAllocator = new POSIXAllocator();
	bufferCreationAllocator = new POSIXAllocator();
	imageCreationAllocator = new POSIXAllocator();
	imageViewCreationAllocator = new POSIXAllocator();
	imageViewTerminationAllocator = new POSIXAllocator();
	imageTerminationAllocator = new POSIXAllocator();
	bufferTerminationAllocator = new POSIXAllocator();
	memoryAllocationAllocator = new POSIXAllocator();
	memoryDeallocationAllocator = new POSIXAllocator();
	sparseImageCreationAllocator = new POSIXAllocator();
	sparseImageTerminationAllocator = new POSIXAllocator();
	surfaceCreationAllocator = new POSIXAllocator();
	swapchainCreationAllocator = new POSIXAllocator();
	swapchainTerminationAllocator = new POSIXAllocator();

	instanceCrationCallbacks = (VkAllocationCallbacks)* instanceCrationAllocator;
	deviceCreationCallbacks = (VkAllocationCallbacks)* deviceCreationAllocator;
	deviceTerminationCallbacks = (VkAllocationCallbacks)* deviceTerminationAllocator;
	instanceTerminationCallbacks = (VkAllocationCallbacks)* instanceTerminationAllocator;
	bufferCreationCallbacks = (VkAllocationCallbacks)*bufferCreationAllocator;
	imageCreationCallbacks = (VkAllocationCallbacks)*imageCreationAllocator;
	imageViewCreationCallbacks = (VkAllocationCallbacks)*imageViewCreationAllocator;
	imageViewTerminationCallbacks = (VkAllocationCallbacks)*imageViewTerminationAllocator;
	imageTerminationCallbacks = (VkAllocationCallbacks)*imageTerminationAllocator;
	bufferTerminationCallbacks = (VkAllocationCallbacks)*bufferTerminationAllocator;
	memoryAllocationCallbacks = (VkAllocationCallbacks)*memoryAllocationAllocator;
	memoryDeallocationCallbacks = (VkAllocationCallbacks)*memoryDeallocationAllocator;
	sparseImageCreationCallbacks = (VkAllocationCallbacks)*sparseImageCreationAllocator;
	sparseImageTerminationCallbacks = (VkAllocationCallbacks)*sparseImageTerminationAllocator;
	surfaceCreationCallbacks = (VkAllocationCallbacks)*surfaceCreationAllocator;
	swapchainCreationCallbacks = (VkAllocationCallbacks)*swapchainCreationAllocator;
	swapchainTerminationCallbacks = (VkAllocationCallbacks)*swapchainTerminationAllocator;

	createInstance();
	enumeratePhysicalDevices();
	getPhysicalDevicePropertiesAndFeatures();
	createLogicalDevice();
	getDeviceLayers();
	showDeviceExtensions();
	createBuffers();
	getPhysicalDeviceImageFormatProperties();
	getPhysicalDeviceSparseImageFormatProperties();
	createImage();
	//createSparseImage();
	allocateDeviceMemories();
	getImageMemoryRequirements();
	bindBufferMemory();
	//createImageView();
	getQueue();
	getQueueFamilyPresentationSupport();
	createSurface();
	createSwapchain();
	waitToDraw();
	createShaderModule();
	createPipelineLayout();
	createComputePipeline();
	createDescriptorSetPool();
	allocateDescriptorSet();
	updateDescriptorSet();
	compute();
}

void VulkanEngine::createInstance() {
	appInfo.apiVersion = VK_API_VERSION_1_0;
	appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
	appInfo.pApplicationName = "Vulkan Test";
	appInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
	appInfo.pEngineName = "Vulkan Engine";
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = nullptr;

	std::vector<const char*> layerNames = { "VK_LAYER_LUNARG_standard_validation" };
	std::vector<const char*> extensionNames = { "VK_KHR_surface", "VK_KHR_win32_surface" };

	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pNext = nullptr;
	instanceCreateInfo.enabledExtensionCount = 2;
	instanceCreateInfo.enabledLayerCount = 1;
	instanceCreateInfo.ppEnabledExtensionNames = extensionNames.data();
	instanceCreateInfo.ppEnabledLayerNames = layerNames.data();
	instanceCreateInfo.flags = 0;

	instanceCreateInfo.pApplicationInfo = &appInfo;

	if (vkCreateInstance(&instanceCreateInfo, &instanceCrationCallbacks, &instance) == VK_SUCCESS) {
		std::cout << "Instance created successfully.\n";
	}
	else {
		throw VulkanException("Instance creation failed.");
	}
}

void VulkanEngine::getInstanceLayers() {
	vkEnumerateInstanceLayerProperties(&layerPropertiesCount, nullptr);

	std::cout << "Number of Instance Layers available to physical device:\t" << std::to_string(layerPropertiesCount) << std::endl;

	layerProperties.resize(layerPropertiesCount);

	vkEnumerateInstanceLayerProperties(&layerPropertiesCount, layerProperties.data());

	std::cout << "Instance Layers:\n";

	for (int i = 0; i < layerPropertiesCount; i++) {
		std::cout << "\tLayer #" << std::to_string(i) << std::endl;
		std::cout << "\t\tName:\t" << layerProperties[i].layerName << std::endl;
		std::cout << "\t\tSpecification Version:\t" << getVersionString(layerProperties[i].specVersion) << std::endl;
		std::cout << "\t\tImplementation Version:\t" << layerProperties[i].implementationVersion << std::endl;
		std::cout << "\t\tDescription:\t" << layerProperties[i].description << std::endl;
	}
}

void VulkanEngine::getDeviceLayers() {
	vkEnumerateDeviceLayerProperties(physicalDevices[0], &layerPropertiesCount, nullptr);

	std::cout << "Number of Device Layers available to physical device:\t" << std::to_string(layerPropertiesCount) << std::endl;

	layerProperties.resize(layerPropertiesCount);

	vkEnumerateDeviceLayerProperties(physicalDevices[0], &layerPropertiesCount, layerProperties.data());

	std::cout << "Device Layers:\n";

	for (int i = 0; i < layerPropertiesCount; i++) {
		std::cout << "\tLayer #" << std::to_string(i) << std::endl;
		std::cout << "\t\tName:\t" << layerProperties[i].layerName << std::endl;
		std::cout << "\t\tSpecification Version:\t" << getVersionString(layerProperties[i].specVersion) << std::endl;
		std::cout << "\t\tImplementation Version:\t" << layerProperties[i].implementationVersion << std::endl;
		std::cout << "\t\tDescription:\t" << layerProperties[i].description << std::endl;
	}
}

void VulkanEngine::enumeratePhysicalDevices() {
	if (vkEnumeratePhysicalDevices(instance, &numberOfSupportedDevices, physicalDevices) == VK_SUCCESS) {
		std::cout << "Physical device enumeration succeeded, first available device selected for logical device creation.\n";
	}
	else {
		throw VulkanException("Physical device enumeration failed.");
	}
}

void VulkanEngine::getPhysicalDevicePropertiesAndFeatures() {
	vkGetPhysicalDeviceProperties(physicalDevices[0], &deviceProperties);

	std::cout << "Highest Supported Vulkan Version:\t" << getVersionString(deviceProperties.apiVersion) << std::endl;

	vkGetPhysicalDeviceFeatures(physicalDevices[0], &supportedDeviceFeatures);

	std::cout << "Supports Tesselation Shader Feature:\t" << ((supportedDeviceFeatures.tessellationShader) ? ("Yes") : ("No")) << std::endl;

	std::cout << "Maximum Supported Image Size:\t" << deviceProperties.limits.maxImageDimension1D << "x" << deviceProperties.limits.maxImageDimension2D << "x" << deviceProperties.limits.maxImageDimension3D << std::endl;

	std::cout << "Minimum Memory Map Alignment:\t" << deviceProperties.limits.minMemoryMapAlignment << std::endl;

	std::cout << "Max Local Working Group Size: " << deviceProperties.limits.maxComputeWorkGroupSize[0] << "x" << deviceProperties.limits.maxComputeWorkGroupSize[1] << "x" << deviceProperties.limits.maxComputeWorkGroupSize[2] << std::endl;

	std::cout << "Max Total Working Group Invocations: " << deviceProperties.limits.maxComputeWorkGroupInvocations << std::endl;

	std::cout << "Max bound descriptor sets in a pipeline layout: " << deviceProperties.limits.maxBoundDescriptorSets << std::endl;

	std::cout << "Minimum Uniform Buffer offest alignment: " << deviceProperties.limits.minUniformBufferOffsetAlignment << std::endl;;

	std::cout << "Maximum Texel Buffer elements: " << deviceProperties.limits.maxTexelBufferElements << std::endl;;

	vkGetPhysicalDeviceMemoryProperties(physicalDevices[0], &deviceMemoryProperties);

	memoryTypeCount = deviceMemoryProperties.memoryTypeCount;

	std::cout << "Memory Type count: " << memoryTypeCount << std::endl;

	std::string flagsString;

	for (int i = 0; i < deviceMemoryProperties.memoryTypeCount; i++) {
		flagsString = "";

		int heapIndex = deviceMemoryProperties.memoryTypes[i].heapIndex;

		if ((deviceMemoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) != 0 && (deviceMemoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0)
			deviceLocalVisibleMemoryTypeIndex = i;

		if ((deviceMemoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) != 0) {
			flagsString += "DEVICE_LOCAL ";
		}

		if ((deviceMemoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0) {
			flagsString += "HOST_VISIBLE ";
		}

		if ((deviceMemoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) != 0) {
			flagsString += "HOST_COHERENT ";
		}

		if ((deviceMemoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT) != 0) {
			flagsString += "HOST_CACHED ";
		}

		if ((deviceMemoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT) != 0) {
			flagsString += "LAZILY_ALLOCATED ";
		}


		std::cout << "Memory type " << i << ":\n";
		std::cout << "\tFlags: " << flagsString << std::endl;
		std::cout << "\tHeap Index: " << heapIndex << std::endl;
		std::cout << "\tHeap Size: " << ((float)deviceMemoryProperties.memoryHeaps[heapIndex].size) / (1024 * 1024 * 1024) << "GB" << std::endl;

		//if ((((deviceMemoryProperties.memoryTypes[i].propertyFlags << (31 - VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)) >> (31 - VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)) >> VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) == 0)
		totalHeapMemorySize += deviceMemoryProperties.memoryHeaps[heapIndex].size;
	}

	if (deviceLocalVisibleMemoryTypeIndex == -1)
		throw VulkanException("No desired memory type found.");

	std::cout << "\nTotal Heaps Size:\t" << std::to_string((float)totalHeapMemorySize / (1024 * 1024 * 1024)) << "GB" << std::endl;
}

void VulkanEngine::terminate() {
	VulkanEngine *instanceToTerminate = NULL;

	//if (*ppUnstableInstance_img != NULL)
	//	instanceToTerminate = *ppUnstableInstance_img;
	//else
	instanceToTerminate = this;

	//Shutting Down
	//releasing memory
	delete instanceToTerminate->deviceQueueCreateInfo.pQueuePriorities;


	//Terminte other threads
	//Working current thread possible command buffer generation through flags

	std::cout << "Waiting for logical device to return...\n";

	bool lockCreated = false;

	std::unique_lock<std::mutex>* pLock = NULL;

	pLock = new std::unique_lock<std::mutex>(instanceToTerminate->mtxLogicalDeviceHandle);
	lockCreated = true;

	vkDeviceWaitIdle(instanceToTerminate->logicalDevices[0]);

	if (lockCreated) {
		pLock->unlock();
	}

	if (lockCreated)
	{
		pLock->release();
	}

	vkDestroySwapchainKHR(logicalDevices[0], swapchain, &swapchainTerminationCallbacks);
	std::cout << "Swapchain destroyed succesfully." << std::endl;

	vkDestroyFence(logicalDevices[0], acquireNextImageIndexFence, nullptr);
	std::cout << "Swapchain next image index wait fence destroyed." << std::endl;


	std::unique_lock<std::mutex> *pLock4 = NULL;

	lockCreated = false;


	pLock4 = new std::unique_lock<std::mutex>(instanceToTerminate->mtxMemoryHandle);

	lockCreated = true;

	vkFreeMemory(instanceToTerminate->logicalDevices[0], instanceToTerminate->memories[0], &instanceToTerminate->memoryDeallocationCallbacks);
	std::cout << "Device Memory released.\n";

	if (lockCreated)
	{

		pLock4->unlock();

	}
	if (lockCreated)
	{

		pLock4->release();

	}


	//vkDestroyImageView(instanceToTerminate->logicalDevices[0], instanceToTerminate->imageViews[0], &instanceToTerminate->imageViewTerminationCallbacks);
	//std::cout << "Image View destroyed.\n";


	//vkDestroyImage(logicalDevices[0], sparseImages[0], &sparseImageTerminationCallbacks);
	//std::cout << "Sparse Image destroyed.\n";

	vkDestroyImage(instanceToTerminate->logicalDevices[0], instanceToTerminate->images[0], &instanceToTerminate->imageTerminationCallbacks);
	std::cout << "Image destroyed.\n";


	vkDestroyBuffer(instanceToTerminate->logicalDevices[0], instanceToTerminate->buffers[0], &instanceToTerminate->bufferTerminationCallbacks);
	std::cout << "Buffer destroyed.\n";


	std::unique_lock<std::mutex> *pLock3 = NULL;

	lockCreated = false;


	pLock3 = new std::unique_lock<std::mutex>(instanceToTerminate->mtxLogicalDeviceHandle);
	lockCreated = true;

	vkDestroyDevice(instanceToTerminate->logicalDevices[0], &instanceToTerminate->deviceTerminationCallbacks);
	std::cout << "Logical Device destroyed.\n";

	if (lockCreated)
	{

		pLock3->unlock();

	}
	if (lockCreated)
	{

		pLock3->release();

	}

	vkDestroyInstance(instanceToTerminate->instance, &instanceToTerminate->instanceTerminationCallbacks);
	std::cout << "Instance destroyed.\n";

	getchar();

}

void VulkanEngine::createLogicalDevice() {
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[0], &numQueueFamilies, nullptr);

	std::cout << "Number of Queue Families:\t" << std::to_string(numQueueFamilies) << std::endl;



	queueFamilyProperties.resize(numQueueFamilies);

	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[0], &numQueueFamilies, queueFamilyProperties.data());

	for (int i = 0; i < numQueueFamilies; i++) {
		std::string queueFlagsString = "";

		if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT != 0) {
			queueFlagsString += "GRAPHICS ";
		}

		if (queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT != 0) {
			queueFlagsString += "COMPUTE ";
		}

		if (queueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT != 0) {
			queueFlagsString += "TRANSFER ";
		}

		if (queueFamilyProperties[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT != 0) {
			queueFlagsString += "SPARSE-BINDING ";
		}


		std::cout << std::endl;
		std::cout << "Queue Family " << i << ":\n";
		std::cout << "\t Queue Family Queue Amount: " << queueFamilyProperties[i].queueCount << std::endl;
		std::cout << "\t Queue Family Min Image Transfer Granularity Width: " << queueFamilyProperties[i].minImageTransferGranularity.width << std::endl;
		std::cout << "\t Queue Family Min Image Transfer Granularity Height: " << queueFamilyProperties[i].minImageTransferGranularity.height << std::endl;
		std::cout << "\t Queue Family Min Image Transfer Granularity Depth: " << queueFamilyProperties[i].minImageTransferGranularity.depth << std::endl;
		std::cout << "\t Queue Family Flags: " << queueFlagsString << std::endl;
		std::cout << "\t Queue Family Timestamp Valid Bits: " << queueFamilyProperties[i].timestampValidBits << "Bits" << std::endl;


		if (queueFamilyProperties[i].queueFlags % 2 == 1 && graphicQueueFamilyIndex == -1) {
			graphicQueueFamilyIndex = i;
			graphicQueueFamilyNumQueue = queueFamilyProperties[i].queueCount;

		}

		queueCount += queueFamilyProperties[i].queueCount;
	}

	if (graphicQueueFamilyIndex == -1 || graphicQueueFamilyNumQueue == 0) {
		throw VulkanException("Cannot find queue family that supports graphical rendering.");
	}

	std::cout << "Total Device Queue Count:\t" << std::to_string(queueCount) << std::endl;



	deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	deviceQueueCreateInfo.flags = 0;
	deviceQueueCreateInfo.pNext = nullptr;
	deviceQueueCreateInfo.queueFamilyIndex = graphicQueueFamilyIndex;
	deviceQueueCreateInfo.queueCount = graphicQueueFamilyNumQueue;
	deviceQueueCreateInfo.pQueuePriorities = new const float[1]{ 1.0f };


	desiredDeviceFeatures.tessellationShader = VK_TRUE;
	desiredDeviceFeatures.geometryShader = VK_TRUE;
	desiredDeviceFeatures.multiDrawIndirect = supportedDeviceFeatures.multiDrawIndirect;

	std::vector<const char*> extensionNames = { "VK_KHR_swapchain" };

	logicalDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	logicalDeviceCreateInfo.flags = 0;
	logicalDeviceCreateInfo.pNext = nullptr;
	logicalDeviceCreateInfo.queueCreateInfoCount = 1;
	logicalDeviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
	logicalDeviceCreateInfo.enabledExtensionCount = 1;
	logicalDeviceCreateInfo.enabledLayerCount = 0;
	logicalDeviceCreateInfo.ppEnabledExtensionNames = extensionNames.data();
	logicalDeviceCreateInfo.ppEnabledLayerNames = nullptr;
	logicalDeviceCreateInfo.pEnabledFeatures = &desiredDeviceFeatures;


	if (vkCreateDevice(physicalDevices[0], &logicalDeviceCreateInfo, &deviceCreationCallbacks, logicalDevices) == VK_SUCCESS) {
		std::cout << "Logical device creation succeeded.\n";
	}
	else {
		throw VulkanException("Logical Device creation failed.\n");
	}
}

void VulkanEngine::createBuffers() {

	buffers = new VkBuffer[2];

	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.pNext = nullptr;
	bufferCreateInfo.flags = 0;
	bufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	bufferCreateInfo.size = bufferSize;

	if (vkCreateBuffer(logicalDevices[0], &bufferCreateInfo, &bufferCreationCallbacks, buffers) == VK_SUCCESS) {
		std::cout << "Buffer object created successfully.\n";
	}
	else {
		throw VulkanException("Buffer creation failed.");
	}

	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.pNext = nullptr;
	bufferCreateInfo.flags = 0;
	bufferCreateInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	bufferCreateInfo.size = bufferSize;

	if (vkCreateBuffer(logicalDevices[0], &bufferCreateInfo, &bufferCreationCallbacks, buffers + 1) == VK_SUCCESS) {
		std::cout << "Buffer object created successfully.\n";
	}
	else {
		throw VulkanException("Buffer creation failed.");
	}
}

void VulkanEngine::createImage() {

	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.pNext = nullptr;
	imageCreateInfo.flags = VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	imageCreateInfo.extent.width = 1024;
	imageCreateInfo.extent.height = 1024;
	imageCreateInfo.extent.depth = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageCreateInfo.queueFamilyIndexCount = 0;
	imageCreateInfo.pQueueFamilyIndices = nullptr;

	images = new VkImage[1];

	VkResult result = vkCreateImage(logicalDevices[0], &imageCreateInfo, &imageCreationCallbacks, images);

	switch (result) {
	case VK_SUCCESS:
		std::cout << "Image (" << 1024 << "x" << 1024 << ") created successfully.\n";
		break;
	default:
		throw VulkanException("Image creation failed.");
	}
}

void VulkanEngine::getPhysicalDeviceImageFormatProperties() {
	vkGetPhysicalDeviceFormatProperties(physicalDevices[0], imageFormat, &formatProperties);

	std::string bufferFormatFeatureFlagsString = "";
	std::string linearTilingFormatFeatureFlagsString = "";
	std::string optimalTilingFormatFeatureFlagsString = "";

#pragma region format features bits  
	if (formatProperties.bufferFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT != 0) {
		bufferFormatFeatureFlagsString += "SAMPLED_IMAGED ";
	}

	if (formatProperties.bufferFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT != 0) {
		bufferFormatFeatureFlagsString += "SAMPLED_IMAGE_FILTER_LINEAR ";
	}

	if (formatProperties.bufferFeatures & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT != 0) {
		bufferFormatFeatureFlagsString += "STORAGE_IMAGE ";
	}

	if (formatProperties.bufferFeatures & VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT != 0) {
		bufferFormatFeatureFlagsString += "STORAGE_IMAGE_ATOMIC ";
	}

	if (formatProperties.bufferFeatures & VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT != 0) {
		bufferFormatFeatureFlagsString += "UNIFORM_TEXEL_BUFFER ";
	}

	if (formatProperties.bufferFeatures & VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT != 0) {
		bufferFormatFeatureFlagsString += "STORAGE_TEXEL_BUFFER ";
	}

	if (formatProperties.bufferFeatures & VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT != 0) {
		bufferFormatFeatureFlagsString += "STORAGE_TEXEL_BUFFER_ATOMIC ";
	}

	if (formatProperties.bufferFeatures & VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT != 0) {
		bufferFormatFeatureFlagsString += "VERTEX_BUFFER ";
	}

	if (formatProperties.bufferFeatures & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT != 0) {
		bufferFormatFeatureFlagsString += "COLOR_ATTACHMENT ";
	}

	if (formatProperties.bufferFeatures & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT != 0) {
		bufferFormatFeatureFlagsString += "COLOR_ATTACHMENT_BLEND ";
	}

	if (formatProperties.bufferFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT != 0) {
		bufferFormatFeatureFlagsString += "DEPTH_STENCIL_ATTACHMENT ";
	}

	if (formatProperties.bufferFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT != 0) {
		bufferFormatFeatureFlagsString += "BLIT_SRC ";
	}

	if (formatProperties.bufferFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT != 0) {
		bufferFormatFeatureFlagsString += "BLIT_DST ";
	}

	if (formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT != 0) {
		linearTilingFormatFeatureFlagsString += "SAMPLED_IMAGED ";
	}

	if (formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT != 0) {
		linearTilingFormatFeatureFlagsString += "SAMPLED_IMAGE_FILTER_LINEAR ";
	}

	if (formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT != 0) {
		linearTilingFormatFeatureFlagsString += "STORAGE_IMAGE ";
	}

	if (formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT != 0) {
		linearTilingFormatFeatureFlagsString += "STORAGE_IMAGE_ATOMIC ";
	}

	if (formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT != 0) {
		linearTilingFormatFeatureFlagsString += "UNIFORM_TEXEL_BUFFER ";
	}

	if (formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT != 0) {
		linearTilingFormatFeatureFlagsString += "STORAGE_TEXEL_BUFFER ";
	}

	if (formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT != 0) {
		linearTilingFormatFeatureFlagsString += "STORAGE_TEXEL_BUFFER_ATOMIC ";
	}

	if (formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT != 0) {
		linearTilingFormatFeatureFlagsString += "VERTEX_BUFFER ";
	}

	if (formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT != 0) {
		linearTilingFormatFeatureFlagsString += "COLOR_ATTACHMENT ";
	}

	if (formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT != 0) {
		linearTilingFormatFeatureFlagsString += "COLOR_ATTACHMENT_BLEND ";
	}

	if (formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT != 0) {
		linearTilingFormatFeatureFlagsString += "DEPTH_STENCIL_ATTACHMENT ";
	}

	if (formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT != 0) {
		linearTilingFormatFeatureFlagsString += "BLIT_SRC ";
	}

	if (formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT != 0) {
		linearTilingFormatFeatureFlagsString += "BLIT_DST ";
	}

	if (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT != 0) {
		optimalTilingFormatFeatureFlagsString += "SAMPLED_IMAGED ";
	}

	if (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT != 0) {
		optimalTilingFormatFeatureFlagsString += "SAMPLED_IMAGE_FILTER_LINEAR ";
	}

	if (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT != 0) {
		optimalTilingFormatFeatureFlagsString += "STORAGE_IMAGE ";
	}

	if (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT != 0) {
		optimalTilingFormatFeatureFlagsString += "STORAGE_IMAGE_ATOMIC ";
	}

	if (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT != 0) {
		optimalTilingFormatFeatureFlagsString += "UNIFORM_TEXEL_BUFFER ";
	}

	if (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT != 0) {
		optimalTilingFormatFeatureFlagsString += "STORAGE_TEXEL_BUFFER ";
	}

	if (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT != 0) {
		optimalTilingFormatFeatureFlagsString += "STORAGE_TEXEL_BUFFER_ATOMIC ";
	}

	if (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT != 0) {
		optimalTilingFormatFeatureFlagsString += "VERTEX_BUFFER ";
	}

	if (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT != 0) {
		optimalTilingFormatFeatureFlagsString += "COLOR_ATTACHMENT ";
	}

	if (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT != 0) {
		optimalTilingFormatFeatureFlagsString += "COLOR_ATTACHMENT_BLEND ";
	}

	if (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT != 0) {
		optimalTilingFormatFeatureFlagsString += "DEPTH_STENCIL_ATTACHMENT ";
	}

	if (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT != 0) {
		optimalTilingFormatFeatureFlagsString += "BLIT_SRC ";
	}

	if (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT != 0) {
		optimalTilingFormatFeatureFlagsString += "BLIT_DST ";
	}
#pragma endregion

	std::cout << "Physical Device format support features pertaining to format " << imageFormatString << std::endl;
	std::cout << "\tBuffer format features flags: " << bufferFormatFeatureFlagsString << std::endl;
	std::cout << "\tLinear tiling format feature flags: " << linearTilingFormatFeatureFlagsString << std::endl;
	std::cout << "\tOptimal tiling format feature flags: " << optimalTilingFormatFeatureFlagsString << std::endl;

	VkResult result = vkGetPhysicalDeviceImageFormatProperties(physicalDevices[0], VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_LINEAR, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT, &imageFormatProperties);
	float maxResourceSizeGB;
	VkSampleCountFlags sampleCountFlags;
	uint32_t sampleCount;

	switch (result) {
	case VK_SUCCESS:
		std::cout << "Physical Device support extent pertaining to image format R8G8B8A8_UNORM (2D) with optimal tiling usable as source and destination of transfer commands, allowing image view creation off itself:\n";

		maxResourceSizeGB = ((float)imageFormatProperties.maxResourceSize) / (1024 * 1024 * 1024);

		std::cout << "\tMax Array Layers: " << imageFormatProperties.maxArrayLayers << std::endl;
		std::cout << "\tMax MimMap Levels: " << imageFormatProperties.maxMipLevels << std::endl;
		std::cout << "\tMax Resource Size: " << maxResourceSizeGB << "GB" << std::endl;
		std::cout << "\tMax Sample Count: " << imageFormatProperties.sampleCounts << std::endl;
		break;
	case VK_ERROR_OUT_OF_HOST_MEMORY:
		throw VulkanException("Couldn't fetch image format properties, out of host memory.");
		break;
	case VK_ERROR_OUT_OF_DEVICE_MEMORY:
		throw VulkanException("Couldn't fetch image format properties, out of device memory.");
		break;
	case VK_ERROR_FORMAT_NOT_SUPPORTED:
		throw VulkanException("Format not supported.");
		break;
	}
}

void VulkanEngine::createImageView() {
	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCreateInfo.pNext = nullptr;
	imageViewCreateInfo.flags = 0;
	imageViewCreateInfo.image = images[0];
	imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageViewCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;
	imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
	imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
	imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;
	imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	imageViewCreateInfo.subresourceRange.levelCount = 1;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = 1;

	imageViews = new VkImageView[1];

	VkResult result = vkCreateImageView(logicalDevices[0], &imageViewCreateInfo, &imageViewCreationCallbacks, imageViews);

	switch (result) {
	case VK_SUCCESS:
		std::cout << "Image View creation succeeded.\n";
		break;
	default:
		throw VulkanException("Image View creation failed.");
	}
}

void VulkanEngine::allocateDeviceMemories() {
	deviceMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	deviceMemoryAllocateInfo.pNext = nullptr;
	deviceMemoryAllocateInfo.allocationSize = memoryAllocationSize; //1KB
	deviceMemoryAllocateInfo.memoryTypeIndex = deviceLocalVisibleMemoryTypeIndex;

	VkResult result;

	memories = new VkDeviceMemory[2];

	std::unique_lock<std::mutex> lock5(mtxMemoryHandle);
	{
		result = vkAllocateMemory(logicalDevices[0], &deviceMemoryAllocateInfo, &memoryAllocationCallbacks, memories);
	}
	lock5.unlock();
	lock5.release();

	if (result != VK_SUCCESS)
		throw VulkanException("Couldn't allocate memory.");

	std::unique_lock<std::mutex> lock13(mtxMemoryHandle);
	{
		result = vkAllocateMemory(logicalDevices[0], &deviceMemoryAllocateInfo, &memoryAllocationCallbacks, memories + 1);
	}
	lock13.unlock();
	lock13.release();

	if (result != VK_SUCCESS)
		throw VulkanException("Couldn't allocate memory.");

	std::unique_lock<std::mutex> *pLock6 = nullptr;

	std::unique_lock<std::mutex> *pLock7 = nullptr;;

	void **mappedMemory = new void*;

	switch (result) {
	case VK_SUCCESS:
		std::cout << "Device Memory Allocation successful." << std::endl;

		vkGetDeviceMemoryCommitment(logicalDevices[0], memories[0], &memoryCommittedBytesCount);

		std::cout << "Device Memory Allocation bytes commited:\t" << ((float)memoryCommittedBytesCount) / (1024) << "KB" << std::endl;


		pLock6 = new std::unique_lock<std::mutex>(mtxMemoryHandle);
		{
			result = vkMapMemory(logicalDevices[0], memories[0], 0, memoryAllocationSize, 0, mappedMemory);
		}
		pLock6->unlock();
		pLock6->release();

		switch (result) {
		case VK_SUCCESS:
			std::cout << "Memory successfully mapped." << std::endl;

			break;
		default:
			throw VulkanException("Memory mapping failed.");
		}

		std::cout << "Writing to visible mapped memory... (1KB)" << std::endl;

		memset(*mappedMemory, 3, memoryAllocationSize);

		std::cout << "Writing finished." << std::endl;

		memoryFlushRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		memoryFlushRange.pNext = nullptr;
		memoryFlushRange.size = VK_WHOLE_SIZE;
		memoryFlushRange.offset = 0;
		memoryFlushRange.memory = memories[0];

		std::cout << "Flushing mapped memory... (1KB)" << std::endl;
		result = vkFlushMappedMemoryRanges(logicalDevices[0], 1, &memoryFlushRange);

		std::cout << "Device Memory Allocation bytes commited (after Flush):\t" << ((float)memoryCommittedBytesCount) / (1024) << "KB" << std::endl;

		switch (result) {
		case VK_SUCCESS:
			std::cout << "Flushing sucessfully finished." << std::endl;
			break;
		default:
			throw VulkanException("Mapped Memory Flushing failed.");
		}

		pLock7 = new std::unique_lock<std::mutex>(mtxMemoryHandle);
		{
			vkUnmapMemory(logicalDevices[0], memories[0]);

			std::cout << "Memory unmapped.\n";
		}
		pLock7->unlock();
		pLock7->release();

		/*std::cout << "Invalidating mapped memory... (1KB)" << std::endl;
		result = vkInvalidateMappedMemoryRanges(logicalDevices[0], 1, &memoryFlushRange);*/

		//switch (result) {
		//case VK_SUCCESS:
		//	std::cout << "Invalidating sucessfully finished." << std::endl;
		//	break;
		//default:
		//	throw VulkanException("Mapped Memory Invalidation failed.");
		//}


		break;
	default:
		throw VulkanException("Cannot allocate memory.");
	}
}

void VulkanEngine::getImageMemoryRequirements() {
	vkGetImageMemoryRequirements(logicalDevices[0], images[0], &imageMemoryRequirements);

	std::cout << "Image Memory Requirements:\n";

	std::cout << "\tImage Memory required size: " << ((float)imageMemoryRequirements.size) / (1024) << "KB" << std::endl;
	std::cout << "\tImage Memory required alignment: " << imageMemoryRequirements.alignment << std::endl;

	for (uint32_t i = 0; i < sizeof(imageMemoryRequirements.memoryTypeBits); i++) {
		if (((imageMemoryRequirements.memoryTypeBits >> i) % 2) == 1)
			std::cout << "\tImage supports memory type index " << i << std::endl;
	}

	bool compatibleAllocatedMemory = (imageMemoryRequirements.memoryTypeBits & (1 << deviceLocalVisibleMemoryTypeIndex)) != 0;

	if (!compatibleAllocatedMemory)
		throw VulkanException("Image doesn't support mapped memory type.");
	//else
	/*std::cout << "Image supports Memory Mapping for the selected memory type (index "<< deviceLocalExclusiveMemoryTypeIndex <<").\n";*/
}

void VulkanEngine::bindBufferMemory() {
	VkResult result;

	std::unique_lock<std::mutex> lock8(mtxBufferHandle);
	{
		result = vkBindBufferMemory(logicalDevices[0], buffers[0], memories[0], 0);
	}
	lock8.unlock();
	lock8.release();

	if (result == VK_SUCCESS)
	{
		std::cout << "Memory bound to buffer successfully.\n";
	}
	else
		throw VulkanException("Memory binding to buffer failed.");


	std::unique_lock<std::mutex> lock14(mtxBufferHandle);
	{
		result = vkBindBufferMemory(logicalDevices[0], buffers[1], memories[1], 0);
	}
	lock14.unlock();
	lock14.release();
}

void VulkanEngine::createSparseImage() {
	sparseImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	sparseImageCreateInfo.pNext = nullptr;
	sparseImageCreateInfo.flags = VK_IMAGE_CREATE_SPARSE_RESIDENCY_BIT | VK_IMAGE_CREATE_SPARSE_BINDING_BIT;
	sparseImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	sparseImageCreateInfo.format = VK_FORMAT_R32G32B32A32_UINT;
	sparseImageCreateInfo.extent.width = 4096;
	sparseImageCreateInfo.extent.height = 4096;
	sparseImageCreateInfo.extent.depth = 1;
	sparseImageCreateInfo.arrayLayers = 1;
	sparseImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	sparseImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	sparseImageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	sparseImageCreateInfo.mipLevels = imageFormatProperties.maxMipLevels;
	sparseImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	sparseImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	sparseImageCreateInfo.queueFamilyIndexCount = 0;
	sparseImageCreateInfo.pQueueFamilyIndices = nullptr;

	sparseImages = new VkImage[1];

	VkResult result = vkCreateImage(logicalDevices[0], &sparseImageCreateInfo, &imageCreationCallbacks, sparseImages);

	switch (result) {
	case VK_SUCCESS:
		std::cout << "Sparse Image (" << 2048 << "x" << 2048 << ", with different other parameters) created successfully.\n";
		break;
	default:
		throw VulkanException("Sparse Image creation failed.");
	}

	vkGetImageSparseMemoryRequirements(logicalDevices[0], sparseImages[0], &sparseMemoryRequirementsCount, nullptr);

	sparseImageMemoryRequirements = new VkSparseImageMemoryRequirements[sparseMemoryRequirementsCount];

	vkGetImageSparseMemoryRequirements(logicalDevices[0], sparseImages[0], &sparseMemoryRequirementsCount, sparseImageMemoryRequirements);

	std::cout << "Sparse Image Memory Requirements:\n";

	for (uint32_t i = 0; i < sparseMemoryRequirementsCount; i++) {
		VkSparseImageMemoryRequirements sparseImageMemoryRequirementsElement = sparseImageMemoryRequirements[i];

		std::string aspectMaskString = "";

		if ((sparseImageMemoryRequirementsElement.formatProperties.aspectMask & VK_IMAGE_ASPECT_COLOR_BIT) != 0) {
			aspectMaskString += "COLOR_BIT ";
		}

		if ((sparseImageMemoryRequirementsElement.formatProperties.aspectMask & VK_IMAGE_ASPECT_DEPTH_BIT) != 0) {
			aspectMaskString += "DEPTH_BIT ";
		}

		if ((sparseImageMemoryRequirementsElement.formatProperties.aspectMask & VK_IMAGE_ASPECT_STENCIL_BIT) != 0) {
			aspectMaskString += "STENCIL_BIT ";
		}

		if ((sparseImageMemoryRequirementsElement.formatProperties.aspectMask & VK_IMAGE_ASPECT_METADATA_BIT) != 0) {
			aspectMaskString += "METADATA_BIT ";
		}

		std::cout << "\tApplied to following aspects: " << aspectMaskString << std::endl;

		std::cout << "\tImage Granularity Width: " << sparseImageMemoryRequirementsElement.formatProperties.imageGranularity.width << std::endl;
		std::cout << "\tImage Granularity Height: " << sparseImageMemoryRequirementsElement.formatProperties.imageGranularity.height << std::endl;
		std::cout << "\tImage Granularity Depth: " << sparseImageMemoryRequirementsElement.formatProperties.imageGranularity.depth << std::endl;

		std::string flagsString = "";

		if ((sparseImageMemoryRequirementsElement.formatProperties.flags & VK_SPARSE_IMAGE_FORMAT_SINGLE_MIPTAIL_BIT) != 0) {
			flagsString += "SINGLE_MIPTAIL ";
		}

		if ((sparseImageMemoryRequirementsElement.formatProperties.flags & VK_SPARSE_IMAGE_FORMAT_ALIGNED_MIP_SIZE_BIT) != 0) {
			flagsString += "ALIGNED_MIP_SIZE ";
		}

		if ((sparseImageMemoryRequirementsElement.formatProperties.flags & VK_SPARSE_IMAGE_FORMAT_NONSTANDARD_BLOCK_SIZE_BIT) != 0) {
			flagsString += "NONSTANDARD_BLOCK_SIZE ";
		}

		std::cout << "\tFlags: " << flagsString << std::endl;

		std::cout << "\tFirst Mip-Tail Level: " << sparseImageMemoryRequirementsElement.imageMipTailFirstLod << std::endl;
		std::cout << "\tFirst Mip-Tail Size: " << sparseImageMemoryRequirementsElement.imageMipTailSize << std::endl;
		std::cout << "\tFirst Mip-Tail Offset (in memory binding region): " << sparseImageMemoryRequirementsElement.imageMipTailOffset << std::endl;
		std::cout << "\tFirst Mip-Tail Stride (between deviant miptails of array): " << sparseImageMemoryRequirementsElement.imageMipTailStride << std::endl;

	}
}

void VulkanEngine::getPhysicalDeviceSparseImageFormatProperties() {
	VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	VkFormat format = VK_FORMAT_R16G16B16A16_UNORM;
	VkSampleCountFlagBits samplesCount = VK_SAMPLE_COUNT_1_BIT;
	VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
	VkImageType type = VK_IMAGE_TYPE_2D;


	vkGetPhysicalDeviceSparseImageFormatProperties(physicalDevices[0], format, type, samplesCount, usageFlags, tiling, &physicalDeviceSparseImageFormatPropertiesCount, nullptr);

	physicalDeviceSparseImageFormatProperties = new VkSparseImageFormatProperties[physicalDeviceSparseImageFormatPropertiesCount];

	vkGetPhysicalDeviceSparseImageFormatProperties(physicalDevices[0], format, type, samplesCount, usageFlags, tiling, &physicalDeviceSparseImageFormatPropertiesCount, physicalDeviceSparseImageFormatProperties);

	std::cout << "Physical Device support extent pertaining to sparse image format VK_FORMAT_R16G16B16A16_UNORM(2D) with optimal tiling usable as source and destination of transfer commands, as well as sampling, and allowing image view creation off itself, with one multisampling:\n";

	for (uint32_t i = 0; i < physicalDeviceSparseImageFormatPropertiesCount; i++) {

		std::string aspectMaskString = "";

		if ((physicalDeviceSparseImageFormatProperties[i].aspectMask & VK_IMAGE_ASPECT_COLOR_BIT) != 0) {
			aspectMaskString += "COLOR_BIT ";
		}

		if ((physicalDeviceSparseImageFormatProperties[i].aspectMask & VK_IMAGE_ASPECT_DEPTH_BIT) != 0) {
			aspectMaskString += "DEPTH_BIT ";
		}

		if ((physicalDeviceSparseImageFormatProperties[i].aspectMask & VK_IMAGE_ASPECT_STENCIL_BIT) != 0) {
			aspectMaskString += "STENCIL_BIT ";
		}

		if ((physicalDeviceSparseImageFormatProperties[i].aspectMask & VK_IMAGE_ASPECT_METADATA_BIT) != 0) {
			aspectMaskString += "METADATA_BIT ";
		}

		std::cout << "\tApplied to following aspects: " << aspectMaskString << std::endl;

		std::cout << "\tImage Granularity Width: " << physicalDeviceSparseImageFormatProperties[i].imageGranularity.width << std::endl;
		std::cout << "\tImage Granularity Height: " << physicalDeviceSparseImageFormatProperties[i].imageGranularity.height << std::endl;
		std::cout << "\tImage Granularity Depth: " << physicalDeviceSparseImageFormatProperties[i].imageGranularity.depth << std::endl;

		std::string flagsString = "";

		if ((physicalDeviceSparseImageFormatProperties[i].flags & VK_SPARSE_IMAGE_FORMAT_SINGLE_MIPTAIL_BIT) != 0) {
			flagsString += "SINGLE_MIPTAIL ";
		}

		if ((physicalDeviceSparseImageFormatProperties[i].flags & VK_SPARSE_IMAGE_FORMAT_ALIGNED_MIP_SIZE_BIT) != 0) {
			flagsString += "ALIGNED_MIP_SIZE ";
		}

		if ((physicalDeviceSparseImageFormatProperties[i].flags & VK_SPARSE_IMAGE_FORMAT_NONSTANDARD_BLOCK_SIZE_BIT) != 0) {
			flagsString += "NONSTANDARD_BLOCK_SIZE ";
		}

		std::cout << "\tFlags: " << flagsString << std::endl;
	}
}

void VulkanEngine::getQueue() {
	vkGetDeviceQueue(logicalDevices[0], graphicQueueFamilyIndex, 0, &queue);

	if (queue == VK_NULL_HANDLE) {
		throw "Couldn't obtain queue from device.";
	}
	else {
		std::cout << "Queue obtained successfully.\n";
	}
}

void VulkanEngine::getQueueFamilyPresentationSupport() {
	if (vkGetPhysicalDeviceWin32PresentationSupportKHR(physicalDevices[0], graphicQueueFamilyIndex) == VK_TRUE) {
		std::cout << "Selected Graphical Queue Family supports presentation." << std::endl;
	}
	else
		throw VulkanException("Selected queue family (graphical) doesn't support presentation.");
}

void VulkanEngine::createSurface() {
	VkResult result;

	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.pNext = nullptr;
	surfaceCreateInfo.flags = 0;
	surfaceCreateInfo.hinstance = hInstance;
	surfaceCreateInfo.hwnd = windowHandle;

	result = vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, &surfaceCreationCallbacks, &surface);

	if (result == VK_SUCCESS) {
		std::cout << "Surface created and associated with the window." << std::endl;
	}
	else
		throw VulkanException("Failed to create and/or assocate surface with the window");
}

void VulkanEngine::createSwapchain() {
	VkResult surfaceSupportResult = vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevices[0], graphicQueueFamilyIndex, surface, &physicalDeviceSurfaceSupported);

	if (surfaceSupportResult == VK_SUCCESS && physicalDeviceSurfaceSupported == VK_TRUE) {
		std::cout << "Physical Device selected graphical queue family supports presentation." << std::endl;
	}
	else
		throw VulkanException("Physical Device selected graphical queue family doesn't support presentation.");

	VkResult surfaceCapabilitiesResult = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevices[0], surface, &surfaceCapabilities);

	if (surfaceCapabilitiesResult == VK_SUCCESS) {
		std::cout << "Successfully fetched device surface capabilities." << std::endl;

		std::cout << "Minimum swap chain image count: " << surfaceCapabilities.minImageCount << std::endl;
		std::cout << "Maximum swap chain image count: " << surfaceCapabilities.maxImageCount << std::endl;
	}
	else
		throw VulkanException("Couldn't fetch device surface capabilities.");

	if (vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevices[0], surface, &surfaceSupportedFormatsCount, nullptr) != VK_SUCCESS)
		throw VulkanException("Couldn't get surface supported formats.");

	surfaceSupportedFormats = new VkSurfaceFormatKHR[surfaceSupportedFormatsCount];

	if (vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevices[0], surface, &surfaceSupportedFormatsCount, surfaceSupportedFormats) != VK_SUCCESS)
		throw VulkanException("Couldn't get surface supported formats.");


	uint32_t supportedFormatColorSpacePairIndex = -1;
	uint32_t supportedPresentModeIndex = -1;

	for (int i = 0; i < surfaceSupportedFormatsCount; i++) {
		if (surfaceSupportedFormats[i].format == VK_FORMAT_R8G8B8A8_SRGB && surfaceSupportedFormats[i].colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
		{
			supportedFormatColorSpacePairIndex = i;
			break;
		}
	}

	if (supportedFormatColorSpacePairIndex == -1)
		throw VulkanException("Couldn't find R8G8B8A8_SRGB and SRGB_NONLINEAR format/color-space pair in supported formats/color-spaces.");

	if (vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevices[0], surface, &surfaceSupportedPresentModesCount, nullptr) != VK_SUCCESS) {
		throw VulkanException("Couldn't get surface supported presentation modes.");
	}

	surfaceSupportedPresentModes = new VkPresentModeKHR[surfaceSupportedPresentModesCount];

	if (vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevices[0], surface, &surfaceSupportedPresentModesCount, surfaceSupportedPresentModes) != VK_SUCCESS) {
		throw VulkanException("Couldn't get surface supported presentation modes.");
	}

	for (int i = 0; i < surfaceSupportedPresentModesCount; i++) {
		if (surfaceSupportedPresentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR) {
			supportedPresentModeIndex = i;
			break;
		}
	}

	if (surfaceSupportedPresentModesCount == -1)
		throw VulkanException("Couldn't find IMMEDIATE present mode in supported present modes.");

	swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCreateInfo.pNext = nullptr;
	swapchainCreateInfo.flags = 0;
	swapchainCreateInfo.surface = surface;
	swapchainCreateInfo.minImageCount = (2 >= surfaceCapabilities.minImageCount && 2 <= surfaceCapabilities.maxImageCount) ? (2) : (surfaceCapabilities.minImageCount);
	swapchainCreateInfo.imageFormat = surfaceSupportedFormats[supportedFormatColorSpacePairIndex].format;
	swapchainCreateInfo.imageColorSpace = surfaceSupportedFormats[supportedFormatColorSpacePairIndex].colorSpace;
	swapchainCreateInfo.imageExtent.width = surfaceCapabilities.currentExtent.width;
	swapchainCreateInfo.imageExtent.height = surfaceCapabilities.currentExtent.height;
	swapchainCreateInfo.imageArrayLayers = 1;

	if ((surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) != 0 && (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_STORAGE_BIT) != 0)
	{
		std::cout << "Surface supports USAGE_STORAGE and COLOR_ATTACHMENT usage bits." << std::endl;
	}
	else
		throw VulkanException("Surface doesn't support USAGE_STORAGE and/or COLOR_ATTACHMENT usage bits.");

	swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
	swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapchainCreateInfo.pQueueFamilyIndices = nullptr;
	swapchainCreateInfo.queueFamilyIndexCount = 0;
	swapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchainCreateInfo.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
	swapchainCreateInfo.clipped = VK_TRUE;

	VkResult result = vkCreateSwapchainKHR(logicalDevices[0], &swapchainCreateInfo, &swapchainCreationCallbacks, &swapchain);

	if (result == VK_SUCCESS) {
		std::cout << "Swapchain created successfully." << std::endl;
	}
	else
		throw VulkanException("Swapchain creation failed.");

	VkResult swapchainImageResult;

	if ((swapchainImageResult = vkGetSwapchainImagesKHR(logicalDevices[0], swapchain, &swapchainImagesCount, nullptr)) != VK_SUCCESS)
		throw VulkanException("Couldn't get swapchain images.");

	swapchainImages = new VkImage[swapchainImagesCount];

	if ((swapchainImageResult = vkGetSwapchainImagesKHR(logicalDevices[0], swapchain, &swapchainImagesCount, swapchainImages)) == VK_SUCCESS) {
		std::cout << "Swapchain images obtained successfully." << std::endl;
	}
	else
		throw VulkanException("Couldn't get swapchain images.");
}

void VulkanEngine::waitToDraw() {
	VkSemaphore semaphore = {};
	uint32_t imageIndex = -1;

	VkFenceCreateInfo fenceCreateInfo = {};

	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.pNext = nullptr;
	fenceCreateInfo.flags = 0;

	VkResult createFenceResult = vkCreateFence(logicalDevices[0], &fenceCreateInfo, nullptr, &acquireNextImageIndexFence);

	vkResetFences(logicalDevices[0], 1, &acquireNextImageIndexFence);

	VkResult acquireNextImageIndexResult = vkAcquireNextImageKHR(logicalDevices[0], swapchain, 0, VK_NULL_HANDLE, acquireNextImageIndexFence, &imageIndex);

	if (acquireNextImageIndexResult == VK_NOT_READY) {
		vkWaitForFences(logicalDevices[0], 1, &acquireNextImageIndexFence, VK_TRUE, 1000000000000000);
	}

	std::cout << "Next available swapchain image index acquired." << std::endl;
	present(imageIndex);
	return;
}

void VulkanEngine::present(uint32_t swapchainPresentImageIndex) {
	VkCommandPool commandPool = {};

	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = nullptr;
	presentInfo.pSwapchains = &swapchain;
	presentInfo.swapchainCount = 1;
	presentInfo.pImageIndices = &swapchainPresentImageIndex;
	presentInfo.pResults = nullptr;
	presentInfo.waitSemaphoreCount = 0;
	presentInfo.pWaitSemaphores = nullptr;

	VkCommandPoolCreateInfo commandPoolCreateInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO, nullptr, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT, graphicQueueFamilyIndex };

	vkCreateCommandPool(logicalDevices[0], &commandPoolCreateInfo, nullptr, &commandPool);

	VkCommandBufferAllocateInfo commandBufferAllocateInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, nullptr, commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY , 1 };
	VkCommandBuffer commandBuffer = {};

	vkAllocateCommandBuffers(logicalDevices[0], &commandBufferAllocateInfo, &commandBuffer);

	VkCommandBufferBeginInfo commandBufferBeginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, nullptr, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT , nullptr };

	vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);

	VkImageMemoryBarrier imageBarrier = {
		VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		nullptr,
		VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		VK_ACCESS_MEMORY_READ_BIT,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		VK_QUEUE_FAMILY_IGNORED,
		VK_QUEUE_FAMILY_IGNORED,
		swapchainImages[swapchainPresentImageIndex],
		{
			VK_IMAGE_ASPECT_COLOR_BIT,
			0,
			1,
			0,
			1
		}
	};

	vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_DEPENDENCY_BY_REGION_BIT, 0, nullptr, 0, nullptr, 1, &imageBarrier);

	VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO, nullptr, 0, nullptr, nullptr, 1, &commandBuffer, 0 , nullptr };

	vkEndCommandBuffer(commandBuffer);

	vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);

	vkDeviceWaitIdle(logicalDevices[0]);

	vkDestroyCommandPool(logicalDevices[0], commandPool, nullptr);

	VkResult result = vkQueuePresentKHR(queue, &presentInfo);

	if (result == VK_SUCCESS) {
		std::cout << "Image presentation command successfully submitted to queue." << std::endl;
	}
	else if (result == VK_SUBOPTIMAL_KHR) {
		std::cout << "Image presentation command successfully submitted to queue, but the swapchain is not longer optimal for the target surface." << std::endl;
	}
	else {
		throw VulkanException("Failed to present.");
	}
}

void VulkanEngine::createShaderModule() {

	HRSRC handleShaderResource = FindResource(NULL, MAKEINTRESOURCE(IDR_SPIRV1), "SPIRV");
	HGLOBAL handleLoadedShaderResource = LoadResource(NULL, handleShaderResource);

	void * pShaderData = LockResource(handleLoadedShaderResource);
	DWORD shaderSize = SizeofResource(NULL, handleShaderResource);

	shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderModuleCreateInfo.pNext = nullptr;
	shaderModuleCreateInfo.codeSize = shaderSize;
	shaderModuleCreateInfo.flags = 0;
	shaderModuleCreateInfo.pCode = (uint32_t*)pShaderData;

	if (vkCreateShaderModule(logicalDevices[0], &shaderModuleCreateInfo, nullptr, &shaderModule) == VK_SUCCESS) {
		std::cout << "Shader Module created successfully." << std::endl;
	}
	else
		throw VulkanException("Couldn't create shader module.");
}

void VulkanEngine::createPipelineLayout() {
	VkDescriptorSetLayoutBinding *bindings = new VkDescriptorSetLayoutBinding[2];

	bindings[0].binding = 0;
	bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	bindings[0].descriptorCount = 1;
	bindings[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
	bindings[0].pImmutableSamplers = nullptr;

	bindings[1].binding = 1;
	bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	bindings[1].descriptorCount = 1;
	bindings[1].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
	bindings[1].pImmutableSamplers = nullptr;


	descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutCreateInfo.pNext = nullptr;
	descriptorSetLayoutCreateInfo.bindingCount = 2;
	descriptorSetLayoutCreateInfo.flags = 0;
	descriptorSetLayoutCreateInfo.pBindings = bindings;

	if (vkCreateDescriptorSetLayout(logicalDevices[0], &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout) == VK_SUCCESS) {
		std::cout << "Descriptor Set Layout created successfully." << std::endl;
	}
	else
		throw VulkanException("Couldn't create descriptor set layout.");

	VkPushConstantRange pushConstantRanage = {};
	pushConstantRanage.offset = 0;
	pushConstantRanage.size = 4;
	pushConstantRanage.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;


	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.pNext = nullptr;
	pipelineLayoutCreateInfo.flags = 0;
	pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRanage;
	pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
	pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;
	pipelineLayoutCreateInfo.setLayoutCount = 1;

	VkResult result = vkCreatePipelineLayout(logicalDevices[0], &pipelineLayoutCreateInfo, nullptr, &pipelineLayout);

	if (result == VK_SUCCESS) {
		std::cout << "Pipeline Layout created successfully." << std::endl;
	}
	else
		throw VulkanException("Couldn't create pipeline layout.");
}

void VulkanEngine::createComputePipeline() {
	VkPipelineShaderStageCreateInfo computeShaderStage;

	computeShaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	computeShaderStage.pNext = nullptr;
	computeShaderStage.flags = 0;
	computeShaderStage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	computeShaderStage.module = shaderModule;
	computeShaderStage.pName = u8"main";
	computeShaderStage.pSpecializationInfo = nullptr;

	computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	computePipelineCreateInfo.pNext = nullptr;
	computePipelineCreateInfo.layout = pipelineLayout;
	computePipelineCreateInfo.flags = 0;
	computePipelineCreateInfo.stage = computeShaderStage;
	computePipelineCreateInfo.basePipelineIndex = -1;
	computePipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;

	VkResult result = vkCreateComputePipelines(logicalDevices[0], VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &pipeline);

	if (result == VK_SUCCESS) {
		std::cout << "Compute Pipeline created successfully." << std::endl;
	}
	else
		throw VulkanException("Couldn't created compute pipeline.");
}

void VulkanEngine::createDescriptorSetPool() {
	VkDescriptorPoolSize poolSizes[2];
	poolSizes[0].descriptorCount = 1;
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

	poolSizes[1].descriptorCount = 1;
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

	descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolCreateInfo.pNext = nullptr;
	descriptorPoolCreateInfo.flags = 0;
	descriptorPoolCreateInfo.maxSets = 1;
	descriptorPoolCreateInfo.poolSizeCount = 2;
	descriptorPoolCreateInfo.pPoolSizes = poolSizes;

	if (vkCreateDescriptorPool(logicalDevices[0], &descriptorPoolCreateInfo, nullptr, &descriptorPool) == VK_SUCCESS) {
		std::cout << "Descriptor Set Pool created successfully." << std::endl;
	}
	else
		throw VulkanException("Couldn't create descriptor set pool.");
}

void VulkanEngine::allocateDescriptorSet() {
	descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorSetAllocateInfo.pNext = nullptr;
	descriptorSetAllocateInfo.descriptorPool = descriptorPool;
	descriptorSetAllocateInfo.descriptorSetCount = 1;
	descriptorSetAllocateInfo.pSetLayouts = &descriptorSetLayout;

	if (vkAllocateDescriptorSets(logicalDevices[0], &descriptorSetAllocateInfo, &descriptorSet) == VK_SUCCESS) {
		std::cout << "Descriptor Set allocated successfully." << std::endl;
	}
	else
		throw VulkanException("Couldn't allocate descriptor set.");

}

void VulkanEngine::updateDescriptorSet() {
	VkDescriptorBufferInfo *descriptorSetBufferInfos = new VkDescriptorBufferInfo[2];
	descriptorSetBufferInfos[0].buffer = buffers[0];
	descriptorSetBufferInfos[0].offset = 0;
	descriptorSetBufferInfos[0].range = VK_WHOLE_SIZE;

	descriptorSetBufferInfos[1].buffer = buffers[1];
	descriptorSetBufferInfos[1].offset = 0;
	descriptorSetBufferInfos[1].range = VK_WHOLE_SIZE;

	VkWriteDescriptorSet *descriptorSetWrites = new VkWriteDescriptorSet[2];
	descriptorSetWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorSetWrites[0].pNext = nullptr;
	descriptorSetWrites[0].dstSet = descriptorSet;
	descriptorSetWrites[0].dstBinding = 0;
	descriptorSetWrites[0].dstArrayElement = 0;
	descriptorSetWrites[0].descriptorCount = 1;
	descriptorSetWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorSetWrites[0].pImageInfo = nullptr;
	descriptorSetWrites[0].pBufferInfo = descriptorSetBufferInfos;
	descriptorSetWrites[0].pTexelBufferView = nullptr;

	descriptorSetWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorSetWrites[1].pNext = nullptr;
	descriptorSetWrites[1].dstSet = descriptorSet;
	descriptorSetWrites[1].dstBinding = 1;
	descriptorSetWrites[1].dstArrayElement = 0;
	descriptorSetWrites[1].descriptorCount = 1;
	descriptorSetWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	descriptorSetWrites[1].pImageInfo = nullptr;
	descriptorSetWrites[1].pBufferInfo = descriptorSetBufferInfos + 1;
	descriptorSetWrites[1].pTexelBufferView = nullptr;

	vkUpdateDescriptorSets(logicalDevices[0], 2, descriptorSetWrites, 0, nullptr);
	std::cout << "Descriptor Set updated/written to successfully." << std::endl;
}

void VulkanEngine::compute() {
	VkCommandPool commandPool = {};

	VkCommandPoolCreateInfo commandPoolCreateInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO, nullptr, 0, graphicQueueFamilyIndex };
	VkResult result = vkCreateCommandPool(logicalDevices[0], &commandPoolCreateInfo, nullptr, &commandPool);

	if (result == VK_SUCCESS) {
		std::cout << "Command Pool created successfully." << std::endl;
	}
	else
		throw VulkanException("Couldn't create command pool.");

	VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.pNext = nullptr;
	commandBufferAllocateInfo.commandBufferCount = 1;
	commandBufferAllocateInfo.commandPool = commandPool;
	commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

	VkCommandBuffer commandBuffer = {};

	result = vkAllocateCommandBuffers(logicalDevices[0], &commandBufferAllocateInfo, &commandBuffer);

	if (result == VK_SUCCESS)
	{
		std::cout << "Command Buffer allocated successfully." << std::endl;
	}
	else
		throw VulkanException("Couldn't allocate command buffer.");

	VkCommandBufferBeginInfo commandBufferBeginInfo = {};
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBeginInfo.pInheritanceInfo = nullptr;
	commandBufferBeginInfo.flags = 0;

	result = vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);

	if (result == VK_SUCCESS)
	{
		std::cout << "Command Buffer began successfully." << std::endl;
	}
	else
		throw VulkanException("Couldn't begin command buffer.");

	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);


	VkBufferMemoryBarrier bufferMemoryBeforeBarrier = {};
	VkBufferMemoryBarrier bufferMemoryAfterBarrier = {};

	bufferMemoryBeforeBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
	bufferMemoryBeforeBarrier.pNext = nullptr;
	bufferMemoryBeforeBarrier.buffer = buffers[0];
	bufferMemoryBeforeBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
	bufferMemoryBeforeBarrier.dstAccessMask = VK_ACCESS_UNIFORM_READ_BIT;
	bufferMemoryBeforeBarrier.offset = 0;
	bufferMemoryBeforeBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	bufferMemoryBeforeBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	bufferMemoryBeforeBarrier.size = VK_WHOLE_SIZE;

	bufferMemoryAfterBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
	bufferMemoryAfterBarrier.pNext = nullptr;
	bufferMemoryAfterBarrier.buffer = buffers[1];
	bufferMemoryAfterBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
	bufferMemoryAfterBarrier.dstAccessMask = VK_ACCESS_HOST_WRITE_BIT;
	bufferMemoryAfterBarrier.offset = 0;
	bufferMemoryAfterBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	bufferMemoryAfterBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	bufferMemoryAfterBarrier.size = VK_WHOLE_SIZE;


	vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0, 0, nullptr, 1, &bufferMemoryBeforeBarrier, 0, nullptr);
	vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_HOST_BIT, 0, 0, nullptr, 1, &bufferMemoryAfterBarrier, 0, nullptr);

	unsigned int pushConstant = 3;

	vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, 4, &pushConstant);


	vkCmdDispatch(commandBuffer, 1, 1, 1);

	vkEndCommandBuffer(commandBuffer);

	std::cout << "Command Buffer ended successfully." << std::endl;

	VkSubmitInfo queueSubmit = {};
	queueSubmit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	queueSubmit.pNext = nullptr;
	queueSubmit.waitSemaphoreCount = 0;
	queueSubmit.pWaitSemaphores = nullptr;
	queueSubmit.pWaitDstStageMask = nullptr;
	queueSubmit.commandBufferCount = 1;
	queueSubmit.pCommandBuffers = &commandBuffer;
	queueSubmit.signalSemaphoreCount = 0;
	queueSubmit.pSignalSemaphores = nullptr;

	VkFence queueComputeDoneFence = {};

	VkFenceCreateInfo fenceCreateInfo = {};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.pNext = nullptr;
	fenceCreateInfo.flags = 0;

	result = vkCreateFence(logicalDevices[0], &fenceCreateInfo, nullptr, &queueComputeDoneFence);

	if (result != VK_SUCCESS)
		throw VulkanException("Couldn't create fence.");

	if (vkQueueSubmit(queue, 1, &queueSubmit, queueComputeDoneFence) == VK_SUCCESS) {
		std::cout << "Command Buffer successfully submitted to queue.";
	}
	else
		throw VulkanException("Couldn't submit command buffer.");

	vkWaitForFences(logicalDevices[0], 1, &queueComputeDoneFence, VK_TRUE, 1000 * 1000000000);

	vkResetFences(logicalDevices[0], 1, &queueComputeDoneFence);

	VkDeviceSize mapSize;
	void **pResultMappedMemory = new void*;


	vkMapMemory(logicalDevices[0], memories[1], 0, VK_WHOLE_SIZE, 0, pResultMappedMemory);

	uint32_t counter = 0;

	std::string readableOutputData = "";
	
	uint16_t padding = 1;

	for (int i = 0; i < memoryAllocationSize + padding; i++)
	{
		if (((unsigned char*)(*pResultMappedMemory))[i] == 6)
			counter++;
		char tmp[3];

		unsigned char byte = *(((unsigned char*)(*pResultMappedMemory)) + i);
		sprintf_s(tmp, "%02X", byte);
		readableOutputData += tmp;
	}

	vkUnmapMemory(logicalDevices[0], memories[1]);
}