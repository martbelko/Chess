#pragma once

#include <WebgpuLib.h>

#include "Core/Base.h"

#include "Renderer/Material.h"

#include <unordered_map>

namespace Chess {

	class MaterialSystem
	{
	public:
		void RegisterMaterial(const std::string& name, const Ref<Material>& material);

		Ref<Material> GetMaterialByName(const std::string& name) { ASSERT(m_Materials.contains(name), "Unknown material name"); return m_Materials[name]; }
	private:
		std::unordered_map<std::string, Ref<Material>> m_Materials;
	};

}
