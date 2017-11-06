#include "Vulkan Engine.h"

#pragma comment(lib, "vulkan-1.lib")

VulkanEngine::VulkanEngine(HINSTANCE hInstance, HWND windowHandle) {
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

	createInstance();
	enumeratePhysicalDevices();
	getPhysicalDevicePropertiesAndFeatures();
	createLogicalDevice();
	getDeviceLayers();
	showDeviceExtensions();
	createBuffer();
	getPhysicalDeviceImageFormatProperties();
	getPhysicalDeviceSparseImageFormatProperties();
	createImage();
	//createSparseImage();
	allocateDeviceMemory();
	getImageMemoryRequirements();
	bindImageMemory();
	createImageView();
	getQueue();
	getQueueFamilyPresentationSupport();
	createSurface();
	createSwapchain();
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

	vkGetPhysicalDeviceMemoryProperties(physicalDevices[0], &deviceMemoryProperties);

	memoryTypeCount = deviceMemoryProperties.memoryTypeCount;

	std::cout << "Memory Type count: " << memoryTypeCount << std::endl;

	std::string flagsString;

	for (int i = 0; i < deviceMemoryProperties.memoryTypeCount; i++) {
		flagsString = "";

		int heapIndex = deviceMemoryProperties.memoryTypes[i].heapIndex;

		if ((deviceMemoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) != 0 && (deviceMemoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0)
			deviceLocalExclusiveMemoryTypeIndex = i;

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

	if (deviceLocalExclusiveMemoryTypeIndex == -1)
		throw VulkanException("No desired memory type found.");

	std::cout << "\nTotal Heaps Size:\t" << std::to_string((float)totalHeapMemorySize / (1024 * 1024 * 1024)) << "GB" << std::endl;
}

void VulkanEngine::terminate() {
	//Shutting Down
	//releasing memory
	delete deviceQueueCreateInfo.pQueuePriorities;


	//Terminte other threads
	//Working current thread possible command buffer generation through flags

	std::cout << "Waiting for logical device to return...\n";

	std::unique_lock<std::mutex> lock(mtxLogicalDeviceHandle);
	vkDeviceWaitIdle(logicalDevices[0]);
	lock.unlock();
	lock.release();

	std::unique_lock<std::mutex> lock4(mtxMemoryHandle);
	{
		vkFreeMemory(logicalDevices[0], memories[0], &memoryDeallocationCallbacks);
		std::cout << "Device Memory freed.\n";
	}
	lock4.unlock();
	lock4.release();

	vkDestroyImageView(logicalDevices[0], imageViews[0], &imageViewTerminationCallbacks);
	std::cout << "Image View destroyed.\n";

	//vkDestroyImage(logicalDevices[0], sparseImages[0], &sparseImageTerminationCallbacks);
	//std::cout << "Sparse Image destroyed.\n";

	vkDestroyImage(logicalDevices[0], images[0], &imageTerminationCallbacks);
	std::cout << "Image destroyed.\n";

	vkDestroyBuffer(logicalDevices[0], buffers[0], &bufferTerminationCallbacks);
	std::cout << "Buffer destroyed.\n";

	std::unique_lock<std::mutex> lock3(mtxLogicalDeviceHandle);
	{
		vkDestroyDevice(logicalDevices[0], &deviceTerminationCallbacks);

		std::cout << "Logical Device destroyed.\n";
	}
	lock3.unlock();
	lock3.release();

	vkDestroyInstance(instance, &instanceTerminationCallbacks);
	std::cout << "Instance destroyed.\n";
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
		throw VulkanException("Logical Device creation failed.");
	}
}

void VulkanEngine::createBuffer() {

	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.pNext = nullptr;
	bufferCreateInfo.flags = 0;
	bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	bufferCreateInfo.size = 1024;

	buffers = new VkBuffer[1];

	if (vkCreateBuffer(logicalDevices[0], &bufferCreateInfo, &bufferCreationCallbacks, buffers) == VK_SUCCESS) {
		std::cout << "Buffer (1KB) object created successfully.\n";
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

void VulkanEngine::allocateDeviceMemory() {
	deviceMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	deviceMemoryAllocateInfo.pNext = nullptr;
	deviceMemoryAllocateInfo.allocationSize = memoryAllocationSize; //1GB
	deviceMemoryAllocateInfo.memoryTypeIndex = deviceLocalExclusiveMemoryTypeIndex;

	VkResult result;

	memories = new VkDeviceMemory[1];

	std::unique_lock<std::mutex> lock5(mtxMemoryHandle);
	{
		result = vkAllocateMemory(logicalDevices[0], &deviceMemoryAllocateInfo, &memoryAllocationCallbacks, memories);
	}
	lock5.unlock();
	lock5.release();

	std::unique_lock<std::mutex> *pLock6 = nullptr;

	std::unique_lock<std::mutex> *pLock7 = nullptr;;

	void **mappedMemory = new void*;

	switch (result) {
	case VK_SUCCESS:
		std::cout << "Device Memory Allocation successful." << std::endl;

		vkGetDeviceMemoryCommitment(logicalDevices[0], memories[0], &memoryCommittedBytesCount);

		std::cout << "Device Memory Allocation bytes commited:\t" << ((float)memoryCommittedBytesCount) / (1024 * 1024) << "MB" << std::endl;


		pLock6 = new std::unique_lock<std::mutex>(mtxMemoryHandle);
		{
			result = vkMapMemory(logicalDevices[0], memories[0], 0, VK_WHOLE_SIZE, 0, mappedMemory);
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

		std::cout << "Writing to visible mapped memory... (1GB)" << std::endl;
		memset(*mappedMemory, 7, memoryAllocationSize);
		std::cout << "Writing finished." << std::endl;

		memoryFlushRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		memoryFlushRange.pNext = nullptr;
		memoryFlushRange.size = VK_WHOLE_SIZE;
		memoryFlushRange.offset = 0;
		memoryFlushRange.memory = memories[0];

		std::cout << "Flushing mapped memory... (1GB)" << std::endl;
		result = vkFlushMappedMemoryRanges(logicalDevices[0], 1, &memoryFlushRange);

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

		/*std::cout << "Invalidating mapped memory... (1GB)" << std::endl;
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

	bool compatibleAllocatedMemory = (imageMemoryRequirements.memoryTypeBits & (1 << deviceLocalExclusiveMemoryTypeIndex)) != 0;

	if (!compatibleAllocatedMemory)
		throw VulkanException("Image doesn't support mapped memory type.");
	//else
	/*std::cout << "Image supports Memory Mapping for the selected memory type (index "<< deviceLocalExclusiveMemoryTypeIndex <<").\n";*/
}

void VulkanEngine::bindImageMemory() {
	VkResult result;

	std::unique_lock<std::mutex> lock8(mtxImageHandle);
	{
		result = vkBindImageMemory(logicalDevices[0], images[0], memories[0], 0);
	}
	lock8.unlock();
	lock8.release();

	switch (result) {
	case VK_SUCCESS:
		std::cout << "Memory bound to image successfully.\n";
		break;
	default:
		std::cout << "Memory binding to image failed.";

	}
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
		std::cout << "Successfully fetched device surface capabilities" << std::endl;

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
		std::cout << "Swapchain images acquired successfully." << std::endl;
	}
	else
		throw VulkanException("Couldn't get swapchain images.");
}