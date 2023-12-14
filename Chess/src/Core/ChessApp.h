#pragma once

#include <WebgpuLib.h>

#include "Renderer/Renderer.h"

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

		RenderPipeline m_MainPipeline;
		Ref<DataBuffer> m_CameraBuffer;
		wgpu::TextureView m_DepthStencilTextureView;
	};

}
