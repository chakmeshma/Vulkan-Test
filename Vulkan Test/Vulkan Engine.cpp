#include "Vulkan Engine.h"

#pragma comment(lib, "vulkan-1.lib")

VulkanEngine::VulkanEngine() {
	init();
}

VulkanEngine::~VulkanEngine() {
	terminate();
}

void VulkanEngine::showInstanceExtensions() {
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
	showInstanceExtensions();

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

	createInstance();
	enumeratePhysicalDevices();
	getPhysicalDevicePropertiesAndFeatures();
	createLogicalDevice();
	getLayers();
	showDeviceExtensions();
	createBuffer();
	getPhysicalDeviceImageFormatProperties();
	createImage();
	createImageView();
	createSparseImage();
	allocateMapDeviceMemory();
	getImageMemoryRequirements();
	bindImageMemory();
}

void VulkanEngine::createInstance() {
	appInfo.apiVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.applicationVersion = 1;
	appInfo.pApplicationName = "Vulkan Engine";


	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pApplicationInfo = &appInfo;

	if (vkCreateInstance(&instanceCreateInfo, &instanceCrationCallbacks, &instance) == VK_SUCCESS) {
		std::cout << "Instance created successfully.\n";
	}
	else {
		throw VulkanException("Instance creation failed.");
	}
}

