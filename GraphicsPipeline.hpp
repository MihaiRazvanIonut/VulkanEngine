#pragma once

#include "Shaders/Shaders.h"
#include <vulkan/vulkan.hpp>
#include <iostream>

namespace vkInit {

	struct GraphicsPipelineInBundle {

		vk::Device logical_device;
		std::string vertex_file_path;
		std::string fragment_file_path;
		vk::Extent2D swapchain_extent;
		vk::Format swapchain_image_format;

	};

	struct GraphicsPipelineOutBundle {

		vk::PipelineLayout layout;
		vk::RenderPass render_pass;
		vk::Pipeline pipeline;

	};

	vk::PipelineLayout makePipelineLayout(vk::Device logical_device) {

		vk::PipelineLayoutCreateInfo layout_info = {};
		layout_info.flags = vk::PipelineLayoutCreateFlags();
		layout_info.setLayoutCount = 0;
		layout_info.pushConstantRangeCount = 0;

		try {

			return logical_device.createPipelineLayout(layout_info);

		}
		catch(vk::SystemError err){

			std::cerr << "Failed to create pipeline layout!" << std::endl;

		}

	}

	vk::RenderPass makeRenderPass(vk::Device logical_device, vk::Format swapchain_image_format) {

		vk::AttachmentDescription color_attachment = {};
		color_attachment.flags = vk::AttachmentDescriptionFlags();
		color_attachment.format = swapchain_image_format;
		color_attachment.samples = vk::SampleCountFlagBits::e1;
		color_attachment.loadOp = vk::AttachmentLoadOp::eClear;
		color_attachment.storeOp = vk::AttachmentStoreOp::eStore;
		color_attachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
		color_attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		color_attachment.initialLayout = vk::ImageLayout::eUndefined;
		color_attachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

		vk::AttachmentReference color_attachment_refrence = {};
		color_attachment_refrence.attachment = 0;
		color_attachment_refrence.layout = vk::ImageLayout::eColorAttachmentOptimal;

		vk::SubpassDescription subpass = {};
		subpass.flags = vk::SubpassDescriptionFlags();
		subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &color_attachment_refrence;

		vk::RenderPassCreateInfo render_pass_info = {};
		render_pass_info.flags = vk::RenderPassCreateFlags();
		render_pass_info.attachmentCount = 1;
		render_pass_info.pAttachments = &color_attachment;
		render_pass_info.subpassCount = 1;
		render_pass_info.pSubpasses = &subpass;

		try {

			return logical_device.createRenderPass(render_pass_info);

		}
		catch (vk::SystemError err) {

			std::cerr << "Failed to create rander pass!" << std::endl;

		}

	}

