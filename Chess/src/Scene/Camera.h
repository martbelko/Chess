#pragma once

#include "WebgpuLib.h"

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Chess {

	class PerspectiveCamera
	{
	public:
		PerspectiveCamera(float fovy = 1.0f, float aspect = 1.0f, float near = 0.1f, float far = 1000.0f);

		void SetProjection(float fovy, float aspect, float near = 0.1f, float far = 1000.0f);

		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

		void OnMouseMove(const glm::vec2& delta);
		void OnMouseButtonPressed(const Base::MousePressedEvent& ev);
		void OnMouseButtonReleased(const Base::MouseReleasedEvent& ev);
	private:
		void UpdateEyePos();
		void RecalculateViewMatrix();
	private:
		static const float m_MinElevation;
		static const float m_MaxElevation;
		static const float m_MinDistance;
		static const float m_AngleSensitivity;
		static const float m_ZoomSensitivity;

		float m_AngleDirection;
		float m_AngleElevation;
		float m_Distance;
		glm::vec3 m_EyePosition;
		bool m_IsRotating, m_IsZooming;

		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewProjectionMatrix;
	private:
		static inline const glm::vec3 WORLD_RIGHT = glm::vec3(1, 0, 0);
		static inline const glm::vec3 WORLD_FORWARD = glm::vec3(0, 0, -1);
		static inline const glm::vec3 WORLD_UP = glm::vec3(0, 1, 0);
	};


}
