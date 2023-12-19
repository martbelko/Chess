#include "MaterialSystem.h"

namespace Chess {

	void MaterialSystem::RegisterMaterial(const std::string& name, const Ref<Material>& material)
	{
		ASSERT(!m_Materials.contains(name), "The name is already register as material");
		m_Materials[name] = material;
	}

}