	GraphicsPipelineOutBundle makeGraphicsPipeline(bool debug, GraphicsPipelineInBundle specification) {

		vk::GraphicsPipelineCreateInfo graphics_pipeline_info = {};

		graphics_pipeline_info.flags = vk::PipelineCreateFlags();

		std::vector<vk::PipelineShaderStageCreateInfo> shader_stages;

		/// FIRST STAGE: | VERTEX INPUT |

		vk::PipelineVertexInputStateCreateInfo vertex_input_info = {};

		vertex_input_info.flags = vk::PipelineVertexInputStateCreateFlags();
		vertex_input_info.vertexAttributeDescriptionCount = 0;
		vertex_input_info.vertexBindingDescriptionCount = 0;
		graphics_pipeline_info.pVertexInputState = &vertex_input_info;

		/// SECOND STAGE: | INPUT ASSEMBLY |

		vk::PipelineInputAssemblyStateCreateInfo input_assembly_info = {};

		input_assembly_info.flags = vk::PipelineInputAssemblyStateCreateFlags();
		input_assembly_info.topology = vk::PrimitiveTopology::eTriangleList;
		graphics_pipeline_info.pInputAssemblyState = &input_assembly_info;

		/// THIRD STAGE: | VERTEX SHADER |

		vk::ShaderModule vertex_shader_module = vkUtil::createShaderModule(debug, specification.vertex_file_path, specification.logical_device);
		vk::PipelineShaderStageCreateInfo vertex_shader_info = {};
		vertex_shader_info.flags = vk::PipelineShaderStageCreateFlags();
		vertex_shader_info.stage = vk::ShaderStageFlagBits::eVertex;
		vertex_shader_info.module = vertex_shader_module;
		vertex_shader_info.pName = "main";
		shader_stages.push_back(vertex_shader_info);

		/// FOURTH STAGE: | VIEWPORT AND SCISSOR

		vk::Viewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;

		viewport.width = specification.swapchain_extent.width;
		viewport.height = specification.swapchain_extent.height;

		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		vk::Rect2D scissor = {};
		scissor.offset.x = 0.0f;
		scissor.offset.y = 0.0f;
		scissor.extent = specification.swapchain_extent;
		vk::PipelineViewportStateCreateInfo viewport_info = {};
		viewport_info.flags = vk::PipelineViewportStateCreateFlags();
		viewport_info.viewportCount = 1;
		viewport_info.pViewports = &viewport;
		viewport_info.scissorCount = 1;
		viewport_info.pScissors = &scissor;

		graphics_pipeline_info.pViewportState = &viewport_info;

		/// FIFTH STAGE: | Rasterization |

		vk::PipelineRasterizationStateCreateInfo rasterization_info = {};
		rasterization_info.flags = vk::PipelineRasterizationStateCreateFlags();
		rasterization_info.depthClampEnable = VK_FALSE;
		rasterization_info.rasterizerDiscardEnable = VK_FALSE;
		rasterization_info.polygonMode = vk::PolygonMode::eFill;
		rasterization_info.lineWidth = 1.0f;
		rasterization_info.cullMode = vk::CullModeFlagBits::eBack;
		rasterization_info.frontFace = vk::FrontFace::eClockwise;
		rasterization_info.depthBiasEnable = VK_FALSE;
		graphics_pipeline_info.pRasterizationState = &rasterization_info;

		/// SIXTH STAGE: | FRAGMENT SHADER |

		vk::ShaderModule fragment_shader_module = vkUtil::createShaderModule(debug, specification.fragment_file_path, specification.logical_device);
		vk::PipelineShaderStageCreateInfo fragment_shader_info = {};
		fragment_shader_info.flags = vk::PipelineShaderStageCreateFlags();
		fragment_shader_info.stage = vk::ShaderStageFlagBits::eFragment;
		fragment_shader_info.module = fragment_shader_module;
		fragment_shader_info.pName = "main";
		shader_stages.push_back(fragment_shader_info);

		graphics_pipeline_info.stageCount = shader_stages.size();
		graphics_pipeline_info.pStages = shader_stages.data();

		/// SEVENTH STAGE: | MULTISAMPLING |

		vk::PipelineMultisampleStateCreateInfo multisampling_info = {};
		multisampling_info.flags = vk::PipelineMultisampleStateCreateFlags();
		multisampling_info.sampleShadingEnable = VK_FALSE;
		multisampling_info.rasterizationSamples = vk::SampleCountFlagBits::e1;

		graphics_pipeline_info.pMultisampleState = &multisampling_info;

		/// EIGHTH STAGE: | COLOR BLEND |

		vk::PipelineColorBlendAttachmentState color_blend_attachment = {};
		color_blend_attachment.colorWriteMask = vk::ColorComponentFlagBits::eR |
											    vk::ColorComponentFlagBits::eG |
			                                    vk::ColorComponentFlagBits::eB |
			                                    vk::ColorComponentFlagBits::eA ;
		color_blend_attachment.blendEnable = VK_FALSE;

		vk::PipelineColorBlendStateCreateInfo color_blend_info = {};
		color_blend_info.flags = vk::PipelineColorBlendStateCreateFlags();
		color_blend_info.logicOpEnable = VK_FALSE;
		color_blend_info.logicOp = vk::LogicOp::eCopy;
		color_blend_info.attachmentCount = 1;
		color_blend_info.pAttachments = &color_blend_attachment;
		color_blend_info.blendConstants[0] = 0.0f;
		color_blend_info.blendConstants[1] = 0.0f;
		color_blend_info.blendConstants[2] = 0.0f;
		color_blend_info.blendConstants[3] = 0.0f;

		graphics_pipeline_info.pColorBlendState = &color_blend_info;

		/// PIPELINE LAYOUT

		vk::PipelineLayout pipeline_layout = makePipelineLayout(specification.logical_device);
		graphics_pipeline_info.layout = pipeline_layout;


		/// RENDER PASS

		vk::RenderPass render_pass = makeRenderPass(specification.logical_device, specification.swapchain_image_format);
		graphics_pipeline_info.renderPass = render_pass;


		graphics_pipeline_info.basePipelineHandle = nullptr;

		vk::Pipeline graphics_pipeline;

		try {

			graphics_pipeline = (specification.logical_device.createGraphicsPipeline(nullptr, graphics_pipeline_info)).value;

		}
		catch (vk::SystemError err) {

			std::cerr << "Failed to create graphics pipeline!" << std::endl;

		}

		GraphicsPipelineOutBundle output = {};
		output.layout = pipeline_layout;
		output.render_pass = render_pass;
		output.pipeline = graphics_pipeline;

		specification.logical_device.destroyShaderModule(vertex_shader_module);
		specification.logical_device.destroyShaderModule(fragment_shader_module);
		return output;

	}

}