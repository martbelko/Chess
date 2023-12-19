#pragma once

#include "Renderer/VertexBufferView.h"
#include "Renderer/Material.h"

namespace Chess {

	struct Mesh
	{
		VertexBufferView vboView;

		Mesh(const VertexBufferView& vboView)
			: vboView(vboView) {}
	};

}
