#pragma once

#include "Core/Base.h"

#include <webgpu/webgpu_cpp.h>

namespace Chess {

	class Texture
	{
	public:
		Texture(wgpu::Texture texture)
			: m_RendererPtr(texture) {}
		~Texture();

		wgpu::Texture GetRendererPtr() const { return m_RendererPtr; }
	public:
		static Ref<Texture> CreateFromSize(u32 width, u32 height, wgpu::TextureFormat format = wgpu::TextureFormat::BGRA8Unorm, wgpu::TextureUsage usage = wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::TextureBinding);
		static Ref<Texture> CreateDepthStencilTexture(u32 width, u32 height, wgpu::TextureFormat format = wgpu::TextureFormat::Depth24PlusStencil8);
	private:
		wgpu::Texture m_RendererPtr;

	};

}
