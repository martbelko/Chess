#include "RenderPipeline.h"

#include "WebgpuLib.h"

namespace Chess {

	void RenderPipeline::Bind(wgpu::RenderPassEncoder renderPass)
	{
		renderPass.SetPipeline(m_RendererPtr);

		for (u32 slot = 0; slot < m_VertexBuffers.size(); ++slot)
		{
			const Ref<VertexBuffer>& vbo = m_VertexBuffers[slot];
			ASSERT(vbo != nullptr, "Vertex buffer was not set");
			renderPass.SetVertexBuffer(slot, vbo->GetRendererPtr(), 0, vbo->GetSize());
		}

		for (u32 slot = 0; slot < m_DataBuffers.size(); ++slot)
		{
			m_DataBuffers[slot].first->Bind(renderPass, m_DataBuffers[slot].second, slot);
		}
	}

	void RenderPipeline::SetVertexBuffer(u32 slot, const Ref<VertexBuffer>& vbo)
	{
		ASSERT(slot < m_VertexBuffers.size(), "No buffer in this slot");
		m_VertexBuffers[slot] = vbo;
	}

}
