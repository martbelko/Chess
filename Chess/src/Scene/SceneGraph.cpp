#include "SceneGraph.h"

#include "Renderer/RenderData.h"
#include "Renderer/Material.h"

#include "Scene/Node.h"
#include "Scene/Entity.h"

namespace Chess {

	SceneGraph::SceneGraph(const Entity& rootEntity)
	{
		m_Root = new Node(rootEntity);
		m_Root->parent = nullptr;

		m_Count = 1;
	}

	SceneGraph::~SceneGraph()
	{
		bool x = m_Root == nullptr;
		Delete(m_Root);
	}

	void SceneGraph::AddNode(Node* node, Node* parent) const
	{
		node->parent = parent;
		parent->children.push_back(node);
	}

	void SceneGraph::Update() const
	{
		UpdateInternal(m_Root, glm::mat4(1.0f));
	}

	void SceneGraph::UpdateInternal(Node* node, const glm::mat4& accumulatedTransform) const
	{
		Entity entity = node->entity;
		if (entity.HasComponent<TransformComponent>())
		{
			TransformComponent& tc = entity.GetComponent<TransformComponent>();
			tc.worldMatrix = tc.CalculateLocalTransform() * accumulatedTransform;

			for (Node* child : node->children)
			{
				UpdateInternal(child, tc.worldMatrix);
			}
		}
		else
		{
			for (Node* child : node->children)
			{
				UpdateInternal(child, accumulatedTransform);
			}
		}
	}

	void SceneGraph::Delete(Node* node)
	{
		for (Node* n : node->children)
		{
			Delete(n);
		}

		delete node;
	}

}
