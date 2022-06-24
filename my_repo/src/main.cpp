#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>
using namespace std;
struct LayerProperties
{
    VkLayerProperties properties;
    vector<VkExtensionProperties> extensions;
};

int main(int argc, char **argv)
{
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
        prop.properties= globalProp;
        // getting extensions all set
        uint32_t extensionCount ;
        VkResult extRes;
        char * layerName = globalProp.layerName;
        vkEnumerateInstanceExtensionProperties(layerName,&extensionCount,NULL);
        cout << "number of extension layers on instance " << extensionCount << endl;
        prop.extensions.resize(extensionCount);
        vkEnumerateInstanceExtensionProperties(layerName,&extensionCount,prop.extensions.data());
        // now we have extensions for the layer that we can print out info for
        for (auto extInstance : prop.extensions) {

         cout << "extension name is " << extInstance.extensionName << endl;
        }


    }
}
