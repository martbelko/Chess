#pragma once

#include "Core/Base.h"

#include "Renderer/RenderPipeline.h"

#include <webgpu/webgpu_cpp.h>

#include <glm/glm.hpp>

#include <vector>

namespace Chess {

	class Renderer
	{
	public:
		Renderer(u32 width, u32 height);

		void BeginScene();
		void EndScene();

		void BeginImGui();
		void EndImGui();

		void Finish();

		void OnWindowResize(u32 width, u32 height);
	private:
		u32 m_ViewportWidth, m_ViewportHeight;
	};

}
