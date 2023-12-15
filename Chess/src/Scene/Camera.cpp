#include "Camera.h"

#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

namespace Chess {

	const float PerspectiveCamera::m_MinElevation = -1.5f;
	const float PerspectiveCamera::m_MaxElevation = 1.5f;
	const float PerspectiveCamera::m_MinDistance = 1.0f;
	const float PerspectiveCamera::m_AngleSensitivity = 0.008f;
	const float PerspectiveCamera::m_ZoomSensitivity = 0.003f;

	PerspectiveCamera::PerspectiveCamera(float fovy, float aspect, float near, float far)
		: m_AngleDirection(0.0f), m_AngleElevation(0.0f), m_Distance(10.0f), m_IsRotating(false), m_IsZooming(false)
	{
		m_ProjectionMatrix = glm::perspective(fovy, aspect, near, far);
		UpdateEyePos();
	}

	void PerspectiveCamera::SetProjection(float fovy, float aspect, float near, float far)
	{
		m_ProjectionMatrix = glm::perspective(fovy, aspect, near, far);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void PerspectiveCamera::RecalculateViewMatrix()
	{
		m_ViewMatrix = glm::lookAt(m_EyePosition, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
	}

	void PerspectiveCamera::OnMouseMove(const glm::vec2& delta)
	{
		float dx = delta.x;
		float dy = delta.y;

		if (m_IsRotating)
		{
			m_AngleDirection += dx * m_AngleSensitivity;
			m_AngleElevation += dy * m_AngleSensitivity;

			m_AngleElevation = glm::clamp(m_AngleElevation, m_MinElevation, m_MaxElevation);
		}

		if (m_IsZooming)
		{
			m_Distance *= (1.0f + dy * m_ZoomSensitivity);

			if (m_Distance < m_MinDistance)
			{
				m_Distance = m_MinDistance;
			}
		}

		UpdateEyePos();
	}

	void PerspectiveCamera::OnMouseButtonPressed(const Base::MousePressedEvent& ev)
	{
		if (ev.button == GLFW_MOUSE_BUTTON_MIDDLE)
		{
			m_IsRotating = true;
		}
		else if (ev.button == GLFW_MOUSE_BUTTON_LEFT)
		{
			m_IsZooming = true;
		}
	}

	void PerspectiveCamera::OnMouseButtonReleased(const Base::MouseReleasedEvent& ev)
	{
		if (ev.button == GLFW_MOUSE_BUTTON_MIDDLE)
		{
			m_IsRotating = false;
		}
		else if (ev.button == GLFW_MOUSE_BUTTON_LEFT)
		{
			m_IsZooming = false;
		}
	}

	void PerspectiveCamera::UpdateEyePos()
	{
		m_EyePosition.x = m_Distance * glm::cos(m_AngleElevation) * -glm::sin(m_AngleDirection);
		m_EyePosition.y = m_Distance * glm::sin(m_AngleElevation);
		m_EyePosition.z = m_Distance * glm::cos(m_AngleElevation) * glm::cos(m_AngleDirection);

		RecalculateViewMatrix();
	}


}
