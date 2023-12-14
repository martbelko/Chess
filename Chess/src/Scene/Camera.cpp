#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Chess {

	PerspectiveCamera::PerspectiveCamera(float fovy, float aspect, float near, float far)
	{
		RecalculateViewMatrix();
	}

	void PerspectiveCamera::SetProjection(float fovy, float aspect, float near, float far)
	{
		m_ProjectionMatrix = glm::perspective(fovy, aspect, near, far);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	glm::quat PerspectiveCamera::GetOrientation() const
	{
		return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
	}

	glm::vec3 PerspectiveCamera::GetRightDirection() const
	{
		return GetOrientation() * WORLD_RIGHT * glm::conjugate(GetOrientation());
	}

	glm::vec3 PerspectiveCamera::GetForwardDirection() const
	{
		return GetOrientation() * WORLD_FORWARD * glm::conjugate(GetOrientation());
	}

	glm::vec3 PerspectiveCamera::GetUpDirection() const
	{
		return GetOrientation() * WORLD_UP * glm::conjugate(GetOrientation());
	}

	void PerspectiveCamera::OnMouseMove(const glm::vec2& delta)
	{
		constexpr float sens = 0.005f;

		m_Yaw += delta.x * sens;
		m_Pitch += delta.y * sens;

		RecalculateViewMatrix();
	}

	void PerspectiveCamera::RecalculateViewMatrix()
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(GetOrientation());
		m_ViewMatrix = glm::inverse(transform);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}


}
