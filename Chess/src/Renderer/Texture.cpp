#include "Texture.h"

#include "WebgpuLib.h"

namespace Chess {

	using namespace Base;

	Texture::~Texture()
	{
		m_RendererPtr.Destroy();
	}

	Ref<Texture> Texture::CreateFromSize(u32 width, u32 height, wgpu::TextureFormat format, wgpu::TextureUsage usage)
	{
		wgpu::TextureDescriptor textureDesc;
		textureDesc.dimension = wgpu::TextureDimension::e2D;
		textureDesc.format = format;
		textureDesc.size = { width, height, 1 };
		textureDesc.usage = usage;
		textureDesc.viewFormats = nullptr;

		wgpu::Texture texture = GraphicsContext::GetDevice().CreateTexture(&textureDesc);
		return CreateRef<Texture>(texture);
	}

	Ref<Texture> Texture::CreateDepthStencilTexture(u32 width, u32 height, wgpu::TextureFormat format)
	{
		wgpu::TextureDescriptor textureDesc;
		textureDesc.dimension = wgpu::TextureDimension::e2D;
		textureDesc.format = format;
		textureDesc.mipLevelCount = 1;
		textureDesc.sampleCount = 1;
		textureDesc.size = { width, height, 1 };
		textureDesc.usage = wgpu::TextureUsage::RenderAttachment;
		textureDesc.viewFormatCount = 1;
		textureDesc.viewFormats = &format;

		wgpu::Texture texture = GraphicsContext::GetDevice().CreateTexture(&textureDesc);
		return CreateRef<Texture>(texture);
	}

}
