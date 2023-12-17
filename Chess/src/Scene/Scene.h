#pragma once

#include "WebgpuLib.h"

#include "Core/UUID.h"

#include "Scene/SceneGraph.h"
#include "Scene/Components.h"

#include <entt/entt.hpp>

#include <unordered_map>

namespace Chess {

	struct Node;

	class Scene
	{
	public:
		Scene();

		Node* AddNode(Node* parent, const std::string& name = "");
		Node* GetRoot() const { return m_SceneGraph->GetRoot(); }

		void Update() { m_SceneGraph->Update(); }

		Node* GetNodeByName(const std::string& name) { ASSERT(m_EntityMapName.contains(name)); return m_EntityMapName[name]; }

		template<typename... Components>
		std::vector<Node*> CreateView()
		{
			auto view = m_Registry.view<Components...>();
			std::vector<Node*> result;

			if constexpr (sizeof...(Components) == 1)
			{
				result.reserve(view.size());
			}
			else
			{
				result.reserve(view.size_hint());
			}

			for (entt::entity ent : view)
			{
				Node* node = m_EntityMapId[static_cast<u32>(ent)];
				result.push_back(node);
			}

			return result;
		}
	private:
		Scope<SceneGraph> m_SceneGraph;
		entt::registry m_Registry;

		std::unordered_map<u32, Node*> m_EntityMapId;
		std::unordered_map<std::string, Node*> m_EntityMapName;
	private:
		friend class Entity;
	};

}
