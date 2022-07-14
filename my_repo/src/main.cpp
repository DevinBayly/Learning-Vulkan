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

vector<const char *> extensionsRequested = {VK_KHR_SURFACE_EXTENSION_NAME};
static bool debug = false;

int main(int argc, char **argv, char **envp)
{
    // for (char **env = envp; *env != 0; env++)
    // {
    //     char *thisEnv = *env;
    //     printf("%s\n", thisEnv);
    // }
    uint32_t layerCount;
    std::vector<VkLayerProperties> layerProps;
    VkResult result;
    result = vkEnumerateInstanceLayerProperties(&layerCount, NULL);
    std::cout << "result " << result << " layer count " << layerCount << std::endl;
    layerProps.resize(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, layerProps.data());
    vector<LayerProperties> propList;
    // now iterating over the layers
    for (auto globalProp : layerProps)
    {
        std::cout << globalProp.description << std::endl;
        LayerProperties prop;
        prop.properties = globalProp;
        // getting extensions all set
        uint32_t extensionCount;
        VkResult extRes;
        char *layerName = globalProp.layerName;
        vkEnumerateInstanceExtensionProperties(layerName, &extensionCount, NULL);
        cout << "number of extension layers on instance " << extensionCount << endl;
        prop.extensions.resize(extensionCount);
        vkEnumerateInstanceExtensionProperties(layerName, &extensionCount, prop.extensions.data());
        // now we have extensions for the layer that we can print out info for
        for (auto extInstance : prop.extensions)
        {

            cout << "extension name is " << extInstance.extensionName << endl;
        }
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = NULL;
    appInfo.pApplicationName = "first vulkan from book";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 3, 216);

    VkInstanceCreateInfo instInfo{};
    instInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instInfo.pNext = VK_NULL_HANDLE;
    instInfo.flags = 0;
    instInfo.pApplicationInfo = &appInfo;

    // specifying layers app is requesting
    instInfo.enabledLayerCount = (uint32_t)layersRequested.size();
    instInfo.ppEnabledLayerNames = layersRequested.data();

    // specifying extensions requested
    instInfo.enabledExtensionCount = extensionsRequested.size();
    instInfo.ppEnabledExtensionNames = extensionsRequested.data();
    VkInstance instance;
    VkResult instanceCreateResult = vkCreateInstance(&instInfo, NULL, &instance);
    cout << " result is " << instanceCreateResult << endl;
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
        if (name.find("llvm")  == string::npos) {
            selectedGpu = gpuDev;
        };
    }
    cout << "number of devs " << numDevices << endl;
    // extensions on device might exist per requested layer, like the monitor or api dump layer

    // iterate over teh layers and then run the physical device query

    for (auto globalLayer : layerProps)
    {
        uint32_t num_extensions;
        vector<VkExtensionProperties> exprops{};
        vkEnumerateDeviceExtensionProperties(selectedGpu,globalLayer.layerName,&num_extensions,NULL);
        exprops.resize(num_extensions);
        vkEnumerateDeviceExtensionProperties(selectedGpu,globalLayer.layerName,&num_extensions,exprops.data());
        for (auto exprop: exprops) {

        cout << "extension prop name is  " << exprop.extensionName << endl;
        }

        // now we will probably see the properties fly by in the window
    }
    // looking at the memory properties now
    VkPhysicalDeviceMemoryProperties memprops{};
    vkGetPhysicalDeviceMemoryProperties(selectedGpu,&memprops);
    cout << "Done" << endl;
}
