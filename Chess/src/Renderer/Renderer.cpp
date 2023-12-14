#include "Renderer.h"

#include "WebgpuLib.h"

#include "Renderer/RenderPipelineBuilder.h"
#include "Renderer/Texture.h"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_wgpu.h>

#include <glm/gtc/matrix_transform.hpp>

#include <array>

namespace Chess {

	using namespace Base;

	static u64 AlignStorageBufferSize(u64 size)
	{
		u32 alignment = GraphicsContext::GetLimits().minStorageBufferOffsetAlignment;
		return (size + alignment - 1) & ~(alignment - 1);
	}

	Renderer::Renderer(u32 width, u32 height)
		: m_ViewportWidth(width), m_ViewportHeight(height)
	{
	}

	void Renderer::BeginScene()
	{
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::BeginImGui()
	{
		ImGui_ImplWGPU_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void Renderer::EndImGui()
	{
		wgpu::RenderPassColorAttachment colorAttachment;
		colorAttachment.view = GraphicsContext::GetSwapChain().GetCurrentTextureView();
		colorAttachment.loadOp = wgpu::LoadOp::Load;
		colorAttachment.storeOp = wgpu::StoreOp::Store;

		wgpu::CommandEncoderDescriptor commandEncoderDesc;
		commandEncoderDesc.label = "ImGUI_Command_Encoder";
		wgpu::CommandEncoder encoder = GraphicsContext::GetDevice().CreateCommandEncoder(&commandEncoderDesc);

		wgpu::RenderPassDescriptor renderPassDesc;
		renderPassDesc.colorAttachmentCount = 1;
		renderPassDesc.colorAttachments = &colorAttachment;
		renderPassDesc.depthStencilAttachment = nullptr;
		renderPassDesc.timestampWrites = nullptr;
		wgpu::RenderPassEncoder renderPass = encoder.BeginRenderPass(&renderPassDesc);

		ImGui::Render();
		ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), renderPass.Get());

		renderPass.End();

		wgpu::CommandBufferDescriptor cmdBufferDescriptor;
		cmdBufferDescriptor.label = "ImGUI_Command_Buffer";
		wgpu::CommandBuffer command = encoder.Finish(&cmdBufferDescriptor);
		GraphicsContext::GetQueue().Submit(1, &command);
	}

	void Renderer::Finish()
	{
#if !defined(SS_PLATFORM_WEB)
		GraphicsContext::GetSwapChain().Present();
		GraphicsContext::GetDevice().Tick();
#endif
	}

	void Renderer::OnWindowResize(u32 width, u32 height)
	{
		if (width == 0 || height == 0)
			return;

		GraphicsContext::OnWindowResize(width, height);
		m_ViewportWidth = width;
		m_ViewportHeight = height;
	}

}
