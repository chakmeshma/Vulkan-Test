//
// Created by chakmeshma on 20.11.2017.
//

#ifndef VULKAN_TEST_VULKAN_ENGINE_H
#define VULKAN_TEST_VULKAN_ENGINE_H


#pragma once

#define VK_USE_PLATFORM_WIN32_KHR

#ifndef NDEBUG
#define VKASSERT_SUCCESS(i) assert(i == VK_SUCCESS)
#define ASSERT(i) assert(i)
#else
#define VKASSERT_SUCCESS(i) i
#define ASSERT(i) i
#endif


#include <vulkan\vulkan.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include "IL\il.h"
#include <assert.h>
#include "math.h"
#include "Vulkan Engine Exception.h"
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>


typedef struct {
    float position[3];
    float normal[3];
    float uv[2];
    float tangent[3];
    float bitangent[3];
} attribute;

//const uint16_t MAX_DEFAULT_ARRAY_SIZE = 10;
const uint16_t MAX_MESHES = 30;

//const uint16_t MAX_BUFFER_ARRAY_SIZE = MAX_DEFAULT_ARRAY_SIZE;
//const uint16_t MAX_IMAGE_ARRAY_SIZE = MAX_DEFAULT_ARRAY_SIZE;
//const uint16_t MAX_IMAGE_VIEW_ARRAY_SIZE = MAX_DEFAULT_ARRAY_SIZE;
//const uint16_t MAX_DEVICE_MEMORY_ALLOCATION_ARRAY_SIZE = MAX_DEFAULT_ARRAY_SIZE;
//const uint16_t MAX_SPARSE_IMAGE_ARRAY_SIZE = MAX_DEFAULT_ARRAY_SIZE;
//const uint16_t MAX_SPARSE_IMAGE_MEMORY_REQUIREMENTS_ARRAY_SIZE = MAX_DEFAULT_ARRAY_SIZE;
//const uint16_t MAX_SPARSE_IMAGE_FORMAT_PROPERTIES_ARRAY_SIZE = MAX_DEFAULT_ARRAY_SIZE;
const uint16_t MAX_COLOR_TEXTURE_ARRAY_SIZE = MAX_MESHES;
const uint16_t MAX_NORMAL_TEXTURE_ARRAY_SIZE = MAX_COLOR_TEXTURE_ARRAY_SIZE;
const uint16_t MAX_SPECULAR_TEXTURE_ARRAY_SIZE = MAX_COLOR_TEXTURE_ARRAY_SIZE;
const uint16_t MAX_UNIFORM_BUFFER_ARRAY_SIZE = MAX_MESHES;
const uint16_t MAX_VERTEX_BUFFER_ARRAY_SIZE = MAX_UNIFORM_BUFFER_ARRAY_SIZE;
const uint16_t MAX_INDEX_BUFFER_ARRAY_SIZE = MAX_UNIFORM_BUFFER_ARRAY_SIZE;


class VulkanEngine {
public:
    VulkanEngine(HINSTANCE hInstance, HWND windowHandle, VulkanEngine **ppUnstableInstance);

    ~VulkanEngine();

    void getInstanceExtensions();

    void getDeviceExtensions();

    static std::string getVersionString(uint32_t versionBitmask);

    void draw();

    bool terminating = false;

    struct ModelMatrix {
        float modelMatrix[16];
    };

    struct ViewProjectionMatrices {
        float viewMatrix[16];
        float projectionMatrix[16];
    };
private:
    const char *texturesDirectoryPath = ".\\Resources\\";
    //const char* meshesDirectoryPath = "C:\\Users\\chakm\\Desktop\\Bee Shader Resources\\Objects\\";

