#include "Engine.hpp"
#include "Instance.hpp"
#include "Logging.hpp"
#include "Device.hpp"
#include "Swapchain.hpp"
#include "GraphicsPipeline.hpp"


Engine::Engine() {

	buildGlfwWindow();

	makeInstance();

	makeDebugMessenger();

	makeDevice();

	makePipeline();
}


Engine::~Engine() {

	device.destroyPipeline(graphics_pipeline);
	device.destroyPipelineLayout(graphics_pipeline_layout);
	device.destroyRenderPass(graphics_pipeline_render_pass);

	for (vkUtil::SwapChainFrame frame : swapchain_frames) {

		device.destroyImageView(frame.image_view);

	}

	device.destroySwapchainKHR(swapchain);

	device.destroy();

	instance.destroySurfaceKHR(surface);

	instance.destroyDebugUtilsMessengerEXT(debug_messenger, nullptr, dispatch_loader);

	instance.destroy();

	glfwTerminate();

}

void Engine::buildGlfwWindow() {

	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	if (!(window = glfwCreateWindow(width, height, "Cyan Crate: A Vulkan Engine", nullptr, nullptr))) {

		std::cerr << "GLFW window failed to create!" << '\n';

	}


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
	swapchain_frames = bundle.frames;
	swapchain_format = bundle.format;
	swapchain_extent = bundle.extent;

}

void Engine::makePipeline() {

	vkInit::GraphicsPipelineInBundle specification = {};

	specification.logical_device = device;
	specification.vertex_file_path = "Shaders/vertex.spv";
	specification.fragment_file_path = "Shaders/fragment.spv";
	specification.swapchain_image_format = swapchain_format;
	specification.swapchain_extent = swapchain_extent;

	vkInit::GraphicsPipelineOutBundle output = vkInit::makeGraphicsPipeline(debug_mode, specification);

	graphics_pipeline_layout = output.layout;
	graphics_pipeline_render_pass = output.render_pass;
	graphics_pipeline = output.pipeline;

}
