#pragma once

#include "Core/Base.h"

#include <glm/glm.hpp>

#include <vector>

namespace Chess {

	struct Node;
	class Entity;

	class SceneGraph
	{
	public:
		SceneGraph() = default;
		SceneGraph(const Entity& rootEntity);
		~SceneGraph();

		Node* GetRoot() const { return m_Root; }

		void AddNode(Node* node, Node* parent) const;

		void Update() const;
	private:
		void UpdateInternal(Node* node, const glm::mat4& accumulatedTransform) const;
		void Delete(Node* node);
	private:
		Node* m_Root = nullptr;
		u64 m_Count = 0;
	};

}