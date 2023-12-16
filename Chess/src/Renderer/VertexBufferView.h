#pragma once

#include "Renderer/VertexBuffer.h"

namespace Chess {

	struct VertexBufferView
	{
		Ref<VertexBuffer> vbo;
		u64 from, to;

		VertexBufferView(const Ref<VertexBuffer> vbo)
			: vbo(vbo), from(0), to(vbo->GetVertexCount()) {}
		VertexBufferView(const Ref<VertexBuffer> vbo, u64 fromIndex, u64 toIndex)
			: vbo(vbo), from(fromIndex), to(toIndex) {}
	};

}