#pragma once

#include <WebgpuLib.h>

#include "Renderer/Renderer.h"

#include "Scene/Scene.h"
#include "Scene/Camera.h"

namespace Chess {

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
		float m_LastFrameTime;
		Renderer* m_Renderer;
		Ref<Scene> m_Scene;
		PerspectiveCamera m_Camera;

		Ref<VertexBuffer> m_SceneVbo;
		Ref<DataBuffer> m_ObjectUbo;
		Ref<DataBuffer> m_CameraBuffer;
		Ref<DataBuffer> m_MaterialUbo;
		RenderPipeline m_MainPipeline;
		wgpu::TextureView m_DepthStencilTextureView;
	};

}
