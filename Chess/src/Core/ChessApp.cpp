#include <WebgpuLib.h>

#include "Core/ChessApp.h"

#include "Renderer/Renderer.h"
#include "Renderer/RenderPipelineBuilder.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/VertexBufferView.h"
#include "Renderer/Material.h"

#include "Scene/ObjLoader.h"
#include "Scene/Components.h"
#include "Scene/Node.h"
#include "Scene/Entity.h"

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

		m_RenderObjectsBuffer.reserve(MAX_INSTANCE_OBJECT_COUNT);
		m_RenderMaterialsBuffer.reserve(MAX_INSTANCE_OBJECT_COUNT);

		m_Scene = CreateRef<Scene>();

		constexpr float CELL_SIZE = 4.30658f;

		objl::Loader loader;
		if (!loader.LoadFile("C:/Users/Pc/Desktop/chess-simple.obj"))
		{
			LOG_ERROR("Could not load .obj file");
		}
		else
		{
			struct MeshDesc
			{
				std::string name;
				u64 endIndex;
				Ref<Material> material;
			};

			std::vector<Vertex3D> vertices;
			std::vector<MeshDesc> meshDescs;
			meshDescs.reserve(loader.LoadedMeshes.size());

			auto objlVec3ToGlm = [](const objl::Vector3 v) -> glm::vec3
			{
				float x = v.X;
				float y = v.Y;
				float z = v.Z;
				return glm::vec3(x, y, z);
			};

			for (const objl::Material& loadedMat : loader.LoadedMaterials)
			{
				Ref<Material> material = CreateRef<Material>(
					glm::vec4(objlVec3ToGlm(loadedMat.Ka), 1.0f),
					glm::vec4(objlVec3ToGlm(loadedMat.Kd), 1.0f),
					glm::vec4(objlVec3ToGlm(loadedMat.Ks), 1.0f)
				);

				m_MaterialSystem.RegisterMaterial(loadedMat.name, material);
			}

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

				meshDescs.push_back(MeshDesc{
					.name = mesh.MeshName,
					.endIndex = vertices.size(),
					.material = m_MaterialSystem.GetMaterialByName(mesh.MeshMaterial.name)
				});
			}

			m_SceneVbo = VertexBuffer::CreateFromData(vertices.data(), vertices.size(), sizeof(Vertex3D));

			Node* chessboard = m_Scene->AddNode(m_Scene->GetRoot(), "Chessboard");
			Node* chessPieces = m_Scene->AddNode(chessboard, "ChessPieces");
			Node* chessGround = m_Scene->AddNode(chessboard, "ChessGround");

			u64 from = 0;
			for (size_t i = 0; i < meshDescs.size(); i++)
			{
				u64 endIndex = meshDescs[i].endIndex;
				Ref<Material> material = meshDescs[i].material;

				VertexBufferView vboView = VertexBufferView(m_SceneVbo, from, endIndex - 1);
				from = endIndex;

				Ref<Mesh> mesh = CreateRef<Mesh>(vboView);

				std::string name = meshDescs[i].name;
				if (name == "Chessboard" || name == "Chessboard_2")
				{
					if (name == "Chessboard")
					{
						name = "Chessboard_1";
					}

					Node* node = m_Scene->AddNode(chessboard, name);
					node->entity.AddComponent<MeshComponent>(mesh, material);
				}
				else
				{
					if (name.length() == 2)
					{
						auto positionToString = [](u8 x, u8 y) -> std::string
						{
							char ch = x + 'a';
							return ch + std::to_string(y + 1);
						};

						u8 offset = name == "a1" ? 0 : 1;
						glm::vec3 dec = { 0.0f, 0.0f, 0.0f };
						if (offset == 1)
						{
							dec.x = -CELL_SIZE;
						}

						for (u8 y = 0; y < 8; y++)
						{
							for (u8 x = offset; x < 8; x += 2)
							{
								float xOffset = x * CELL_SIZE;
								float yOffset = y * CELL_SIZE;

								std::string placeName = positionToString(x, y);

								Node* node = m_Scene->AddNode(chessGround, placeName);
								node->entity.AddComponent<MeshComponent>(mesh, material);
								node->entity.GetComponent<TransformComponent>().translation += glm::vec3(xOffset, 0.0f, -yOffset) + dec;
							}

							offset = offset == 0 ? 1 : 0;
						}
					}
					else
					{
						Node* node = m_Scene->AddNode(chessPieces, name);
						node->entity.AddComponent<MeshComponent>(mesh, material);
					}
				}
			}
		}

		CreateViewportBasedPipelines();

		auto makeMove = [&](i8 fromX, i8 fromY, i8 toX, i8 toY, TransformComponent& tc) -> void
		{
			float xDiff = toX - fromX;
			float yDiff = toY - fromY;
			glm::vec3 diff = glm::vec3(xDiff, 0.0f, -yDiff) * CELL_SIZE;
			tc.translation += diff;
		};

		for (Node* node : m_Scene->GetNodeByName("ChessPieces")->children)
		{
			TransformComponent& tc = node->entity.GetComponent<TransformComponent>();
			makeMove(0, 0, 2, 2, tc);
		}

		for (Node* node : m_Scene->CreateView<MeshComponent>())
		{
			const std::string& tag = node->entity.GetComponent<TagComponent>().tag;
			if (tag.starts_with("Chessboard") || tag.size() == 2)
			{
				continue;
			}

			TransformComponent& tc = node->entity.GetComponent<TransformComponent>();
			//makeMove(0, 0, 2, 2, tc);
		}

		TransformComponent& tc = m_Scene->GetNodeByName("Chessboard")->entity.GetComponent<TransformComponent>();
		//tc.translation = glm::vec3(10.0f, 0.0f, 0.0f);
	}

	ChessApp::~ChessApp()
	{
		delete m_Renderer;
	}

	void ChessApp::SetChessboardRender()
	{
		for (const Piece& piece : m_ChessState.GetPieces())
		{
			if (piece.IsActive())
			{
				continue;
			}


		}
	}

	void ChessApp::CreateViewportBasedPipelines()
	{
		const Window* wnd = GetWindow();
		Ref<Shader> shader = Shader::CreateFromFile("Simple.wgsl");

		m_ObjectUbo = DataBuffer::CreateUniformBufferFromSize(sizeof(ObjectBuffer) * MAX_INSTANCE_OBJECT_COUNT);
		m_CameraBuffer = DataBuffer::CreateUniformBufferFromSize(sizeof(CameraBuffer));
		m_MaterialUbo = DataBuffer::CreateUniformBufferFromSize(sizeof(MaterialBuffer) * MAX_INSTANCE_OBJECT_COUNT);

		DataBufferLayout objectBufferLayout{
			{ sizeof(ObjectBuffer) * MAX_INSTANCE_OBJECT_COUNT, wgpu::ShaderStage::Vertex }
		};

		DataBufferLayout cameraBufferLayout{
			{ sizeof(CameraBuffer), wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment }
		};

		DataBufferLayout materialBufferLayout{
			{ sizeof(MaterialBuffer) * MAX_INSTANCE_OBJECT_COUNT, wgpu::ShaderStage::Fragment }
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
		builder.AddDataBuffer(m_ObjectUbo, objectBufferLayout);
		builder.AddDataBuffer(m_CameraBuffer, cameraBufferLayout);
		builder.AddDataBuffer(m_MaterialUbo, materialBufferLayout);

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

		m_Scene->Update();
	}

	void ChessApp::Render()
	{
		std::vector<Node*> nodes = m_Scene->CreateView<MeshComponent>();
		if (nodes.empty())
		{
			return;
		}

		std::sort(nodes.begin(), nodes.end(), [](Node* a, Node* b)
		{
			const MeshComponent& mesh1component = a->entity.GetComponent<MeshComponent>();
			const MeshComponent& mesh2component = b->entity.GetComponent<MeshComponent>();
			Ref<Mesh> mesh1 = mesh1component.mesh;
			Ref<Mesh> mesh2 = mesh2component.mesh;
			Ref<Material> mat1 = mesh1component.material;
			Ref<Material> mat2 = mesh2component.material;

			return mesh1.get() < mesh2.get() || (mesh1.get() == mesh2.get() && mat1.get() < mat2.get());
		});

		float aspect = static_cast<float>(GetWindow()->GetWidth()) / GetWindow()->GetHeight();
		m_Camera.SetProjection(glm::radians(75.0f), aspect, 0.01f, 1000.0f);

		CameraBuffer cameraBuffer;
		cameraBuffer.view = m_Camera.GetViewMatrix();
		cameraBuffer.projection = m_Camera.GetProjectionMatrix();
		cameraBuffer.viewProjection = m_Camera.GetViewProjectionMatrix();
		cameraBuffer.position = glm::vec4(m_Camera.GetPosition(), 1.0f);

		m_CameraBuffer->SetData(&cameraBuffer, sizeof(CameraBuffer));

		wgpu::TextureView nextTexture = GraphicsContext::GetSwapChain().GetCurrentTextureView();

		wgpu::RenderPassColorAttachment renderPassColorAttachment;
		renderPassColorAttachment.view = nextTexture;
		renderPassColorAttachment.loadOp = wgpu::LoadOp::Clear;
		renderPassColorAttachment.storeOp = wgpu::StoreOp::Store;
		renderPassColorAttachment.clearValue = wgpu::Color{ 0.2, 0.3, 0.4, 0.0 };

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

		wgpu::CommandEncoderDescriptor commandEncoderDesc;
		commandEncoderDesc.label = "CommandEncoder";

		Ref<Mesh> currentMesh = nodes[0]->entity.GetComponent<MeshComponent>().mesh;
		Ref<Material> currentMaterial = nodes[0]->entity.GetComponent<MeshComponent>().material;

		size_t nodeIndexStart = 0, nodeIndexEnd = 0;

		m_RenderObjectsBuffer.clear();
		m_RenderMaterialsBuffer.clear();

		u32 drawCalls = 0;
		for (Node* node : nodes)
		{
			const MeshComponent& mc = node->entity.GetComponent<MeshComponent>();
			if (currentMesh == mc.mesh && currentMaterial == mc.material && m_RenderObjectsBuffer.size() < MAX_INSTANCE_OBJECT_COUNT)
			{
				const TransformComponent& transformComponent = node->entity.GetComponent<TransformComponent>();
				const MeshComponent& meshComponent = node->entity.GetComponent<MeshComponent>();
				const Ref<Material> material = meshComponent.material;

				ObjectBuffer objectBuffer{
					.model = transformComponent.worldMatrix,
					.modelInv = glm::inverse(transformComponent.worldMatrix)
				};

				MaterialBuffer materialBuffer{
					.ambient = material->ambient,
					.diffuse = material->diffuse,
					.specular = material->specular
				};

				m_RenderObjectsBuffer.push_back(std::move(objectBuffer));
				m_RenderMaterialsBuffer.push_back(std::move(materialBuffer));

				nodeIndexEnd++;
			}
			else
			{
				wgpu::CommandEncoder encoder = GraphicsContext::GetDevice().CreateCommandEncoder(&commandEncoderDesc);
				wgpu::RenderPassEncoder renderPass = encoder.BeginRenderPass(&renderPassDesc);

				Node* renderNode = nodes[nodeIndexStart];
				const TransformComponent& transformComponent = renderNode->entity.GetComponent<TransformComponent>();
				const MeshComponent& meshComponent = renderNode->entity.GetComponent<MeshComponent>();

				u64 from = meshComponent.mesh->vboView.from;
				u64 to = meshComponent.mesh->vboView.to;
				const Ref<Material>& material = meshComponent.material;

				m_ObjectUbo->SetData(m_RenderObjectsBuffer.data(), sizeof(ObjectBuffer) * m_RenderObjectsBuffer.size());
				m_MaterialUbo->SetData(m_RenderMaterialsBuffer.data(), sizeof(MaterialBuffer) * m_RenderMaterialsBuffer.size());

				m_MainPipeline.Bind(renderPass);

				renderPass.Draw(to - from + 1, m_RenderObjectsBuffer.size(), from, 0);
				renderPass.End();

				++drawCalls;

				m_RenderObjectsBuffer.clear();
				m_RenderMaterialsBuffer.clear();

				wgpu::CommandBufferDescriptor cmdBufferDescriptor;
				cmdBufferDescriptor.label = "CommandBuffer";
				wgpu::CommandBuffer command = encoder.Finish(&cmdBufferDescriptor);
				GraphicsContext::GetQueue().Submit(1, &command);

				renderPassColorAttachment.loadOp = wgpu::LoadOp::Load;
				depthStencilAttachment.depthLoadOp = wgpu::LoadOp::Load;

				nodeIndexStart = nodeIndexEnd++;

				const TransformComponent& nextTransformComponent = node->entity.GetComponent<TransformComponent>();
				const MeshComponent& nextMeshComponent = node->entity.GetComponent<MeshComponent>();

				currentMesh = nextMeshComponent.mesh;
				currentMaterial = nextMeshComponent.material;

				ObjectBuffer objectBuffer{
					.model = nextTransformComponent.worldMatrix,
					.modelInv = glm::inverse(nextTransformComponent.worldMatrix)
				};

				MaterialBuffer materialBuffer{
					.ambient = nextMeshComponent.material->ambient,
					.diffuse = nextMeshComponent.material->diffuse,
					.specular = nextMeshComponent.material->specular
				};

				m_RenderObjectsBuffer.push_back(std::move(objectBuffer));
				m_RenderMaterialsBuffer.push_back(std::move(materialBuffer));
			}
		}

		m_Renderer->Finish();
	}

}

extern Base::Application* CreateApplication()
{
	return new Chess::ChessApp();
}
