#pragma once

#include <webgpu/webgpu_cpp.h>

namespace Chess {

	struct ColorTargetDesc
	{
		wgpu::TextureFormat format = wgpu::TextureFormat::BGRA8Unorm;
		wgpu::ColorWriteMask writeMask = wgpu::ColorWriteMask::All;
		wgpu::BlendState blendState;
		bool enableBlending = true;

		ColorTargetDesc()
		{
			blendState.color.srcFactor = wgpu::BlendFactor::SrcAlpha;
			blendState.color.dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha;
			blendState.color.operation = wgpu::BlendOperation::Add;
			blendState.alpha.srcFactor = wgpu::BlendFactor::Zero;
			blendState.alpha.dstFactor = wgpu::BlendFactor::One;
			blendState.alpha.operation = wgpu::BlendOperation::Add;
		}
	};

}