    uint32_t instanceExtensionsCount = 0;
    VkInstance instance;
    VkDevice logicalDevices[1];
    VkDeviceQueueCreateInfo deviceQueueCreateInfos[2];
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
    uint32_t graphicsQueueFamilyIndex = -1;
    uint32_t transferQueueFamilyIndex = -1;
    uint32_t graphicsQueueFamilyNumQueue = 0;
    uint32_t transferQueueFamilyNumQueue = 0;
    std::vector<VkQueueFamilyProperties> queueFamilyProperties;
    VkDeviceCreateInfo logicalDeviceCreateInfo;
    uint32_t layerPropertiesCount = -1;
    std::vector<VkLayerProperties> layerProperties;
    uint32_t deviceExtensionsCount = 0;
    std::vector<VkExtensionProperties> deviceExtensions;
    std::vector<VkExtensionProperties> instanceExtensions;
    VkImageFormatProperties imageFormatProperties = {};
    VkFormatProperties formatProperties = {};
    VkImageCreateInfo imageCreateInfo = {};
    VkImage depthImage;
    VkImageViewCreateInfo imageViewCreateInfo = {};
    VkImageView depthImageView;
    uint32_t memoryTypeCount = 0;
    uint32_t hostVisibleMemoryTypeIndex = -1;
    uint32_t deviceLocalMemoryTypeIndex = -1;
    VkDeviceSize memoryCommittedBytesCount = 0;
    VkMappedMemoryRange memoryFlushRange = {};
    VkMemoryRequirements imageMemoryRequirements = {};
    VkImage *sparseImages;
    VkSparseImageMemoryRequirements *sparseImageMemoryRequirements;
    uint32_t sparseMemoryRequirementsCount = 0;
    VkImageCreateInfo sparseImageCreateInfo = {};
    VkSparseImageFormatProperties *physicalDeviceSparseImageFormatProperties;
    uint32_t physicalDeviceSparseImageFormatPropertiesCount = 0;
    VkQueue graphicsQueue;
    VkQueue transferQueue;
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
    VkPresentModeKHR surfacePresentMode;
    uint32_t swapchainImagesCount = 0;
    VkImage *swapchainImages;
    VkImageView *swapchainImageViews;
    VkPresentInfoKHR presentInfo = {};
    VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
    VkShaderModule computeShaderModule = {};
    VkComputePipelineCreateInfo computePipelineCreateInfo = {};
    VkPipeline computePipeline = {};
    VkPipeline graphicsPipeline = {};
    VkDescriptorSetLayoutCreateInfo computeDescriptorSetLayoutCreateInfo = {};
    VkDescriptorSetLayoutCreateInfo graphicsDescriptorSetLayoutCreateInfo = {};
    VkDescriptorSetLayout computeDescriptorSetLayout = {};
    VkDescriptorSetLayout graphicsDescriptorSetLayout = {};
    VkPipelineLayoutCreateInfo computePipelineLayoutCreateInfo = {};
    VkPipelineLayoutCreateInfo graphicsPipelineLayoutCreateInfo = {};
    VkPipelineLayout computePipelineLayout = {};
    VkPipelineLayout graphicsPipelineLayout = {};
    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
    VkDescriptorPool descriptorPool = {};
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
    VkRenderPassCreateInfo renderPassCreateInfo = {};
    VkRenderPass renderPass = {};
    VkFramebuffer *framebuffers;
    VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
    VkShaderModule graphicsVertexShaderModule;
    VkShaderModule graphicsGeometryShaderModule;
    VkShaderModule graphicsFragmentShaderModule;
    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {};
    VkVertexInputBindingDescription vertexBindingDescription = {};
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = {};
    VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
    VkViewport viewport = {};
    VkRect2D scissor = {};
    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {};
    VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = {};
    VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = {};
    VkSemaphore waitToPresentSemaphore;
    VkSemaphore indexAcquiredSemaphore;
    VkCommandPool renderCommandPool;
    VkCommandPool transferCommandPool;
    VkCommandBuffer renderCommandBuffer;
    VkFormat surfaceImageFormat;
    VkFormat depthFormat;
    std::vector<attribute> sortedAttributes[MAX_VERTEX_BUFFER_ARRAY_SIZE];
    std::vector<uint32_t> sortedIndices[MAX_INDEX_BUFFER_ARRAY_SIZE];
    uint32_t totalUniformBufferSize = sizeof(ModelMatrix);
    uint32_t indexBuffersSizes[MAX_INDEX_BUFFER_ARRAY_SIZE];
    uint32_t vertexBuffersSizes[MAX_VERTEX_BUFFER_ARRAY_SIZE];
    VkImage colorTextureImagesDevice[MAX_COLOR_TEXTURE_ARRAY_SIZE];
    VkImage colorTextureImages[MAX_COLOR_TEXTURE_ARRAY_SIZE];
    VkImageView colorTextureViews[MAX_COLOR_TEXTURE_ARRAY_SIZE];
    VkImage normalTextureImagesDevice[MAX_NORMAL_TEXTURE_ARRAY_SIZE];
    VkImage normalTextureImages[MAX_NORMAL_TEXTURE_ARRAY_SIZE];
    VkImageView normalTextureViews[MAX_NORMAL_TEXTURE_ARRAY_SIZE];
    VkImage specTextureImagesDevice[MAX_SPECULAR_TEXTURE_ARRAY_SIZE];
    VkImage specTextureImages[MAX_SPECULAR_TEXTURE_ARRAY_SIZE];
    VkImageView specTextureViews[MAX_SPECULAR_TEXTURE_ARRAY_SIZE];
    VkDeviceMemory depthImageMemory;
    VkDeviceMemory uniTexturesMemory;
    VkDeviceMemory uniBuffersMemory;
    VkDeviceMemory uniTexturesMemoryDevice;
    VkDeviceMemory uniBuffersMemoryDevice;
    VkBuffer uniformBuffersDevice[MAX_UNIFORM_BUFFER_ARRAY_SIZE];
    VkBuffer vertexBuffersDevice[MAX_VERTEX_BUFFER_ARRAY_SIZE];
    VkBuffer indexBuffersDevice[MAX_INDEX_BUFFER_ARRAY_SIZE];
    VkBuffer uniformBuffers[MAX_UNIFORM_BUFFER_ARRAY_SIZE];
    VkBuffer vertexBuffers[MAX_VERTEX_BUFFER_ARRAY_SIZE];
    VkBuffer indexBuffers[MAX_INDEX_BUFFER_ARRAY_SIZE];
    VkDeviceSize *colorTexturesBindOffsetsDevice;
    VkDeviceSize *normalTexturesBindOffsetsDevice;
    VkDeviceSize *specTexturesBindOffsetsDevice;
    VkDeviceSize *colorTexturesBindOffsets;
    VkDeviceSize *normalTexturesBindOffsets;
    VkDeviceSize *specTexturesBindOffsets;
    VkDeviceSize *uniformBuffersBindOffsetsDevice;
    VkDeviceSize *vertexBuffersBindOffsetsDevice;
    VkDeviceSize *indexBuffersBindOffsetsDevice;
    VkDeviceSize *uniformBuffersBindOffsets;
    VkDeviceSize *vertexBuffersBindOffsets;
    VkDeviceSize *indexBuffersBindOffsets;
    ViewProjectionMatrices viewProjection = {};
    aiScene *cachedScene = NULL;
    VkDescriptorSet meshDescriptorSets[MAX_MESHES];
    VkSampler textureSampler;
    bool inited = false;
    VkFence queueDoneFence;
    LARGE_INTEGER frequency;        // ticks per second
    LARGE_INTEGER t1, t2;           // ticks
    double elapsedTime;

