#include "Scene.h"

#include "Scene/Node.h"
#include "Scene/Entity.h"

namespace Chess {

	Scene::Scene()
	{
		Entity rootEntity = Entity(m_Registry.create(), this);
		m_SceneGraph = CreateScope<SceneGraph>(rootEntity);
	}

	Node* Scene::AddNode(Node* parent, const std::string& name)
	{
		ASSERT(parent != nullptr, "Parent was nullptr");

		UUID id = UUID();

		Entity entity = Entity(m_Registry.create(), this);
		entity.AddComponent<IDComponent>(id);
		entity.AddComponent<TransformComponent>();
		TagComponent& tagComponent = entity.AddComponent<TagComponent>();
		tagComponent.tag = name.empty() ? "Entity" + std::to_string(id) : name;

		Node* node = new Node(entity);

		m_SceneGraph->AddNode(node, parent);

		m_EntityMapId[entity.GetId()] = node;
		m_EntityMapName[tagComponent.tag] = node;

		return node;
	}

}
