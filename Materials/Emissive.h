#pragma once

#include "../Materials/IMaterial.h"
#include "../Utils/Random.h"
#include "../RayTracing/Ray.h"

class Emissive : public IMaterial
{
public:

	Emissive(float emissionPower, const glm::vec3& albedo) : IMaterial(albedo),
		m_emissionPower(emissionPower)
	{
	}

	inline virtual MaterialType GetType() const override
	{
		return MaterialType::Emissive;
	};

	glm::vec3 GetEmission() const { return m_albedo * m_emissionPower; }
	glm::vec3 GetEmissionColour() const { return m_albedo; }
	void SetEmissionColour(glm::vec3& emissionColour) { m_albedo = emissionColour; }
	float GetEmissionPower() const { return m_emissionPower; }
	void SetEmissionPower(float emissionPower) { m_emissionPower = emissionPower; }

	virtual glm::vec3 GetColourContribution(const RayCollisionData& rayCollisionData) override
	{
		return GetEmission();
	}

	virtual Ray GetNewRayDirection(const Ray& ray, const RayCollisionData& rayCollisionData) override
	{
		// Put the new origin at the hit location, but jiggle a bit so we don't collide with ourself
		glm::vec3 newRayOrigin = rayCollisionData.worldPosition + rayCollisionData.worldNormal * 0.0001f;
		glm::vec3 scatteredRayDirection = glm::normalize(rayCollisionData.worldNormal * Random::VectorInUnitSphere());

		return { newRayOrigin, scatteredRayDirection };
	}

private:
	float m_emissionPower = 0.0f;
};