    void init();

    void createInstance();

    void getInstanceLayers();

    void getDeviceLayers();

    void enumeratePhysicalDevices();

    void getPhysicalDevicePropertiesAndFeatures();

    void terminate();

    void createLogicalDevice();

//    void getPhysicalDeviceImageFormatProperties(VkFormat imageFormat);

    void getPhysicalDeviceSparseImageFormatProperties();

    void createAllBuffers();

    void createDepthImageAndImageview();

    void createSparseImage();

    //void allocateDeviceMemories();
    //void bindBufferMemories();
    void getQueues();

    void getQueueFamilyPresentationSupport();

    void createSurface();

    void createSwapchain();

    uint32_t acquireNextFramebufferImageIndex();

    void present(uint32_t swapchainPresentImageIndex);


    void createQueueDoneFence();

    void createRenderpass();

    void createSwapchainImageViews();

    void createFramebuffers();

    void createGraphicsShaderModule(const char *shaderFileName, VkShaderModule *shaderModule);

//    void createFragmentGraphicsShaderModule();
//
//    void createGeometryGraphicsShaderModule();

    void createGraphicsPipeline();

    void createPipelineAndDescriptorSetsLayout();

    void render(uint32_t drawableImageIndex);

    void createRenderCommandPool();

    void createTransferCommandPool();

