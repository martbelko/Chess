#pragma once

#include "Core/Base.h"

#include "Renderer/Shader.h"
#include "Renderer/ColorTarget.h"
#include "Renderer/RenderPipeline.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/DataBuffer.h"

#include <webgpu/webgpu.h>

#include <string>

namespace Chess {

	struct DepthStencilTextureDesc
	{
		bool depthWriteEnabled;
		wgpu::CompareFunction depthCompare;
		wgpu::StencilFaceState stencilFront;
		wgpu::StencilFaceState stencilBack;
		u32 stencilReadMask = 0xFFFFFFFF;
		u32 stencilWriteMask = 0xFFFFFFFF;
	};

	class RenderPipelineBuilder
	{
	public:
		RenderPipelineBuilder(const std::string& name = "Undefined") { m_Name = name; m_PipelineDesc.label = m_Name.c_str(); }

		void AddShader(const Ref<Shader>& shader, const std::string& mainVertName = "vs_main", const std::string& mainFragName = "fs_main");
		void AddColorTarget(const ColorTargetDesc& colorTarget) { m_ColorTargets.push_back(colorTarget); }
		void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer, const BufferLayout& layout);
		void AddDataBuffer(const Ref<DataBuffer>& dataBuffer, const DataBufferLayout& layout);
		void AddDepthStencilTexture(const Ref<Texture>& depthTexture, const DepthStencilTextureDesc& desc)
		{
			m_DepthStencilTexture = depthTexture;
			m_DepthStencilTextureDesc = desc;
		}

		RenderPipeline Build();
	private:
		std::string m_Name;

		wgpu::RenderPipelineDescriptor m_PipelineDesc;
		std::string m_MainVertName, m_MainFragName;

		wgpu::FragmentState m_FragmentState;
		std::vector<ColorTargetDesc> m_ColorTargets;

		Ref<Texture> m_DepthStencilTexture = nullptr;
		DepthStencilTextureDesc m_DepthStencilTextureDesc;

		std::vector<std::pair<Ref<VertexBuffer>, BufferLayout>> m_VertexBuffers;
		std::vector<std::pair<Ref<DataBuffer>, DataBufferLayout>> m_DataBuffers;
	};

}
