#include "VKBase.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#pragma comment(lib, "glfw3.lib") //���ӱ�������ľ�̬��

//���ڵ�ָ�룬ȫ�ֱ����Զ���ʼ��ΪNULL
GLFWwindow* pWindow;
//��ʾ����Ϣ��ָ��
GLFWmonitor* pMonitor;
//���ڱ���
const char* windowTitle = "EasyVK";

using namespace vulkan;

bool InitializeWindow(VkExtent2D size, bool fullScreen = false, bool isResizable = true, bool limitFrameRate = true) {
    //using�����ռ�
    using namespace vulkan;

    if (!glfwInit()) {
        std::cout << std::format("[ InitializeWindow ] ERROR\nFailed to initialize GLFW!\n");
        return false;
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, isResizable);
    pMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* pMode = glfwGetVideoMode(pMonitor);
    pWindow = fullScreen ?
        glfwCreateWindow(pMode->width, pMode->height, windowTitle, pMonitor, nullptr) :
        glfwCreateWindow(size.width, size.height, windowTitle, nullptr, nullptr);
    if (!pWindow) {
        std::cout << std::format("[ InitializeWindow ]\nFailed to create a glfw window!\n");
        glfwTerminate();
        return false;
    }

    //��������--------------------------------
#ifdef _WIN32
    graphicsBase::Base().AddInstanceExtension(VK_KHR_SURFACE_EXTENSION_NAME);
    graphicsBase::Base().AddInstanceExtension(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#else
    uint32_t extensionCount = 0;
    const char** extensionNames;
    extensionNames = glfwGetRequiredInstanceExtensions(&extensionCount);
    if (!extensionNames) {
        std::cout << std::format("[ InitializeWindow ]\nVulkan is not available on this machine!\n");
        glfwTerminate();
        return false;
    }
    for (size_t i = 0; i < extensionCount; i++)
        graphicsBase::Base().AddInstanceExtension(extensionNames[i]);
#endif
    graphicsBase::Base().AddDeviceExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    //�ڴ���window surfaceǰ����Vulkanʵ��
    graphicsBase::Base().UseLatestApiVersion();
        if (graphicsBase::Base().CreateInstance())
            return false;

    //����window surface
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    if (VkResult result = glfwCreateWindowSurface(graphicsBase::Base().Instance(), pWindow, nullptr, &surface)) {
        std::cout << std::format("[ InitializeWindow ] ERROR\nFailed to create a window surface!\nError code: {}\n", int32_t(result));
        glfwTerminate();
        return false;
    }
    graphicsBase::Base().Surface(surface);

    //ͨ����||��������·ִ����ʡȥ����
    if (//��ȡ�����豸����ʹ���б��еĵ�һ�������豸�����ﲻ�����������⺯��ʧ�ܺ���������豸�����
        graphicsBase::Base().GetPhysicalDevices() ||
        //һ��trueһ��false����ʱ����Ҫ�����õĶ���
        graphicsBase::Base().DeterminePhysicalDevice(0, true, false) ||
        //�����߼��豸
        graphicsBase::Base().CreateDevice())
        return false;
    //----------------------------------------

    //��������--------------------------------
    if (graphicsBase::Base().CreateSwapchain(limitFrameRate))
        return false;
    //----------------------------------------
    return true;
}

void TerminateWindow() 
{
    graphicsBase::Base().WaitIdle();
    /*��Ch1-4���*/
    glfwTerminate();
}

void TitleFps() 
{
    static double time0 = glfwGetTime();
    static double time1;
    static double dt;
    static int dframe = -1;
    static std::stringstream info;
    time1 = glfwGetTime();
    dframe++;
    if ((dt = time1 - time0) >= 1) 
    {
        info.precision(1);
        info << windowTitle << "    " << std::fixed << dframe / dt << " FPS";
        glfwSetWindowTitle(pWindow, info.str().c_str());
        info.str(""); //�������������괰�ڱ����������õ�stringstream
        time0 = time1;
        dframe = 0;
    }
}

void MakeWindowFullScreen() 
{
    const GLFWvidmode* pMode = glfwGetVideoMode(pMonitor);
    glfwSetWindowMonitor(pWindow, pMonitor, 0, 0, pMode->width, pMode->height, pMode->refreshRate);
}

void MakeWindowWindowed(VkOffset2D position, VkExtent2D size) 
{
    const GLFWvidmode* pMode = glfwGetVideoMode(pMonitor);
    glfwSetWindowMonitor(pWindow, nullptr, position.x, position.y, size.width, size.height, pMode->refreshRate);
}
