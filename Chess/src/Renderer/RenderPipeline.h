#pragma once

#include "Core/Base.h"

#include "Renderer/VertexBuffer.h"
#include "Renderer/DataBuffer.h"

#include <webgpu/webgpu_cpp.h>

#include <vector>

namespace Chess {

	class RenderPipeline
	{
	public:
		RenderPipeline(wgpu::RenderPipeline pipeline = nullptr)
			: m_RendererPtr(pipeline) {}

		void AddVertexBuffer(const Ref<VertexBuffer>& vbo) { m_VertexBuffers.push_back(vbo); }
		void SetVertexBuffer(u32 slot, const Ref<VertexBuffer>& vbo);

		void AddDataBuffer(const Ref<DataBuffer>& dataBuffer, wgpu::BindGroup bindBroup) { m_DataBuffers.emplace_back(dataBuffer, bindBroup); }

		void SetDepthStencilTexture(const Ref<Texture>& depthStencilTexture) { m_DepthStencilTexture = depthStencilTexture; }

		void Bind(wgpu::RenderPassEncoder renderPass);

		wgpu::RenderPipeline GetRendererPtr() const { return m_RendererPtr; }
		const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const { return m_VertexBuffers; }
		const std::vector<std::pair<Ref<DataBuffer>, wgpu::BindGroup>>& GetDataBuffers() const { return m_DataBuffers; }
	private:
		wgpu::RenderPipeline m_RendererPtr;
		Ref<Texture> m_DepthStencilTexture = nullptr;
		std::vector<Ref<VertexBuffer>> m_VertexBuffers;
		std::vector<std::pair<Ref<DataBuffer>, wgpu::BindGroup>> m_DataBuffers;
	};

}
