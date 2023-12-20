#pragma once

#include <WebgpuLib.h>

#include "Renderer/Renderer.h"
#include "Renderer/MaterialSystem.h"

#include "Scene/Scene.h"
#include "Scene/Camera.h"

#include "Game/ChessState.h"

namespace Chess {

	struct ObjectBuffer
	{
		glm::mat4 model;
		glm::mat4 modelInv;
	};

	struct MaterialBuffer
	{
		glm::vec4 ambient;
		glm::vec4 diffuse;
		glm::vec4 specular;
	};

	class ChessApp : public Base::Application
	{
	public:
		ChessApp();
		~ChessApp();

		void CreateViewportBasedPipelines();

		virtual void Update(Base::Timestep ts) override;
		virtual void Render() override;

		virtual void RenderImGui() override {};
	private:
		void SetChessboardRender();
	private:
		float m_LastFrameTime;
		Renderer* m_Renderer;
		Ref<Scene> m_Scene;
		PerspectiveCamera m_Camera;

		MaterialSystem m_MaterialSystem;
		Ref<VertexBuffer> m_SceneVbo;
		Ref<DataBuffer> m_ObjectUbo;
		Ref<DataBuffer> m_CameraBuffer;
		Ref<DataBuffer> m_MaterialUbo;
		RenderPipeline m_MainPipeline;
		wgpu::TextureView m_DepthStencilTextureView;
		std::vector<ObjectBuffer> m_RenderObjectsBuffer;
		std::vector<MaterialBuffer> m_RenderMaterialsBuffer;

		ChessboardState m_ChessState;
	private:
		static inline constexpr u32 MAX_INSTANCE_OBJECT_COUNT = 10;
	};

}
