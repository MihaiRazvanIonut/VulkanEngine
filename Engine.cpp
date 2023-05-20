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

	device.destroyCommandPool(command_pool);

	device.destroyPipeline(graphics_pipeline);
	device.destroyPipelineLayout(graphics_pipeline_layout);
	device.destroyRenderPass(graphics_pipeline_render_pass);

	cleanupSwapchain();

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
	makeSwapchain();
	frame_number = 0;

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

	makeFramebuffers();

	command_pool = vkInit::makeCommandPool(debug_mode, device, physical_device, surface);

	vkInit::CommandBufferInputChunk command_buffer_input_chunk = { device, command_pool, swapchain_frames };
	main_command_buffer = vkInit::makeCommandBuffer(debug_mode, command_buffer_input_chunk);

	vkInit::makeFrameCommandBuffers(debug_mode, command_buffer_input_chunk);

	makeFrameSynchronizationObjects();

}

void Engine::makeFrameSynchronizationObjects() {

	for (vkUtil::SwapChainFrame& frame : swapchain_frames) {

		frame.in_flight = vkInit::makeFence(debug_mode, device);
		frame.image_available = vkInit::makeSemaphore(debug_mode, device);
		frame.render_finished = vkInit::makeSemaphore(debug_mode, device);

	}

}

void Engine::makeFramebuffers() {


	vkInit::FrameBufferInput framebuffer_input;
	framebuffer_input.logical_device = device;
	framebuffer_input.render_pass = graphics_pipeline_render_pass;
	framebuffer_input.swapchain_extent = swapchain_extent;
	vkInit::makeFramebuffers(framebuffer_input, swapchain_frames);

}

void Engine::recordDrawCommands(vk::CommandBuffer command_buffer, uint32_t image_index, Scene* scene) {

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

	for (glm::vec3 position : scene->triangle_positions) {

		glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
		vkUtil::ObjectData object_data;
		object_data.model = model;
		command_buffer.pushConstants(graphics_pipeline_layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(object_data), &object_data);
		command_buffer.draw(3, 1, 0, 0);

	}

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

void Engine::render(Scene* scene) {

	device.waitForFences(1, &swapchain_frames[frame_number].in_flight, VK_TRUE, UINT64_MAX);
	
	uint32_t image_index;

	try {
		vk::ResultValue aquire = device.acquireNextImageKHR(swapchain, UINT64_MAX, swapchain_frames[frame_number].image_available, nullptr);
		image_index = aquire.value;
	}
	catch (vk::OutOfDateKHRError err) {

		recreateSwapchain();
		return;

	}

	vk::CommandBuffer command_buffer = swapchain_frames[frame_number].commandbuffer;

	command_buffer.reset();

	recordDrawCommands(command_buffer, image_index, scene);

	vk::SubmitInfo submit_info = {};
	vk::Semaphore wait_semaphores[] = { swapchain_frames[frame_number].image_available };
	vk::PipelineStageFlags wait_stages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = wait_semaphores;
	submit_info.pWaitDstStageMask = wait_stages;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &command_buffer;

	vk::Semaphore signal_semaphores[] = { swapchain_frames[frame_number].render_finished };
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = signal_semaphores;

	device.resetFences(1, &swapchain_frames[frame_number].in_flight);

	try {

		graphics_queue.submit(submit_info, swapchain_frames[frame_number].in_flight);

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

	vk::Result present;

	try {
		present = present_queue.presentKHR(present_info);
	}
	catch (vk::OutOfDateKHRError err) {

		present = vk::Result::eErrorOutOfDateKHR;

	}
	if (present == vk::Result::eErrorOutOfDateKHR || present == vk::Result::eSuboptimalKHR) {

		recreateSwapchain();
		return;

	}

	frame_number = (frame_number + 1) % max_frames_in_flight;

}

void Engine::makeSwapchain() {


	vkInit::SwapChainBundle bundle = vkInit::createSwapChain(debug_mode, device, physical_device, surface, width, height);

	swapchain = bundle.swapchain;
	swapchain_frames = bundle.frames;
	swapchain_format = bundle.format;
	swapchain_extent = bundle.extent;
	max_frames_in_flight = static_cast<int> (swapchain_frames.size());



}

void Engine::recreateSwapchain() {

	width = 0;
	height = 0;

	while (width == 0 || height == 0) {

		glfwGetFramebufferSize(window, &width, &height);
		glfwWaitEvents();

	}

	device.waitIdle();

	cleanupSwapchain();
	makeSwapchain();
	makeFramebuffers();
	makeFrameSynchronizationObjects();

	vkInit::CommandBufferInputChunk command_buffer_input_chunk = { device, command_pool, swapchain_frames };
	vkInit::makeFrameCommandBuffers(debug_mode, command_buffer_input_chunk);

}

void Engine::cleanupSwapchain() {

	for (auto& frame : swapchain_frames) {

		device.destroyImageView(frame.image_view);
		device.destroyFramebuffer(frame.framebuffer);
		device.destroyFence(frame.in_flight);
		device.destroySemaphore(frame.image_available);
		device.destroySemaphore(frame.render_finished);

	}

	device.destroySwapchainKHR(swapchain);

}
