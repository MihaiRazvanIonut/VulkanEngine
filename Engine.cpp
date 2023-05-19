#include "Engine.hpp"
#include "Instance.hpp"
#include "Logging.hpp"
#include "Device.hpp"
#include "Swapchain.hpp"
#include "GraphicsPipeline.hpp"
#include "FrameBuffer.hpp"
#include "Commands.hpp"
#include "Synchronization.hpp"


Engine::Engine(const bool& debug, int width, int height, GLFWwindow* window) {

	this->width = width;
	this->height = height;
	this->window = window;
	this->debug_mode = debug;

	makeInstance();

	makeDebugMessenger();

	makeDevice();

	makePipeline();

	finalizeSetup();
}


Engine::~Engine() {

	device.waitIdle();

	device.destroyFence(in_flight_fence);
	device.destroySemaphore(image_available);
	device.destroySemaphore(render_finished);

	device.destroyCommandPool(command_pool);

	device.destroyPipeline(graphics_pipeline);
	device.destroyPipelineLayout(graphics_pipeline_layout);
	device.destroyRenderPass(graphics_pipeline_render_pass);

	for (auto& frame : swapchain_frames) {

		device.destroyImageView(frame.image_view);
		device.destroyFramebuffer(frame.framebuffer);

	}

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

void Engine::finalizeSetup() {

	vkInit::FrameBufferInput framebuffer_input;
	framebuffer_input.logical_device = device;
	framebuffer_input.render_pass = graphics_pipeline_render_pass;
	framebuffer_input.swapchain_extent = swapchain_extent;
	vkInit::makeFramebuffers(framebuffer_input, swapchain_frames);

	command_pool = vkInit::makeCommandPool(debug_mode, device, physical_device, surface);

	vkInit::CommandBufferInputChunk command_buffer_input_chunk = { device, command_pool, swapchain_frames };
	main_command_buffer = vkInit::makeCommandBuffers(debug_messenger, command_buffer_input_chunk);

	in_flight_fence = vkInit::makeFence(debug_mode, device);
	image_available = vkInit::makeSemaphore(debug_mode, device);
	render_finished = vkInit::makeSemaphore(debug_mode, device);

}

void Engine::recordDrawCommands(vk::CommandBuffer command_buffer, uint32_t image_index) {

	vk::CommandBufferBeginInfo command_buffer_begin_info = {};

	try {

		command_buffer.begin(command_buffer_begin_info);

	}
	catch (vk::SystemError err) {

		if (debug_mode) {

			std::cout << "Failed to begin recording command buffer" << std::endl;

		}

	}

	vk::RenderPassBeginInfo render_pass_begin_info = {};

	render_pass_begin_info.renderPass = graphics_pipeline_render_pass;
	render_pass_begin_info.framebuffer = swapchain_frames[image_index].framebuffer;
	render_pass_begin_info.renderArea.offset.x = 0;
	render_pass_begin_info.renderArea.offset.y = 0;
	render_pass_begin_info.renderArea.extent = swapchain_extent;
	vk::ClearValue clear_color = { std::array<float, 4>{1.0f, 0.5f, 0.25f, 1.0f} };
	render_pass_begin_info.clearValueCount = 1;
	render_pass_begin_info.pClearValues = &clear_color;

	command_buffer.beginRenderPass(&render_pass_begin_info, vk::SubpassContents::eInline);

	command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, graphics_pipeline);

	command_buffer.draw(3, 1, 0, 0);

	command_buffer.endRenderPass();

	try {

		command_buffer.end();

	}
	catch (vk::SystemError err) {

		if (debug_mode) {

			std::cout << "Failed to finish recording command buffer" << std::endl;

		}
	}

}

void Engine::render() {

	device.waitForFences(1, &in_flight_fence, VK_TRUE, UINT64_MAX);
	device.resetFences(1, &in_flight_fence);
	
	uint32_t image_index{ device.acquireNextImageKHR(swapchain, UINT64_MAX, image_available, nullptr).value };

	vk::CommandBuffer command_buffer = swapchain_frames[image_index].commandbuffer;

	command_buffer.reset();

	recordDrawCommands(command_buffer, image_index);

	vk::SubmitInfo submit_info = {};
	vk::Semaphore wait_semaphores[] = { image_available };
	vk::PipelineStageFlags wait_stages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = wait_semaphores;
	submit_info.pWaitDstStageMask = wait_stages;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &command_buffer;

	vk::Semaphore signal_semaphores[] = { render_finished };
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = signal_semaphores;

	try {

		graphics_queue.submit(submit_info, in_flight_fence);

	}
	catch (vk::SystemError err) {

		if (debug_mode) {

			std::cout << "Failed to submit draw command buffer" << std::endl;

		}

	}

	vk::PresentInfoKHR present_info = {};
	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores = signal_semaphores;
	vk::SwapchainKHR swapchains[] = { swapchain };
	present_info.swapchainCount = 1;
	present_info.pSwapchains = swapchains;
	present_info.pImageIndices = &image_index;

	present_queue.presentKHR(present_info);

}
