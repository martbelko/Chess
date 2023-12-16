#pragma once

#include "Renderer/VertexBufferView.h"
#include "Renderer/Material.h"

namespace Chess {

	struct Mesh
	{
		VertexBufferView vboView;
		Material material;

		Mesh(VertexBufferView vboView, Material material)
			: vboView(vboView), material(material) {}
	};

}
