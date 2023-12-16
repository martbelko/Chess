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
		glm::vec4 position;
	};

	ChessApp::ChessApp()
	{
		const Base::Window* wnd = GetWindow();
		m_Renderer = new Renderer(wnd->GetWidth(), wnd->GetHeight(), wnd);

		objl::Loader loader;
		if (!loader.LoadFile("C:/Users/Pc/Desktop/chessboard.obj"))
		{
			LOG_ERROR("Could not load .obj file");
		}
		else
		{
			std::vector<Vertex3D> vertices;
			std::vector<u64> counts;
			std::vector<Material> materials;
			counts.reserve(loader.LoadedMeshes.size());

			auto objlVec3ToGlm = [](const objl::Vector3 v) -> glm::vec3
			{
				float x = v.X;
				float y = v.Y;
				float z = v.Z;
				return glm::vec3(x, y, z);
			};

			for (const objl::Mesh& mesh : loader.LoadedMeshes)
			{
				for (u32 index : mesh.Indices)
				{
					const objl::Vertex& vertex = mesh.Vertices[index];

					float px = vertex.Position.X;
					float py = vertex.Position.Y;
					float pz = vertex.Position.Z;

					float nx = vertex.Normal.X;
					float ny = vertex.Normal.Y;
					float nz = vertex.Normal.Z;

					float uvx = vertex.TextureCoordinate.X;
					float uvy = vertex.TextureCoordinate.Y;

					vertices.push_back(Vertex3D{ .position = { px, py, pz }, .normal = { nx, ny, nz }, .uv = { uvx, uvy } });
				}

				glm::vec4 ambient = glm::vec4(objlVec3ToGlm(mesh.MeshMaterial.Ka), 1.0f);
				glm::vec4 diffuse = glm::vec4(objlVec3ToGlm(mesh.MeshMaterial.Kd), 1.0f);
				glm::vec4 specular = glm::vec4(objlVec3ToGlm(mesh.MeshMaterial.Ks), 1.0f);

				materials.push_back(Material(ambient, diffuse, specular));
				counts.push_back(vertices.size());
			}

			m_SceneVbo = VertexBuffer::CreateFromData(vertices.data(), vertices.size(), sizeof(Vertex3D));

			u64 from = 0;
			for (size_t i = 0; i < counts.size(); ++i)
			{
				u64 count = counts[i];
				const Material& mat = materials[i];

				VertexBufferView vboView = VertexBufferView(m_SceneVbo, from, count - 1);
				from = count;

				Node* parent = m_Scene.GetSceneGraph().GetRoot();

				Mesh mesh = Mesh(vboView, mat);
				Node* node = new Node(mesh);
				m_Scene.GetSceneGraph().AddNode(node, parent);
			}
		}

		CreateViewportBasedPipelines();
	}

	ChessApp::~ChessApp()
	{
		delete m_Renderer;
	}

	void ChessApp::CreateViewportBasedPipelines()
	{
		const Window* wnd = GetWindow();
		Ref<Shader> shader = Shader::CreateFromFile("Simple.wgsl");

		m_CameraBuffer = DataBuffer::CreateUniformBufferFromSize(sizeof(CameraBuffer));
		DataBufferLayout cameraBufferLayout{
			{ sizeof(CameraBuffer), wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment }
		};

		BufferLayout vboLayout = {
			{ wgpu::VertexFormat::Float32x3 },
			{ wgpu::VertexFormat::Float32x3 },
			{ wgpu::VertexFormat::Float32x2 }
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
		builder.AddVertexBuffer(m_SceneVbo, vboLayout);
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

		m_Scene.GetSceneGraph().Update();
	}

	void ChessApp::Render()
	{
		float aspect = static_cast<float>(GetWindow()->GetWidth()) / GetWindow()->GetHeight();
		m_Camera.SetProjection(glm::radians(75.0f), aspect, 0.01f, 1000.0f);

		CameraBuffer cameraBuffer;
		cameraBuffer.view = m_Camera.GetViewMatrix();
		cameraBuffer.projection = m_Camera.GetProjectionMatrix();
		cameraBuffer.viewProjection = m_Camera.GetViewProjectionMatrix();
		cameraBuffer.position = glm::vec4(m_Camera.GetPosition(), 1.0f);

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

		for (const Node* node : m_Scene.GetSceneGraph().GetRoot()->children)
		{
			u64 from = node->mesh.vboView.from;
			u64 to = node->mesh.vboView.to;

			renderPass.Draw(to - from + 1, 1, from, 0);
		}

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
