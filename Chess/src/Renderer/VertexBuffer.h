#pragma once

#include <WebgpuLib.h>

#include "Core/Base.h"

#include <webgpu/webgpu_cpp.h>

#include <vector>

namespace Chess {

	static u32 GetFormatSize(wgpu::VertexFormat format)
	{
		switch (format)
		{
		case wgpu::VertexFormat::Float32:
			return 4;
		case wgpu::VertexFormat::Float32x2:
			return 4 * 2;
		case wgpu::VertexFormat::Float32x3:
			return 4 * 3;
		case wgpu::VertexFormat::Float32x4:
			return 4 * 4;
		case wgpu::VertexFormat::Uint32:
			return 4;
		}

		ASSERT(false, "Unknown datatype");
		return 0;
	}

	struct BufferElement
	{
		wgpu::VertexFormat format;
		u32 size;
		u32 offset;

		BufferElement(wgpu::VertexFormat format)
			: format(format), size(GetFormatSize(format)), offset(0) {}
	};

	class BufferLayout
	{
	public:
		BufferLayout() {}

		BufferLayout(std::initializer_list<BufferElement> elements, wgpu::VertexStepMode stepMode = wgpu::VertexStepMode::Vertex)
			: m_Elements(elements), m_StepMode(stepMode)
		{
			CalculateOffsetsAndStride();
		}

		u32 GetStride() const { return m_Stride; }
		const std::vector<BufferElement>& GetElements() const { return m_Elements; }
		wgpu::VertexStepMode GetStepMode() const { return m_StepMode; }

		std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

		const BufferElement& operator[](int index) const { return m_Elements[index]; }
		BufferElement& operator[](int index) { return m_Elements[index]; }
	private:
		void CalculateOffsetsAndStride()
		{
			u32 offset = 0;
			m_Stride = 0;
			for (auto& element : m_Elements)
			{
				element.offset = offset;
				offset += element.size;
				m_Stride += element.size;
			}
		}
	private:
		std::vector<BufferElement> m_Elements;
		u32 m_Stride = 0;
		wgpu::VertexStepMode m_StepMode = wgpu::VertexStepMode::Vertex;
	};

	class VertexBuffer
	{
	public:
		VertexBuffer() = default;
		VertexBuffer(wgpu::Buffer rendererPtr, u32 vertexCount)
			: m_RendererPtr(rendererPtr), m_VertexCount(vertexCount) {}
		~VertexBuffer();

		VertexBuffer(const VertexBuffer&) = delete;
		VertexBuffer(VertexBuffer&& other) noexcept = delete;
		VertexBuffer& operator=(const VertexBuffer&) = delete;
		VertexBuffer& operator=(VertexBuffer&& other) noexcept = delete;

		void Bind(wgpu::RenderPassEncoder renderPass, u32 slot) const;

		void SetData(const void* data, u64 size, u64 offset = 0);

		wgpu::Buffer GetRendererPtr() const { return m_RendererPtr; }
		u64 GetSize() const { return m_RendererPtr.GetSize(); }
		u32 GetVertexCount() const { return m_VertexCount; }
	public:
		static Ref<VertexBuffer> CreateFromData(const void* data, u32 vertexCount, u32 vertexSize);
		static Ref<VertexBuffer> CreateFromSize(u32 vertexCount, u32 vertexSize);
	private:
		wgpu::Buffer m_RendererPtr = nullptr;
		u32 m_VertexCount = 0;
	private:
		friend class PipelineBuilder;
	};


}
