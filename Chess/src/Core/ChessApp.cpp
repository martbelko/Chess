#include <WebgpuLib.h>

#include "Core/ChessApp.h"

#include "Renderer/Renderer.h"
#include "Renderer/RenderPipelineBuilder.h"
#include "Renderer/VertexBuffer.h"

#include "Scene/ObjLoader.h"

#include <imgui/imgui.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <tinyobjloader/tiny_obj_loader.h>

#include <GLFW/glfw3.h>

namespace Chess {

	using namespace Base;

	struct CameraBuffer
	{
		glm::mat4 view;
		glm::mat4 projection;
		glm::mat4 viewProjection;
	};

	ChessApp::ChessApp()
	{
		const Base::Window* wnd = GetWindow();
		m_Renderer = new Renderer(wnd->GetWidth(), wnd->GetHeight(), wnd);

		CreateViewportBasedPipelines();
	}

	ChessApp::~ChessApp()
	{
		delete m_Renderer;
	}

	void ChessApp::CreateViewportBasedPipelines()
	{
		objl::Loader loader;
		bool loadout = loader.LoadFile("C:/Users/Pc/Desktop/chess.obj");

		std::vector<Vertex3D> vertices;
		for (const auto& vertex : loader.LoadedMeshes[16].Vertices)
		{
			float x = vertex.Position.X;
			float y = vertex.Position.Y;
			float z = vertex.Position.Z;
			vertices.push_back(Vertex3D{ .position = { x, y, z } });
		}

		const Window* wnd = GetWindow();
		Ref<Shader> shader = Shader::CreateFromFile("Simple.wgsl");

		m_CameraBuffer = DataBuffer::CreateUniformBufferFromSize(sizeof(CameraBuffer));
		DataBufferLayout cameraBufferLayout{
			{ sizeof(CameraBuffer), wgpu::ShaderStage::Vertex }
		};

		Ref<VertexBuffer> vbo = VertexBuffer::CreateFromData(vertices.data(), vertices.size(), sizeof(Vertex3D));
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
		builder.AddDataBuffer(m_CameraBuffer, cameraBufferLayout);

		m_MainPipeline = builder.Build();
	}

	void ChessApp::Update(Base::Timestep ts)
	{
		m_LastFrameTime = ts.GetSeconds();

		const WindowResizedEvent* lastWindowResizeEvent = nullptr;
		for (const Event& ev : GetWindow()->GetEvents())
		{
			const float camSensitivity = 50.0f * ts.GetSeconds();
			switch (ev.type)
			{
			case EventType::WindowResized:
				lastWindowResizeEvent = &ev.as.windowResizedEvent;
				break;
			case EventType::MouseMoved:
			{
				static glm::vec2 lastPos = { ev.as.mouseMovedEvent.xPos, ev.as.mouseMovedEvent.yPos };
				glm::vec2 currentPos = { ev.as.mouseMovedEvent.xPos, ev.as.mouseMovedEvent.yPos };
				glm::vec2 delta = currentPos - lastPos;
				m_Camera.OnMouseMove(delta * camSensitivity);
				lastPos = currentPos;
				break;
			}
			case Base::EventType::MousePressed:
				m_Camera.OnMouseButtonPressed(ev.as.mousePressedEvent);
				break;
			case Base::EventType::MouseReleased:
				m_Camera.OnMouseButtonReleased(ev.as.mouseReleasedEvent);
				break;
			}
		}

		if (lastWindowResizeEvent)
		{
			m_Renderer->OnWindowResize(lastWindowResizeEvent->width, lastWindowResizeEvent->height);
			CreateViewportBasedPipelines();
		}
	}

	void ChessApp::Render()
	{
		float aspect = static_cast<float>(GetWindow()->GetWidth()) / GetWindow()->GetHeight();
		m_Camera.SetProjection(glm::radians(75.0f), aspect, 0.01f, 1000.0f);

		CameraBuffer cameraBuffer;
		cameraBuffer.view = m_Camera.GetViewMatrix();
		cameraBuffer.projection = m_Camera.GetProjectionMatrix();
		cameraBuffer.viewProjection = m_Camera.GetViewProjectionMatrix();

		m_CameraBuffer->SetData(&cameraBuffer, sizeof(CameraBuffer));

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
