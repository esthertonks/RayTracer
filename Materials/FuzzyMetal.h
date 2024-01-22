#pragma once

#include "../Materials/IMaterial.h"
//#include "../Utils/Random.h"
#include "../RayTracing/Ray.h"

class FuzzyMetal : public IMaterial
{
public:

	FuzzyMetal(float roughness, const glm::vec3 &albedo) : IMaterial(albedo),
		m_roughness(roughness)
	{
	}

	FuzzyMetal() : IMaterial(glm::vec3(0.0f, 0.0f, 0.0f)),
		m_roughness(1.0f)
	{
	}

	inline virtual MaterialType GetType() const override
	{
		return MaterialType::FuzzyMetal;
	};

	inline float GetRoughness() {
		return m_roughness;
	};

	inline void SetRoughness(float roughness) {
		m_roughness = roughness;
	};

	inline virtual glm::vec3 GetColourContribution(const RayCollisionData& rayCollisionData) override
	{
		return m_albedo;
	}

	virtual Ray GetNewRayDirection(const Ray& ray, const RayCollisionData& rayCollisionData) override
	{
		glm::vec3 scatteredRayDirection = glm::reflect(ray.GetDirection(), rayCollisionData.worldNormal/* + roughness * Random::VectorInUnitSphere()*/);
		scatteredRayDirection += m_roughness * Random::RandomUnitVector();

		if (glm::all(glm::lessThan(glm::abs(scatteredRayDirection), glm::vec3(0.0001f))))
			scatteredRayDirection = rayCollisionData.worldNormal;

		glm::vec3 newRayOrigin = rayCollisionData.worldPosition + rayCollisionData.worldNormal * 0.0001f;
		return { newRayOrigin, scatteredRayDirection };

	}

private:
	float m_roughness = 1.0f;
};