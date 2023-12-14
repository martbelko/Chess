#pragma once

#include "Core/Base.h"

#include <webgpu/webgpu_cpp.h>

#include <string>

namespace Chess {

	class Shader
	{
	public:
		Shader(wgpu::ShaderModule shaderModule = nullptr)
			: m_RendererPtr(shaderModule) {}

		Shader(const Shader&) = delete;
		Shader(Shader&& other) noexcept = delete;
		Shader& operator=(const Shader&) = delete;
		Shader& operator=(Shader&& other) = delete;

		wgpu::ShaderModule GetRendererPtr() const { return m_RendererPtr; }
	public:
		static Ref<Shader> CreateFromFile(const std::string& shaderFilename);
	private:
		wgpu::ShaderModule m_RendererPtr;
	};



}
