#include "Engine.hpp"
#include "Instance.hpp"
#include "Logging.hpp"
#include "Device.hpp"


Engine::Engine() {

	buildGlfwWindow();

	makeInstance();

	makeDebugMessenger();

	makeDevice();
}

void Engine::buildGlfwWindow() {

	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	if (!(window = glfwCreateWindow(width, height, "Cyan Crate: A Vulkan Engine", nullptr, nullptr))) {

		std::cerr << "GLFW window failed to create!" << '\n';

	}


}

Engine::~Engine() {

	device.destroySwapchainKHR(swapchain);

	device.destroy();

	instance.destroySurfaceKHR(surface);

	instance.destroyDebugUtilsMessengerEXT(debug_messenger, nullptr, dispatch_loader);

	instance.destroy();
	
	glfwTerminate();

}

void Engine::makeInstance() {

	instance = vkInit::makeInstance(debug_mode, "Cyan Crate: A Vulkan Engine");
	dispatch_loader = vk::DispatchLoaderDynamic(instance, vkGetInstanceProcAddr);


	VkSurfaceKHR c_style_surface;
	
	if (glfwCreateWindowSurface(instance, window, nullptr, &c_style_surface) != VK_SUCCESS) {

		if (debug_mode) {

			std::cout << "Failed to abstract the glfw surface for Vulkan!\n";

		}

		return;

	}

	if (debug_mode) {

		std::cout << "Succeded to abstract the glfw surface for Vulkan!\n";

	}

	surface = c_style_surface;

}

void Engine::makeDebugMessenger() {

	debug_messenger = vkInit::makeDebugMessenger(instance, dispatch_loader);

}

void Engine::makeDevice()
{

	physical_device = vkInit::choosePhysicalDevice(debug_mode, instance);
	device = vkInit::createLogicalDevice(debug_mode, physical_device, surface);
	std::array<vk::Queue, 2>queue = vkInit::getQueue(debug_mode, physical_device, device, surface);
	graphics_queue = queue[0];
	present_queue = queue[1];

	// vkInit::querySwapChainSupport(debug_mode, physical_device, surface); 
	vkInit::SwapChainBundle bundle = vkInit::createSwapChain(debug_mode, device, physical_device, surface, width, height);

	swapchain = bundle.swapchain;
	swapchain_images = bundle.images;
	swapchain_format = bundle.format;
	swapchain_extent = bundle.extent;

}
