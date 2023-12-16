#pragma once

#include "Scene/Node.h"

#include <vector>

namespace Chess {

	class SceneGraph
	{
	public:
		SceneGraph();
		~SceneGraph();

		Node* GetRoot() const { return m_Root; }

		void AddNode(Node* node, Node* parent) const;

		void Update() const;
	private:
		void UpdateInternal(Node* node, const glm::mat4& accumulatedTransform) const;
		void Delete(Node* node);
	private:
		Node* m_Root;
		u64 m_Count;
	};

}