void VulkanEngine::getLayers() {
	vkEnumerateDeviceLayerProperties(physicalDevices[0], &layerPropertiesCount, nullptr);

	std::cout << "Number of Device Layers available to physical device:\t" << std::to_string(layerPropertiesCount) << std::endl;

	layerProperties.resize(layerPropertiesCount);

	vkEnumerateDeviceLayerProperties(physicalDevices[0], &layerPropertiesCount, layerProperties.data());

	std::cout << "Device Layers:\n";

	for (int i = 0; i < layerPropertiesCount; i++) {
		std::cout << "\tLayer #" << std::to_string(i) << std::endl;
		std::cout << "\t\tName:\t" << layerProperties[i].layerName << std::endl;
		std::cout << "\t\tSpecification Version:\t" << getVersionString(layerProperties[i].specVersion) << std::endl;
		std::cout << "\t\tDescription:\t" << layerProperties[i].description << std::endl;
	}

	vkEnumerateInstanceLayerProperties(&layerPropertiesCount, nullptr);

	std::cout << "Number of Instance Layers available to physical device:\t" << std::to_string(layerPropertiesCount) << std::endl;

	layerProperties.resize(layerPropertiesCount);

	vkEnumerateInstanceLayerProperties(&layerPropertiesCount, layerProperties.data());

	std::cout << "Instance Layers:\n";

	for (int i = 0; i < layerPropertiesCount; i++) {
		std::cout << "\tLayer #" << std::to_string(i) << std::endl;
		std::cout << "\t\tName:\t" << layerProperties[i].layerName << std::endl;
		std::cout << "\t\tSpecification Version:\t" << getVersionString(layerProperties[i].specVersion) << std::endl;
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

	vkDestroyImage(logicalDevices[0], sparseImages[0], &sparseImageTerminationCallbacks);
	std::cout << "Sparse Image destroyed.\n";

	vkDestroyImage(logicalDevices[0], images[0], &imageTerminationCallbacks);
	std::cout << "Image destroyed.\n";

	vkDestroyBuffer(logicalDevices[0], buffers[0], &bufferTerminationCallbacks);
	std::cout << "Buffer destroyed.\n";

	std::unique_lock<std::mutex> lock3(mtxLogicalDeviceHandle);
	{
		vkDestroyDevice(logicalDevices[0], &deviceTerminationCallbacks);

		std::cout << "Logical Device destroyed\n";
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


	logicalDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	logicalDeviceCreateInfo.flags = 0;
	logicalDeviceCreateInfo.pNext = nullptr;
	logicalDeviceCreateInfo.queueCreateInfoCount = 1;
	logicalDeviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
	logicalDeviceCreateInfo.enabledExtensionCount = 0;
	logicalDeviceCreateInfo.enabledLayerCount = 0;
	logicalDeviceCreateInfo.ppEnabledExtensionNames = nullptr;
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

	buffers = new VkBuffer[1];

	if (vkCreateBuffer(logicalDevices[0], &bufferCreateInfo, &bufferCreationCallbacks, buffers) == VK_SUCCESS) {
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
	imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageCreateInfo.queueFamilyIndexCount = 0;
	imageCreateInfo.pQueueFamilyIndices = nullptr;

	VkResult result = vkCreateImage(logicalDevices[0], &imageCreateInfo, &imageCreationCallbacks, images);

	switch (result) {
	case VK_SUCCESS:
		std::cout << "Image created successfully.\n";
		break;
	default:
		throw VulkanException("Image creation failed.");
	}
}

void VulkanEngine::getPhysicalDeviceImageFormatProperties() {
	vkGetPhysicalDeviceFormatProperties(physicalDevices[0], VK_FORMAT_R8G8B8A8_UNORM, &formatProperties);

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
	imageViewCreateInfo.subresourceRange.layerCount = 0;

	VkResult result = vkCreateImageView(logicalDevices[0], &imageViewCreateInfo, &imageViewCreationCallbacks, imageViews);

	switch (result) {
	case VK_SUCCESS:
		std::cout << "Image View creation succeeded.\n";
		break;
	default:
		throw VulkanException("Image View creation failed.");
	}
}

void VulkanEngine::allocateMapDeviceMemory() {
	deviceMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	deviceMemoryAllocateInfo.pNext = nullptr;
	deviceMemoryAllocateInfo.allocationSize = memoryAllocationSize; //1GB
	deviceMemoryAllocateInfo.memoryTypeIndex = deviceLocalExclusiveMemoryTypeIndex;

	VkResult result;

	std::unique_lock<std::mutex> lock5(mtxMemoryHandle);
	{
		result = vkAllocateMemory(logicalDevices[0], &deviceMemoryAllocateInfo, &memoryAllocationCallbacks, memories);
	}
	lock5.unlock();
	lock5.release();

	std::unique_lock<std::mutex> *pLock6 = nullptr;

	std::unique_lock<std::mutex> *pLock7 = nullptr;;

	char *mappedMemory;

	switch (result) {
	case VK_SUCCESS:
		std::cout << "Device Memory Allocation successful." << std::endl;

		vkGetDeviceMemoryCommitment(logicalDevices[0], memories[0], &memoryCommittedBytesCount);

		std::cout << "Device Memory Allocation bytes commited:\t" << ((float)memoryCommittedBytesCount) / (1024 * 1024) << "MB" << std::endl;


		pLock6 = new std::unique_lock<std::mutex>(mtxMemoryHandle);
		{
			result = vkMapMemory(logicalDevices[0], memories[0], 0, VK_WHOLE_SIZE, 0, (void**)&mappedMemory);
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
		memset(mappedMemory, 7, memoryAllocationSize);
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
	sparseImageCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	sparseImageCreateInfo.extent.width = 1024;
	sparseImageCreateInfo.extent.height = 1024;
	sparseImageCreateInfo.extent.depth = 1;
	sparseImageCreateInfo.arrayLayers = 1;
	sparseImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	sparseImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	sparseImageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	sparseImageCreateInfo.mipLevels = 1;
	sparseImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	sparseImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	sparseImageCreateInfo.queueFamilyIndexCount = 0;
	sparseImageCreateInfo.pQueueFamilyIndices = nullptr;

	VkResult result = vkCreateImage(logicalDevices[0], &sparseImageCreateInfo, &imageCreationCallbacks, sparseImages);

	switch (result) {
	case VK_SUCCESS:
		std::cout << "Sparse Image created successfully.\n";
		break;
	default:
		throw VulkanException("Sparse Image creation failed.");
	}

	vkGetImageSparseMemoryRequirements(logicalDevices[0], sparseImages[0], &sparseMemoryRequirementsCount, nullptr);
	vkGetImageSparseMemoryRequirements(logicalDevices[0], sparseImages[0], &sparseMemoryRequirementsCount, &sparseImageMemoryRequirements);

	std::cout << "Sparse Image Memory Requirements:\n";

	for (uint32_t i = 0; i < sparseMemoryRequirementsCount; i++) {
		VkSparseImageMemoryRequirements sparseImageMemoryRequirementsElement = (&sparseImageMemoryRequirements)[i];

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
	}
}
