#pragma once

#include "../Materials/IMaterial.h"
#include "../RayTracing/Ray.h"

class Metal : public IMaterial
{
public:

	Metal(const glm::vec3& albedo) : IMaterial(albedo)
	{
	}

	inline virtual MaterialType GetType() const override
	 {
		 return MaterialType::Metal;
	 };

	 virtual glm::vec3 GetColourContribution(const RayCollisionData& rayCollisionData) override
	 {
		 return m_albedo;
	 }

	virtual Ray GetNewRayDirection(const Ray& ray, const RayCollisionData& rayCollisionData) override
	{
		glm::vec3 scatteredRayDirection = glm::reflect(ray.GetDirection(), rayCollisionData.worldNormal);

		if (glm::all(glm::lessThan(glm::abs(scatteredRayDirection), glm::vec3(0.0001f))))
			scatteredRayDirection = rayCollisionData.worldNormal;

		glm::vec3 newRayOrigin = rayCollisionData.worldPosition + rayCollisionData.worldNormal * 0.0001f;
		return { newRayOrigin, scatteredRayDirection };

	}

private:

};