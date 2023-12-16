#include "SceneGraph.h"

#include "Renderer/RenderData.h"

namespace Chess {

	SceneGraph::SceneGraph()
	{
		Ref<VertexBuffer> emptyVbo = VertexBuffer::CreateFromSize(1, sizeof(Vertex3D));
		Mesh emptyMesh = Mesh(emptyVbo);

		m_Root = new Node(emptyMesh);
		m_Root->parent = nullptr;

		m_Count = 1;
	}

	SceneGraph::~SceneGraph()
	{
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
		node->m_GlobalTransform = node->CalculateLocalTransform() * accumulatedTransform;
		for (Node* child : node->children)
		{
			UpdateInternal(child, node->m_GlobalTransform);
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
