#pragma once

#include "SceneGraph.h"

namespace Chess {

	class Scene
	{
	public:
		const SceneGraph& GetSceneGraph() const { return m_SceneGraph; }
	private:
		SceneGraph m_SceneGraph;
	};

}
