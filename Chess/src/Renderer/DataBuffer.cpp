#include "DataBuffer.h"

#include "WebgpuLib.h"

namespace Chess {

	using namespace Base;

	DataBuffer::~DataBuffer()
	{
		ASSERT(m_RendererPtr != nullptr, "Invalid uniform buffer (nullptr)");
		m_RendererPtr.Destroy();
	}

	void DataBuffer::SetData(const void* data, u64 size, u64 offset)
	{
		ASSERT(size + offset <= m_RendererPtr.GetSize(), "Cannot enlarge buffer");
		GraphicsContext::GetQueue().WriteBuffer(m_RendererPtr, offset, data, size);
	}

	void DataBuffer::Bind(wgpu::RenderPassEncoder renderPass, wgpu::BindGroup bindGroup, u32 groupIndex) const
	{
		renderPass.SetBindGroup(groupIndex, bindGroup, 0, nullptr);
	}

	Ref<DataBuffer> DataBuffer::CreateUniformBufferFromData(const void* data, u64 size)
	{
		wgpu::BufferDescriptor bufferDesc;
		bufferDesc.size = size;
		bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform;

		wgpu::Buffer buffer = GraphicsContext::GetDevice().CreateBuffer(&bufferDesc);
		GraphicsContext::GetQueue().WriteBuffer(buffer, 0, data, size);
		return CreateRef<DataBuffer>(buffer, wgpu::BufferBindingType::Uniform);
	}

	Ref<DataBuffer> DataBuffer::CreateUniformBufferFromSize(u64 size)
	{
		wgpu::BufferDescriptor bufferDesc;
		bufferDesc.size = size;
		bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform;

		wgpu::Buffer buffer = GraphicsContext::GetDevice().CreateBuffer(&bufferDesc);
		return CreateRef<DataBuffer>(buffer, wgpu::BufferBindingType::Uniform);
	}

	Ref<DataBuffer> DataBuffer::CreateStorageBufferFromData(const void* data, u64 size, bool readWrite)
	{
		wgpu::BufferDescriptor bufferDesc;
		bufferDesc.size = size;
		bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Storage;

		wgpu::Buffer buffer = GraphicsContext::GetDevice().CreateBuffer(&bufferDesc);
		GraphicsContext::GetQueue().WriteBuffer(buffer, 0, data, size);

		return CreateRef<DataBuffer>(buffer, readWrite ? wgpu::BufferBindingType::Storage : wgpu::BufferBindingType::ReadOnlyStorage);
	}

	Ref<DataBuffer> DataBuffer::CreateStorageBufferFromSize(u64 size, bool readWrite)
	{
		wgpu::BufferDescriptor bufferDesc;
		bufferDesc.size = size;
		bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Storage;

		wgpu::Buffer buffer = GraphicsContext::GetDevice().CreateBuffer(&bufferDesc);
		return CreateRef<DataBuffer>(buffer, readWrite ? wgpu::BufferBindingType::Storage : wgpu::BufferBindingType::ReadOnlyStorage);
	}

}
