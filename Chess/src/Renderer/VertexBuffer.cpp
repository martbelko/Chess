#include "VertexBuffer.h"

#include "WebgpuLib.h"

namespace Chess {

	using namespace Base;

	VertexBuffer::~VertexBuffer()
	{
		ASSERT(m_RendererPtr != nullptr, "Invalid vertex buffer");
		m_RendererPtr.Destroy();
	}

	void VertexBuffer::Bind(wgpu::RenderPassEncoder renderPass, u32 slot) const
	{
		renderPass.SetVertexBuffer(slot, m_RendererPtr, 0, m_RendererPtr.GetSize());
	}

	void VertexBuffer::SetData(const void* data, u64 size, u64 offset)
	{
		ASSERT(size + offset <= m_RendererPtr.GetSize(), "Cannot resize buffer");
		GraphicsContext::GetQueue().WriteBuffer(m_RendererPtr, offset, data, size);
	}

	Ref<VertexBuffer> VertexBuffer::CreateFromData(const void* data, u32 vertexCount, u32 vertexSize)
	{
		wgpu::BufferDescriptor bufferDesc;
		bufferDesc.size = vertexCount * vertexSize;
		bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Vertex;

		wgpu::Buffer vertexBuffer = GraphicsContext::GetDevice().CreateBuffer(&bufferDesc);
		GraphicsContext::GetQueue().WriteBuffer(vertexBuffer, 0, data, bufferDesc.size);
		return CreateRef<VertexBuffer>(vertexBuffer, vertexCount);
	}

	Ref<VertexBuffer> VertexBuffer::CreateFromSize(u32 vertexCount, u32 vertexSize)
	{
		wgpu::BufferDescriptor bufferDesc;
		bufferDesc.size = vertexCount * vertexSize;
		bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Vertex;

		wgpu::Buffer vertexBuffer = GraphicsContext::GetDevice().CreateBuffer(&bufferDesc);
		return CreateRef<VertexBuffer>(vertexBuffer, vertexCount);
	}

}


