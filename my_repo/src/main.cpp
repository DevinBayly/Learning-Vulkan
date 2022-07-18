#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>
using namespace std;
struct LayerProperties
{
    VkLayerProperties properties;
    vector<VkExtensionProperties> extensions;
};

vector<const char *> layersRequested = {"VK_LAYER_LUNARG_api_dump", "VK_LAYER_LUNARG_monitor", "VK_LAYER_KHRONOS_validation"};

// vector<const char *> extensionsRequested = {VK_KHR_SURFACE_EXTENSION_NAME};

vector<const char *> instanceExtensionsRequested = {
    "VK_EXT_debug_utils", "VK_KHR_surface", "VK_EXT_validation_features", "VK_EXT_debug_report"};

vector<const char *> extensionsRequested = {
    "VK_KHR_swapchain"};
static bool debug = false;

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
    void *pUserData)
{

    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

int main(int argc, char **argv, char **envp)
{
    // for (char **env = envp; *env != 0; env++)
    // {
    //     char *thisEnv = *env;
    //     printf("%s\n", thisEnv);
    // }
    // debug stuff
    VkDebugUtilsMessengerEXT debugMessenger;
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugCreateInfo.pfnUserCallback = debugCallback;
    debugCreateInfo.pUserData = nullptr;
    uint32_t layerCount;
    std::vector<VkLayerProperties> layerProps;
    VkResult result;
    // this is supposed to give us all the possible layers we could use

    result = vkEnumerateInstanceLayerProperties(&layerCount, NULL);
    uint32_t extensionsCount;
    vkEnumerateInstanceExtensionProperties(NULL, &extensionsCount, NULL);
    cout << "total instance extensions founnd " << extensionsCount << endl;
    vector<VkExtensionProperties> extensionList{};
    extensionList.resize(extensionsCount);
    vkEnumerateInstanceExtensionProperties(NULL, &extensionsCount, extensionList.data());

    std::cout << "result " << result << " layer count " << layerCount << std::endl;
    layerProps.resize(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, layerProps.data());
    vector<LayerProperties> propList;
    // now iterating over the layers
    cout << "LAYERS" << endl;
    for (auto globalProp : layerProps)
    {
        std::cout << globalProp.layerName << ":" << globalProp.description << std::endl;
        // now we have extensions for the layer that we can print out info for
    }
    cout << "EXTENSIONS" << endl;
    for (auto instanceExt : extensionList)
    {
        cout << instanceExt.extensionName << endl;
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = NULL;
    appInfo.pApplicationName = "first vulkan from book";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 3, 216);

    VkInstanceCreateInfo instInfo{};
    instInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instInfo.pNext = &debugCreateInfo;
    instInfo.flags = 0;
    instInfo.pApplicationInfo = &appInfo;

    // specifying layers app is requesting
    instInfo.enabledLayerCount = (uint32_t)layersRequested.size();
    instInfo.ppEnabledLayerNames = layersRequested.data();
    // try bringing in the glfw extensions
    // specifying extensions requested
    instInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensionsRequested.size());
    instInfo.ppEnabledExtensionNames = instanceExtensionsRequested.data();
    VkInstance instance;
    VkResult instanceCreateResult = vkCreateInstance(&instInfo, NULL, &instance);
    if (instanceCreateResult == VK_ERROR_EXTENSION_NOT_PRESENT)
    {
        cout << "extension not found" << endl;
    }
    cout << " result is " << instanceCreateResult << endl;
    // actually set up debugging now
    if (CreateDebugUtilsMessengerEXT(instance, &debugCreateInfo, nullptr, &debugMessenger) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to set up debug messenger!");
    }
    // we are now turning to enumerating on the various devices
    uint32_t numDevices;
    vkEnumeratePhysicalDevices(instance, &numDevices, NULL);
    vector<VkPhysicalDevice> gpuDevices{};
    gpuDevices.resize(numDevices);
    vkEnumeratePhysicalDevices(instance, &numDevices, gpuDevices.data());
    // and then we go on to look at the device extensions
    VkPhysicalDevice selectedGpu;
    for (VkPhysicalDevice gpuDev : gpuDevices)
    {
        // get the properties, and that will go into things like names
        VkPhysicalDeviceProperties props{};
        vkGetPhysicalDeviceProperties(gpuDev, &props);
        cout << "device's name is " << props.deviceName << endl;
        string name = string(props.deviceName);
        // this passes when the text llvm isn't in the name
        if (name.find("llvm") == string::npos)
        {
            selectedGpu = gpuDev;
        };
    }
    cout << "number of devs " << numDevices << endl;
    // extensions on device might exist per requested layer, like the monitor or api dump layer

    // iterate over teh layers and then run the physical device query

    uint32_t num_extensions;
    vector<VkExtensionProperties> exprops{};
    vkEnumerateDeviceExtensionProperties(selectedGpu, NULL, &num_extensions, NULL);
    exprops.resize(num_extensions);
    vkEnumerateDeviceExtensionProperties(selectedGpu, NULL, &num_extensions, exprops.data());
    for (auto exprop : exprops)
    {

        cout << "extension prop name is  " << exprop.extensionName << endl;
    }

    // we must first create a variable holding the queue information for the physical device
    uint32_t number_queues;
    vector<VkQueueFamilyProperties> fam_props{};

    vkGetPhysicalDeviceQueueFamilyProperties(selectedGpu, &number_queues, NULL);

    fam_props.resize(number_queues);
    vkGetPhysicalDeviceQueueFamilyProperties(selectedGpu, &number_queues, fam_props.data());

    // right now there's only one queue here and it's queue flag is 7 so I think we are in luck
    // 7 = 0000111 so if we & against the VK_QUEUE_GRAPHICS_BIT I think it matches with one of those
    uint32_t graphicsQueueIndex;
    if (fam_props[0].queueFlags & VK_QUEUE_GRAPHICS_BIT)
    {
        cout << "yup this queue supports graphics" << endl;
        graphicsQueueIndex = 0;
    }
    // now we have to create a logical device so that we can eventually have a queue to submitdraw  command buffers
    // we make a deviceInfo which refers to a queue info in order to use the vkcreatedevice that gives us the logical device

    VkDeviceQueueCreateInfo queueInfo{};
    queueInfo.queueFamilyIndex = graphicsQueueIndex;
    queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo.pNext = NULL;
    queueInfo.queueCount = 1;
    float priorities[1] = {1.0};
    queueInfo.pQueuePriorities = priorities; // has to do with work prioritization, more complicated

    // making logical dev
    VkDeviceCreateInfo deviceInfo{};
    deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo.pNext = NULL;
    deviceInfo.queueCreateInfoCount = 1;
    deviceInfo.pQueueCreateInfos = &queueInfo;
    deviceInfo.enabledLayerCount = static_cast<uint32_t>(layersRequested.size()); // is this still deprecated?
    deviceInfo.ppEnabledLayerNames = layersRequested.data();
    deviceInfo.enabledExtensionCount = extensionsRequested.size();
    deviceInfo.ppEnabledExtensionNames = extensionsRequested.data();
    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceInfo.pEnabledFeatures = &deviceFeatures;

    VkDevice device;
    result = vkCreateDevice(selectedGpu, &deviceInfo, NULL, &device);
    cout << "logical result is " << result << endl;
    if (result == VK_SUCCESS)
    {
        cout << "great, created the logical device" << endl;
    }
    else
    {
        cout << "nnope no logical device" << endl;
        return -1;
    };

    // working on the command buffer stuff
    VkCommandPoolCreateInfo cpoolInfo{};
    cpoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cpoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    cpoolInfo.queueFamilyIndex = graphicsQueueIndex;

    // use the info to create a command pool
    VkCommandPool cPool;
    result = vkCreateCommandPool(device, &cpoolInfo, NULL, &cPool);
    cout << result << endl;

    // make an allocation info

    VkCommandBufferAllocateInfo cAllocInfo{};
    cAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cAllocInfo.commandPool = cPool;
    cAllocInfo.commandBufferCount = 1;

    VkCommandBuffer cb;
    result = vkAllocateCommandBuffers(device, &cAllocInfo, &cb);
    cout << "command buffer result is " << result << endl;

    // buffers get recorded with commands between "begin" and "end "

    // then they are submittedto queues
    VkQueue q;
    vkGetDeviceQueue(device, 0, 0, &q);

    // working with device memory and host

    VkPhysicalDeviceMemoryProperties deviceMemProps;
    vkGetPhysicalDeviceMemoryProperties(selectedGpu, &deviceMemProps);

    //  making vertex information
    struct ColorVert
    {

        float x, y, z, w;
        float r, g, b, a;
    };

    const vector<ColorVert> triangleData = {
        {0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},
        {1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f},
        {-1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f}};

    // allocating memory for fun
    // create a buffer info
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = sizeof(triangleData[0]) * triangleData.size();
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT; // for instance this means the buffer is the destination
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    // create a buffer

    VkBuffer demoBuffer;
    vkCreateBuffer(device, &bufferInfo, NULL, &demoBuffer);
    //
    // now we can get requirements to use for allocation
    //
    VkMemoryRequirements memReqs;
    vkGetBufferMemoryRequirements(device, demoBuffer, &memReqs);
    VkMemoryAllocateInfo mAllocInfo{};
    mAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    mAllocInfo.allocationSize = memReqs.size;

    mAllocInfo.memoryTypeIndex = 0; // it's also possible to iterate over the device memory props and match the

    VkDeviceMemory devMem;
    vkAllocateMemory(device, &mAllocInfo, NULL, &devMem);

    // in order for host to use memory it must map and eventually un map
    // this will get covered later in the book so I'm skipping now

    // making images!
    // starts with just a declaration of it and no memory to back it up
    VkImage myImg;
    VkImageCreateInfo imgCreateInfo{};
    imgCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imgCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imgCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imgCreateInfo.extent.depth = 1;
    imgCreateInfo.extent.height = 256;
    imgCreateInfo.extent.width = 256;
    imgCreateInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    imgCreateInfo.format = VK_FORMAT_R8G8B8A8_SRGB; // this means the numbers will be 0-256 not 0-1 UNORM
    imgCreateInfo.mipLevels = 1;
    imgCreateInfo.queueFamilyIndexCount = 0;
    imgCreateInfo.arrayLayers = 1;
    imgCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imgCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imgCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imgCreateInfo.pNext = NULL;
    // usage determines whether it is a transfer dest/src or color attachment drawn on by a render pass
    vkCreateImage(device, &imgCreateInfo, nullptr, &myImg);

    // get the memory requirements in a similar way as what was done for the buffer above

    VkMemoryRequirements imgMemReq;
    vkGetImageMemoryRequirements(device, myImg, &imgMemReq);
    // then we have to bind the memory

    VkMemoryAllocateInfo colImgAllocInfo;
    colImgAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    colImgAllocInfo.allocationSize = imgMemReq.size;
    colImgAllocInfo.memoryTypeIndex = 0; // there's only one type of mem on this card
    colImgAllocInfo.pNext = NULL;
    VkDeviceMemory imgMem;
    vkAllocateMemory(device, &colImgAllocInfo, NULL, &imgMem);
    // then we set the image layout
    vkBindImageMemory(device, myImg, imgMem, 0);

    // now we have to make a view in order to use it in the pipeline
    VkImageView imView;
    VkImageViewCreateInfo imviewCreateInfo;

    imviewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imviewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;

    imviewCreateInfo.format = imgCreateInfo.format;
    imviewCreateInfo.subresourceRange = {};
    imviewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imviewCreateInfo.subresourceRange.baseMipLevel = 0;
    imviewCreateInfo.subresourceRange.levelCount = 1;
    imviewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imviewCreateInfo.subresourceRange.layerCount = 1;
    imviewCreateInfo.pNext = NULL;
    imviewCreateInfo.image = myImg;
    imviewCreateInfo.flags = 0;
    imviewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;
    imviewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
    imviewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
    imviewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;
    vkCreateImageView(device, &imviewCreateInfo, NULL, &imView);

    /// now we are doing stuff with rendering!!
    // to create a render pass you need attachments and render subpasses
    // attachment descriptions first
    VkAttachmentDescription attachmentDesc{};

    attachmentDesc.format = imviewCreateInfo.format;
    attachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
    // how contents are treated at the beginning of pass
    attachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; // instead of clear could be LOAD and that keeps content from prev pass

    // how contents are treated at the end of the subpass
    attachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachmentDesc.finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

    // make a reference
    VkAttachmentReference colorRef = {0, VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL};
    // subpasses can help preserve attachments data so that they can be used in multiple passes
    VkSubpassDescription subpassDesc{};
    subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDesc.colorAttachmentCount = 1;
    subpassDesc.pColorAttachments = &colorRef;

    // now we have all we need in order to make the render pass

    VkRenderPassCreateInfo rpInfo{};
    rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    rpInfo.pNext = NULL;
    rpInfo.attachmentCount = 1;
    rpInfo.pAttachments = &attachmentDesc;
    rpInfo.subpassCount = 1;
    rpInfo.pSubpasses = &subpassDesc;
    rpInfo.dependencyCount = 0;
    rpInfo.pDependencies = NULL;

    VkRenderPass rp;
    vkCreateRenderPass(device, &rpInfo, NULL, &rp);

    cout << "Done" << endl;
}
