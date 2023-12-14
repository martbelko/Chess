#include "RenderPipelineBuilder.h"

#include "WebgpuLib.h"

#include "Core/Base.h"

#include <numeric>

namespace Chess {

	using namespace Base;

	void RenderPipelineBuilder::AddShader(const Ref<Shader>& shader, const std::string& mainVertName, const std::string& mainFragName)
	{
		m_MainVertName = mainVertName;
		m_MainFragName = mainFragName;

		m_PipelineDesc.vertex.module = shader->GetRendererPtr();
		m_PipelineDesc.vertex.entryPoint = m_MainVertName.c_str();
		m_PipelineDesc.vertex.constantCount = 0;
		m_PipelineDesc.vertex.constants = nullptr;

		m_FragmentState.module = shader->GetRendererPtr();
		m_FragmentState.entryPoint = m_MainFragName.c_str();
		m_FragmentState.constantCount = 0;
		m_FragmentState.constants = nullptr;
		m_PipelineDesc.fragment = &m_FragmentState;
	}

	void RenderPipelineBuilder::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer, const BufferLayout& layout)
	{
		m_VertexBuffers.emplace_back(vertexBuffer, layout);
	}

	RenderPipeline RenderPipelineBuilder::Build()
	{
		u32 shaderLocation = 0;

		std::vector<wgpu::VertexBufferLayout> vertexBuffLayouts;
		vertexBuffLayouts.reserve(m_VertexBuffers.size());
		// Sum up number of attributes for res., to avoid vector moving data and creating dangling ptrs
		std::vector<wgpu::VertexAttribute> attribs;
		attribs.reserve(
			std::accumulate(m_VertexBuffers.begin(), m_VertexBuffers.end(), static_cast<u32fast>(0),
				[](u32fast acc, const auto& vbo) { return acc + std::get<1>(vbo).GetElements().size(); })
		);

		for (const auto& [vbo, layout] : m_VertexBuffers)
		{
			for (size_t i = 0; i < layout.GetElements().size(); ++i)
			{
				const BufferElement& element = layout[i];
				attribs.push_back(
					wgpu::VertexAttribute{ .format = element.format, .offset = element.offset, .shaderLocation = shaderLocation++ }
				);
			}

			vertexBuffLayouts.push_back(wgpu::VertexBufferLayout{
				.arrayStride = layout.GetStride(),
				.stepMode = layout.GetStepMode(),
				.attributeCount = attribs.size(),
				.attributes = attribs.data()
			});
		}

		m_PipelineDesc.vertex.bufferCount = vertexBuffLayouts.size();
		m_PipelineDesc.vertex.buffers = vertexBuffLayouts.data();

		m_PipelineDesc.primitive.topology = wgpu::PrimitiveTopology::TriangleList;
		m_PipelineDesc.primitive.stripIndexFormat = wgpu::IndexFormat::Undefined;
		m_PipelineDesc.primitive.frontFace = wgpu::FrontFace::CCW;
		m_PipelineDesc.primitive.cullMode = wgpu::CullMode::None;

		std::vector<wgpu::ColorTargetState> colorTargetStates;
		if (!m_ColorTargets.empty())
		{
			for (const ColorTargetDesc& desc : m_ColorTargets)
			{
				wgpu::ColorTargetState colorTarget;
				colorTarget.format = desc.format;

				if (desc.enableBlending)
					colorTarget.blend = &desc.blendState;
				else
					colorTarget.blend = nullptr;

				colorTarget.writeMask = desc.writeMask;

				colorTargetStates.push_back(std::move(colorTarget));
			}

			m_FragmentState.targetCount = colorTargetStates.size();
			m_FragmentState.targets = colorTargetStates.data();
		}
		else
		{
			m_FragmentState.targetCount = 0;
			m_FragmentState.targets = nullptr;
		}

		wgpu::DepthStencilState depthStencilState;
		if (m_DepthStencilTexture)
		{
			depthStencilState.depthCompare = m_DepthStencilTextureDesc.depthCompare;
			depthStencilState.depthWriteEnabled = m_DepthStencilTextureDesc.depthWriteEnabled;
			depthStencilState.format = m_DepthStencilTexture->GetRendererPtr().GetFormat();
			depthStencilState.stencilReadMask = m_DepthStencilTextureDesc.stencilReadMask;
			depthStencilState.stencilWriteMask = m_DepthStencilTextureDesc.stencilWriteMask;
			depthStencilState.stencilFront = m_DepthStencilTextureDesc.stencilFront;
			depthStencilState.stencilBack = m_DepthStencilTextureDesc.stencilBack;

			m_PipelineDesc.depthStencil = &depthStencilState;
		}
		else
		{
			m_PipelineDesc.depthStencil = nullptr;
		}

		m_PipelineDesc.multisample.count = 1;
		m_PipelineDesc.multisample.mask = ~0u;
		m_PipelineDesc.multisample.alphaToCoverageEnabled = false;

		std::vector<wgpu::BindGroupLayout> bindGroupLayouts;
		bindGroupLayouts.reserve(m_DataBuffers.size());

		std::vector<std::pair<Ref<DataBuffer>, wgpu::BindGroup>> dataBuffers;
		dataBuffers.reserve(m_DataBuffers.size());

		for (u32 i = 0; i < m_DataBuffers.size(); ++i)
		{
			const auto& [buffer, layout] = m_DataBuffers[i];

			std::vector<wgpu::BindGroupLayoutEntry> bindGroupLayountEntries;
			std::vector<wgpu::BindGroupEntry> bindGroupEntries;
			bindGroupLayountEntries.reserve(layout.GetElements().size());
			bindGroupEntries.reserve(layout.GetElements().size());

			for (u32 j = 0; j < layout.GetElements().size(); ++j)
			{
				const DataBufferElement& element = layout[j];

				wgpu::BindGroupLayoutEntry entryLayout;

				// @binding in the shader
				entryLayout.binding = j;
				entryLayout.visibility = element.visibility;

				if (element.type == DataBufferElementType::Buffer)
				{
					entryLayout.buffer.type = buffer->GetBindingType();
					entryLayout.buffer.minBindingSize = element.size;
				}
				else if (element.type == DataBufferElementType::Texture)
				{
					auto getSampleType = [](wgpu::TextureFormat format) -> wgpu::TextureSampleType
					{
						switch (format)
						{
							case wgpu::TextureFormat::R32Float:
								return wgpu::TextureSampleType::UnfilterableFloat;
							case wgpu::TextureFormat::BGRA8Unorm:
								return wgpu::TextureSampleType::Float;

							case wgpu::TextureFormat::Stencil8:
							case wgpu::TextureFormat::Depth16Unorm:
							case wgpu::TextureFormat::Depth24Plus:
							case wgpu::TextureFormat::Depth24PlusStencil8:
							case wgpu::TextureFormat::Depth32Float:
							case wgpu::TextureFormat::Depth32FloatStencil8:
								return wgpu::TextureSampleType::Depth;
						}

						ERROR("Unknow texture format");
						return wgpu::TextureSampleType::Float;
					};

					entryLayout.texture.sampleType = getSampleType(element.texture->GetRendererPtr().GetFormat());
					entryLayout.texture.viewDimension = wgpu::TextureViewDimension::e2D;
				}
				else if (element.type == DataBufferElementType::Sampler)
				{
					entryLayout.sampler.type = wgpu::SamplerBindingType::Filtering;
				}
				else if (element.type == DataBufferElementType::TextureStorage)
				{
					entryLayout.storageTexture.access = wgpu::StorageTextureAccess::WriteOnly;
					entryLayout.storageTexture.format = element.texture->GetRendererPtr().GetFormat();
					entryLayout.storageTexture.viewDimension = wgpu::TextureViewDimension::e2D;
				}

				bindGroupLayountEntries.push_back(std::move(entryLayout));

				wgpu::BindGroupEntry bindGroupEntry;
				bindGroupEntry.binding = j;

				if (element.type == DataBufferElementType::Buffer)
				{
					bindGroupEntry.buffer = buffer->GetRendererPtr();
					bindGroupEntry.offset = element.offset;
					bindGroupEntry.size = element.size;
				}
				else if (element.type == DataBufferElementType::Texture)
				{
					wgpu::TextureViewDescriptor textureViewDesc;
					textureViewDesc.aspect = wgpu::TextureAspect::All;
					textureViewDesc.baseArrayLayer = 0;
					textureViewDesc.arrayLayerCount = 1;
					textureViewDesc.baseMipLevel = 0;
					textureViewDesc.mipLevelCount = 1;
					textureViewDesc.dimension = wgpu::TextureViewDimension::e2D;
					textureViewDesc.format = element.texture->GetRendererPtr().GetFormat();

					wgpu::TextureView textureView = element.texture->GetRendererPtr().CreateView(&textureViewDesc);
					bindGroupEntry.textureView = textureView;
				}
				else if (element.type == DataBufferElementType::Sampler)
				{
					bindGroupEntry.sampler = element.sampler;
				}
				else if (element.type == DataBufferElementType::TextureStorage)
				{
					wgpu::TextureViewDescriptor textureViewDesc;
					textureViewDesc.aspect = wgpu::TextureAspect::All;
					textureViewDesc.baseArrayLayer = 0;
					textureViewDesc.arrayLayerCount = 1;
					textureViewDesc.baseMipLevel = 0;
					textureViewDesc.mipLevelCount = 1;
					textureViewDesc.dimension = wgpu::TextureViewDimension::e2D;
					textureViewDesc.format = element.texture->GetRendererPtr().GetFormat();

					wgpu::TextureView textureView = element.texture->GetRendererPtr().CreateView(&textureViewDesc);
					bindGroupEntry.textureView = textureView;
				}

				bindGroupEntries.push_back(std::move(bindGroupEntry));
			}

			wgpu::BindGroupLayoutDescriptor bindGroupLayoutDesc;
			bindGroupLayoutDesc.entryCount = bindGroupLayountEntries.size();
			bindGroupLayoutDesc.entries = bindGroupLayountEntries.data();
			wgpu::BindGroupLayout bindGroupLayout = GraphicsContext::GetDevice().CreateBindGroupLayout(&bindGroupLayoutDesc);
			bindGroupLayouts.push_back(bindGroupLayout);

			// A bind group contains one or multiple bindings
			wgpu::BindGroupDescriptor bindGroupDesc;
			bindGroupDesc.layout = bindGroupLayout;
			// There must be as many bindings as declared in the layout!
			bindGroupDesc.entryCount = bindGroupEntries.size();
			bindGroupDesc.entries = bindGroupEntries.data();
			wgpu::BindGroup bindGroup = GraphicsContext::GetDevice().CreateBindGroup(&bindGroupDesc);

			dataBuffers.emplace_back(buffer, bindGroup);
		}

		// Create the pipeline layout
		wgpu::PipelineLayoutDescriptor layoutDesc;
		layoutDesc.bindGroupLayoutCount = bindGroupLayouts.size();
		layoutDesc.bindGroupLayouts = bindGroupLayouts.data();

		wgpu::PipelineLayout layout = GraphicsContext::GetDevice().CreatePipelineLayout(&layoutDesc);
		m_PipelineDesc.layout = layout;

		RenderPipeline pipeline = RenderPipeline(GraphicsContext::GetDevice().CreateRenderPipeline(&m_PipelineDesc));
		for (const auto& [vbo, layout] : m_VertexBuffers)
		{
			pipeline.AddVertexBuffer(vbo);
		}

		for (const auto& [dataBuffer, bindGroup] : dataBuffers)
		{
			pipeline.AddDataBuffer(dataBuffer, bindGroup);
		}

		GraphicsContext::GetDevice().Tick();
		return pipeline;
	}

	void RenderPipelineBuilder::AddDataBuffer(const Ref<DataBuffer>& dataBuffer, const DataBufferLayout& layout)
	{
		m_DataBuffers.emplace_back(dataBuffer, layout);
	}

}
