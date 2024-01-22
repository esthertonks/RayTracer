#pragma once

#include <glm/glm.hpp>
#include <vector>

class RayEmitter
{
public:

	RayEmitter();
	~RayEmitter();

	bool Initialise(glm::vec2 dimensions);
	void Resize(glm::vec2 dimensions);

	inline bool Ready() const { 
		return m_cache.m_rayDirections.size() != 0; 
	}

	glm::vec3 GetRayDirection(uint32_t x, uint32_t y) const;
	glm::vec3 GetPosition() const;

	void Rotate(glm::vec2 delta);

	void MoveLeft(float deltaTime);
	void MoveRight(float deltaTime);

	void MoveForward(float deltaTime);
	void MoveBack(float deltaTime);

	void MoveUp(float deltaTime);
	void MoveDown(float deltaTime);

private:

	class RayDirectionCache
	{
	public:
		friend class RayEmitter;

		void Resize(glm::vec2 newScreenDimensions)
		{
			m_screenDimensions = newScreenDimensions;
			m_rayDirections.resize((int)newScreenDimensions.x * (int)newScreenDimensions.y);
		}

	private:
		// Cached ray directions - necessary for running in software.
		std::vector<glm::vec3> m_rayDirections;
		glm::vec2 m_screenDimensions;
	};

	void CalculateProjection(glm::vec2 dimesnions);
	void CalculateView();

	void CalculateRayDirectionCache(glm::vec2 dimesnions);
	void RecalculateRayDirectionCache();

	inline const float GetRotationSpeed() const
	{
		return 0.3f;
	}

	inline const float GetMoveSpeed() const
	{
		return 1.0f;
	}

	glm::vec3 m_forward;
	glm::vec3 m_position;
	const glm::vec3 m_up;

	float m_fieldOfView = 45.0f;//Vertical
	float m_nearClippingPlane = 0.1f;
	float m_farClippingPlane = 100.0f;

	glm::mat4 m_inverseProjection{ 1.0f };
	glm::mat4 m_inverseView{ 1.0f };
	glm::mat4 m_projection{ 1.0f };
	glm::mat4 m_view{ 1.0f };

	bool m_processing;

	RayDirectionCache m_cache;
};

