#include "RayEmitter.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

RayEmitter::RayEmitter() :
	m_forward(0.0f, 0.0f, -1.0f),
	m_position(0.0f, 0.0f, 6.0f),
	m_up(0.0f, 1.0f, 0.0f),

	m_farClippingPlane(100.0f),
	m_fieldOfView(45.0f),//Vertical FOV
	m_nearClippingPlane(0.1f),

	m_inverseProjection(1.0f),
	m_inverseView(1.0f),
	m_projection(1.0f),
	m_view(1.0f),

	m_processing(false)
{
}

RayEmitter::~RayEmitter()
{
}

bool RayEmitter::Initialise(glm::vec2 dimensions)
{
	if (m_processing)
		return false;

	CalculateProjection(dimensions);
	CalculateRayDirectionCache(dimensions);

	return true;
}

void RayEmitter::Resize(glm::vec2 dimensions)
{
	assert(!m_processing);

	CalculateProjection(dimensions);
	CalculateRayDirectionCache(dimensions);
}

void RayEmitter::Rotate(glm::vec2 delta)
{
	if (m_processing)
		return;

	float pitchDelta = delta.y * GetRotationSpeed();
	float yawDelta = delta.x * GetRotationSpeed();

	glm::vec3 right = glm::cross(m_forward, m_up);

	glm::quat q = glm::normalize(glm::cross(glm::angleAxis(-pitchDelta, right), glm::angleAxis(-yawDelta, m_up)));
	m_forward = glm::rotate(q, m_forward);

	CalculateView();
	RecalculateRayDirectionCache();
}

void RayEmitter::MoveLeft(float deltaTime)
{
	if (m_processing)
		return;

	glm::vec3 right = glm::cross(m_forward, m_up);
	m_position -= right * GetMoveSpeed() * deltaTime;

	CalculateView();
	RecalculateRayDirectionCache();
}

void RayEmitter::MoveRight(float deltaTime)
{
	if (m_processing)
		return;

	glm::vec3 right = glm::cross(m_forward, m_up);
	m_position += right * GetMoveSpeed() * deltaTime;

	CalculateView();
	RecalculateRayDirectionCache();
}
void RayEmitter::MoveForward(float deltaTime)
{
	if (m_processing)
		return;

	m_position += m_forward * GetMoveSpeed() * deltaTime;

	CalculateView();
	RecalculateRayDirectionCache();
}

void RayEmitter::MoveBack(float deltaTime)
{
	if (m_processing)
		return;

	m_position -= m_forward * GetMoveSpeed() * deltaTime;

	CalculateView();
	RecalculateRayDirectionCache();
}

void RayEmitter::MoveUp(float deltaTime)
{
	if (m_processing)
		return;

	m_position += m_up * GetMoveSpeed() * deltaTime;

	CalculateView();
	RecalculateRayDirectionCache();
}

void RayEmitter::MoveDown(float deltaTime)
{
	if (m_processing)
		return;

	m_position -= m_up * GetMoveSpeed() * deltaTime;

	CalculateView();
	RecalculateRayDirectionCache();
}


void RayEmitter::CalculateProjection(glm::vec2 dimesnions)
{
	m_projection = glm::perspectiveFov(glm::radians(m_fieldOfView), dimesnions.x, dimesnions.y, m_nearClippingPlane, m_farClippingPlane);
	m_inverseProjection = glm::inverse(m_projection);
}

void RayEmitter::CalculateView()
{
	m_view = glm::lookAt(m_position, m_position + m_forward, m_up);
	m_inverseView = glm::inverse(m_view);
}

glm::vec3 RayEmitter::GetRayDirection(uint32_t x, uint32_t y) const
{
	assert(m_cache.m_rayDirections.size() > 0);
	assert(!m_processing);
	return m_cache.m_rayDirections[x + y * (size_t)m_cache.m_screenDimensions.x];
}

glm::vec3 RayEmitter::GetPosition() const
{
	return m_position;
}

void RayEmitter::CalculateRayDirectionCache(glm::vec2 dimesnions)
{
	m_cache.Resize(dimesnions);
	RecalculateRayDirectionCache();
}

void RayEmitter::RecalculateRayDirectionCache()
{
	m_processing = true;

	for (int y = 0; y < m_cache.m_screenDimensions.y; y++) {
		for (int x = 0; x < m_cache.m_screenDimensions.x; x++) {
			glm::vec2 coord = { (float)x / m_cache.m_screenDimensions.x,(float)y / (float)m_cache.m_screenDimensions.y };
			// Remap texture coords from 0 - 1 to -1 to 1, therefore putting 0 in the screen centre.
			coord = coord * 2.0f - 1.0f;

			// Calc view space before projection from screen coords.
			glm::vec4 viewCoord = m_inverseProjection * glm::vec4(coord.x, coord.y, 1, 1);
			// Calc the world space form the view coords.
			glm::vec3 rayDirection = glm::vec3(m_inverseView * glm::vec4(glm::normalize(glm::vec3(viewCoord) / viewCoord.w), 0));

			m_cache.m_rayDirections[x + y * (int)m_cache.m_screenDimensions.x] = rayDirection;
		}
	}

	m_processing = false;

}
