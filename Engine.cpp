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

	device.destroy();

	instance.destroyDebugUtilsMessengerEXT(debug_messenger, nullptr, dispatch_loader);

	instance.destroy();
	
	glfwTerminate();

}

void Engine::makeInstance() {

	instance = vkInit::makeInstance(debug_mode, "Cyan Crate: A Vulkan Engine");
	dispatch_loader = vk::DispatchLoaderDynamic(instance, vkGetInstanceProcAddr);

}

void Engine::makeDebugMessenger() {

	debug_messenger = vkInit::makeDebugMessenger(instance, dispatch_loader);

}

void Engine::makeDevice()
{

	physical_device = vkInit::choosePhysicalDevice(debug_mode, instance);
	device = vkInit::createLogicalDevice(debug_mode, physical_device);
	graphics_queue = vkInit::getQueue(debug_mode, physical_device, device);

}
