#include <WebgpuLib.h>

#include "Core/ChessApp.h"

#include "Renderer/Renderer.h"
#include "Renderer/RenderPipelineBuilder.h"
#include "Renderer/VertexBuffer.h"

#include <imgui/imgui.h>

#include <tinyobjloader/tiny_obj_loader.h>

namespace Chess {

	using namespace Base;

	ChessApp::ChessApp()
	{
		const Base::Window* wnd = GetWindow();
		m_Renderer = new Renderer(wnd->GetWidth(), wnd->GetHeight(), wnd);

		Ref<Shader> shader = Shader::CreateFromFile("Simple.wgsl");

		float vertices[] = {
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			0.0f, 0.5f, 0.0f
		};

		Ref<VertexBuffer> vbo = VertexBuffer::CreateFromData(vertices, sizeof(vertices) / sizeof(float), sizeof(float) * 3);
		BufferLayout vboLayout = {
			{ wgpu::VertexFormat::Float32x3 }
		};

		Ref<Texture> depthTexture = Texture::CreateDepthStencilTexture(wnd->GetWidth(), wnd->GetHeight(), wgpu::TextureFormat::Depth24Plus);
		wgpu::StencilFaceState stencilFaceState{
			.compare = wgpu::CompareFunction::Always,
			.failOp = wgpu::StencilOperation::Keep,
			.depthFailOp = wgpu::StencilOperation::Keep,
			.passOp = wgpu::StencilOperation::Keep
		};

		DepthStencilTextureDesc depthTextureDesc{
			.depthWriteEnabled = true,
			.depthCompare = wgpu::CompareFunction::Less,
			.stencilFront = stencilFaceState,
			.stencilBack = stencilFaceState
		};

		wgpu::TextureViewDescriptor depthTextureViewDesc;
		depthTextureViewDesc.aspect = wgpu::TextureAspect::DepthOnly;
		depthTextureViewDesc.baseArrayLayer = 0;
		depthTextureViewDesc.arrayLayerCount = 1;
		depthTextureViewDesc.baseMipLevel = 0;
		depthTextureViewDesc.mipLevelCount = 1;
		depthTextureViewDesc.dimension = wgpu::TextureViewDimension::e2D;
		depthTextureViewDesc.format = depthTexture->GetRendererPtr().GetFormat();

		m_DepthStencilTextureView = depthTexture->GetRendererPtr().CreateView(&depthTextureViewDesc);

		RenderPipelineBuilder builder;
		builder.AddColorTarget(ColorTargetDesc());
		builder.AddShader(shader);
		builder.AddVertexBuffer(vbo, vboLayout);
		builder.AddDepthStencilTexture(depthTexture, depthTextureDesc);

		m_MainPipeline = builder.Build();

		tinyobj::attrib_t attributes;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warning, error;
		tinyobj::LoadObj(&attributes, &shapes, &materials, &warning, &error, "C:/Users/Martin/Desktop/Chess.obj", "C:/Users/Martin/Desktop/");
	}

	ChessApp::~ChessApp()
	{
		delete m_Renderer;
	}

	void ChessApp::Update(Base::Timestep ts)
	{
		m_LastFrameTime = ts.GetSeconds();
	}

	void ChessApp::Render()
	{
		wgpu::TextureView nextTexture = GraphicsContext::GetSwapChain().GetCurrentTextureView();

		wgpu::CommandEncoderDescriptor commandEncoderDesc;
		commandEncoderDesc.label = "DisplayTexture_CommandEncoder";
		wgpu::CommandEncoder encoder = GraphicsContext::GetDevice().CreateCommandEncoder(&commandEncoderDesc);

		wgpu::RenderPassColorAttachment renderPassColorAttachment;
		renderPassColorAttachment.view = nextTexture;
		renderPassColorAttachment.loadOp = wgpu::LoadOp::Clear;
		renderPassColorAttachment.storeOp = wgpu::StoreOp::Store;
		renderPassColorAttachment.clearValue = wgpu::Color{ 0.0, 0.0, 0.0, 0.0 };

		wgpu::RenderPassDepthStencilAttachment depthStencilAttachment;
		depthStencilAttachment.view = m_DepthStencilTextureView;
		depthStencilAttachment.depthClearValue = 1.0;
		depthStencilAttachment.depthLoadOp = wgpu::LoadOp::Clear;
		depthStencilAttachment.depthStoreOp = wgpu::StoreOp::Store;
		depthStencilAttachment.depthReadOnly = false;
		depthStencilAttachment.stencilReadOnly = true;

		wgpu::RenderPassDescriptor renderPassDesc;
		renderPassDesc.colorAttachmentCount = 1;
		renderPassDesc.colorAttachments = &renderPassColorAttachment;
		renderPassDesc.depthStencilAttachment = &depthStencilAttachment;
		renderPassDesc.timestampWrites = nullptr;
		wgpu::RenderPassEncoder renderPass = encoder.BeginRenderPass(&renderPassDesc);

		m_MainPipeline.Bind(renderPass);

		renderPass.Draw(m_MainPipeline.GetVertexBuffers()[0]->GetVertexCount(), 1, 0, 0);

		renderPass.End();

		wgpu::CommandBufferDescriptor cmdBufferDescriptor2;
		cmdBufferDescriptor2.label = "DisplayTexture_CommandBuffer";
		wgpu::CommandBuffer command2 = encoder.Finish(&cmdBufferDescriptor2);
		GraphicsContext::GetQueue().Submit(1, &command2);

		m_Renderer->Finish();
	}

}

extern Base::Application* CreateApplication()
{
	return new Chess::ChessApp();
}
