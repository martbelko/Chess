#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Chess {

	class PerspectiveCamera
	{
	public:
		PerspectiveCamera(float fovy = 1.0f, float aspect = 1.0f, float near = 0.1f, float far = 1000.0f);

		void SetProjection(float fovy, float aspect, float near = 0.1f, float far = 1000.0f);

		const glm::vec3& GetPosition() const { return m_Position; }
		void SetPosition(const glm::vec3& position) { m_Position = position; RecalculateViewMatrix(); }

		glm::quat GetOrientation() const;

		glm::vec3 GetRightDirection() const;
		glm::vec3 GetForwardDirection() const;
		glm::vec3 GetUpDirection() const;

		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

		void OnMouseMove(const glm::vec2& delta);
	private:
		void RecalculateViewMatrix();
	private:
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ViewProjectionMatrix;

		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };

		float m_Pitch = 0.0f, m_Yaw = 0.0f;
	private:
		static inline const glm::vec3 WORLD_RIGHT = glm::vec3(1, 0, 0);
		static inline const glm::vec3 WORLD_FORWARD = glm::vec3(0, 0, -1);
		static inline const glm::vec3 WORLD_UP = glm::vec3(0, 1, 0);
	};


}