    void createWaitToPresentSemaphore();

    void createWaitToDrawSemaphore();

    void destroySyncMeans();

    void createDescriptorSets();

    void loadMesh();

    /*void writeBuffers();*/
    VkMemoryRequirements createBuffer(VkBuffer *buffer, VkDeviceSize size, VkBufferUsageFlags usageFlags);

    void getSupportedDepthFormat();

    VkMemoryRequirements createTexture(VkImage *textureImage, VkImageUsageFlags usageFlags);

    void createTextureView(VkImageView *textureImageView, VkImage textureImage);

    void createAllTextures();

    void createDescriptorPool();

    void createSyncMeans();

    void setupTimer();

    void commitBuffers();

    void commitTextures();

    void destroyStagingMeans();

    unsigned long loadShaderCode(const char *fileName, char **fileData);

    template<class T>
    static void multiplyMatrix(T *result, T *left, T *right) {
        for (uint8_t i = 0; i < 4; i++) {
            for (uint8_t j = 0; j < 4; j++) {
                result[j * 4 + i] = static_cast<T>(0);

                for (uint8_t x = 0; x < 4; x++) {
                    result[j * 4 + i] += left[x * 4 + i] * right[j * 4 + x];
                }
            }
        }
    }


    template<class T>
    static void calculateProjectionMatrix(T *matrix, T const &fovy, T const &aspect, T const &zNear, T const &zFar) {
        ASSERT(aspect != static_cast<T>(0));
        ASSERT(zFar != zNear);

        T const rad = fovy;
        T const tanHalfFovy = tan(rad / static_cast<T>(2));

        //float Result[4][4];
        matrix[0] = static_cast<T>(1) / (aspect * tanHalfFovy);
        matrix[1] = static_cast<T>(0);
        matrix[2] = static_cast<T>(0);
        matrix[3] = static_cast<T>(0);
        matrix[4] = static_cast<T>(0);
        matrix[5] = static_cast<T>(1) / (tanHalfFovy);
        matrix[6] = static_cast<T>(0);
        matrix[7] = static_cast<T>(0);
        matrix[8] = static_cast<T>(0);
        matrix[9] = static_cast<T>(0);
        matrix[10] = -(zFar + zNear) / (zFar - zNear);
        matrix[11] = -static_cast<T>(1);
        matrix[12] = static_cast<T>(0);
        matrix[13] = static_cast<T>(0);
        matrix[14] = -(static_cast<T>(2) * zFar * zNear) / (zFar - zNear);
        matrix[15] = static_cast<T>(0);

        return;
    }

    void createGraphicsNormalViewerPipeline();

    VkShaderModule graphicsNormalViewerVertexShaderModule;
    VkShaderModule graphicsNormalViewerGeometryShaderModule;
    VkShaderModule graphicsNormalViewerFragmentShaderModule;
    VkPipeline graphicsDebugPipeline;
    float viewZTranslation = -1.8f;
};


#endif //VULKAN_TEST_VULKAN_ENGINE_H