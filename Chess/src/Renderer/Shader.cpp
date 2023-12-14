#include "Shader.h"

#include "WebgpuLib.h"

#include <webgpu/webgpu_cpp.h>

namespace Chess {

	using namespace Base;

	std::shared_ptr<Shader> Shader::CreateFromFile(const std::string& shaderFilename)
	{
		std::optional<std::string> source = Filesystem::ReadFile(std::filesystem::path("shaders") / shaderFilename);
		// TODO: Check if source is valid

		wgpu::ShaderModuleWGSLDescriptor shaderCodeDesc;
		shaderCodeDesc.sType = wgpu::SType::ShaderModuleWGSLDescriptor;
		shaderCodeDesc.code = (*source).c_str();

		wgpu::ShaderModuleDescriptor shaderDesc;
		shaderDesc.nextInChain = &shaderCodeDesc;

		wgpu::ShaderModule shaderModule = GraphicsContext::GetDevice().CreateShaderModule(&shaderDesc);
		return CreateRef<Shader>(shaderModule);
	}

}
