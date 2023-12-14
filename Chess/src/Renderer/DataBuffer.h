#pragma once

#include "Core/Base.h"

#include "Renderer/Texture.h"

#include <webgpu/webgpu_cpp.h>

#include <memory>
#include <vector>

namespace Chess {

	enum class DataBufferElementType
	{
		Buffer = 0, Texture, TextureStorage, Sampler
	};

	struct DataBufferElement
	{
		DataBufferElementType type;
		u64 size;
		wgpu::ShaderStage visibility;
		Ref<Texture> texture;
		wgpu::Sampler sampler;
		u64 offset;

		DataBufferElement(u64 size, wgpu::ShaderStage visibility)
			: type(DataBufferElementType::Buffer), size(size), visibility(visibility) {}
		DataBufferElement(wgpu::ShaderStage visibility, const Ref<Texture>& texture)
			: type(DataBufferElementType::Texture), size(sizeof(wgpu::Texture)), visibility(visibility), texture(texture) {}
		DataBufferElement(wgpu::ShaderStage visibility, wgpu::Sampler sampler)
			: type(DataBufferElementType::Sampler), size(sizeof(wgpu::Sampler)), visibility(visibility), sampler(sampler) {}

		DataBufferElement(DataBufferElementType type, wgpu::ShaderStage visibility, const Ref<Texture>& texture)
			: type(type), size(sizeof(wgpu::Texture)), visibility(visibility), texture(texture) {}
	};

	class DataBufferLayout
	{
	public:
		DataBufferLayout(std::initializer_list<DataBufferElement> elements)
			: m_Elements(elements)
		{
			u64 offset = 0;
			for (DataBufferElement& element: m_Elements)
			{
				element.offset = offset;
				offset += element.size;
			}
		}

		const std::vector<DataBufferElement>& GetElements() const { return m_Elements; }

		std::vector<DataBufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<DataBufferElement>::iterator end() { return m_Elements.end(); }
		std::vector<DataBufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<DataBufferElement>::const_iterator end() const { return m_Elements.end(); }

		const DataBufferElement& operator[](int index) const { return m_Elements[index]; }
		DataBufferElement& operator[](int index) { return m_Elements[index]; }
	private:
		std::vector<DataBufferElement> m_Elements;
	};

	class DataBuffer
	{
	public:
		DataBuffer() = default;
		DataBuffer(wgpu::Buffer buffer, wgpu::BufferBindingType bindingType)
			: m_RendererPtr(buffer), m_BindingType(bindingType) {}
		~DataBuffer();

		void SetData(const void* data, u64 size, u64 offset = 0);

		void Bind(wgpu::RenderPassEncoder renderPass, wgpu::BindGroup bindGruop, u32 groupIndex = 0) const;

		wgpu::Buffer GetRendererPtr() const { return m_RendererPtr; }
		u64 GetSize() const { return m_RendererPtr.GetSize(); }

		wgpu::BufferBindingType GetBindingType() const { return m_BindingType; }
	public:
		static Ref<DataBuffer> CreateUniformBufferFromData(const void* data, u64 size);
		static Ref<DataBuffer> CreateUniformBufferFromSize(u64 size);

		static Ref<DataBuffer> CreateStorageBufferFromData(const void* data, u64 size, bool readWrite = false);
		static Ref<DataBuffer> CreateStorageBufferFromSize(u64 size, bool readWrite = false);
	private:
		wgpu::Buffer m_RendererPtr = nullptr;
		wgpu::BufferBindingType m_BindingType = wgpu::BufferBindingType::Undefined;
	};

}
