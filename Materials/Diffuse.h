#pragma once

#include "../Materials/IMaterial.h"
#include "../RayTracing/Ray.h"
#include "../Utils/Random.h"

class Diffuse : public IMaterial
{
public:

	Diffuse(const glm::vec3& lightDirection, float roughness, const glm::vec3& albedo) : IMaterial(albedo),
		m_lightDirection(glm::vec3(glm::normalize(lightDirection))),
		m_roughness(roughness)
	{
	}

	Diffuse(const glm::vec3& albedo) : IMaterial(albedo),
		m_lightDirection(glm::vec3(glm::normalize(glm::vec3(-1, -1, -1)))),
		m_roughness(1.0f)
	{
	}

	virtual MaterialType GetType() const override
	{
		return MaterialType::Diffuse;
	};

	inline glm::vec3 GetAlbedo() {
		return m_albedo;
	};

	inline void SetAlbedo(glm::vec3 &albedo) {
		m_albedo = albedo;
	};

	inline float GetRoughness() {
		return m_roughness;
	};

	inline void SetRoughness(float roughness) {
		m_roughness = roughness;
	};

	inline void UpdateLightDirection(glm::vec3& lightDirection) {
		m_lightDirection = glm::normalize(lightDirection);
	};

	virtual glm::vec3 GetColourContribution(const RayCollisionData& rayCollisionData) override
	{
		glm::vec3 sphereColour = m_albedo;
		float lightIntensity = glm::max(glm::dot(rayCollisionData.worldNormal, -m_lightDirection), 0.0f);
		sphereColour *= lightIntensity;
		return sphereColour;
	}

	virtual Ray GetNewRayDirection(const Ray& ray, const RayCollisionData& rayCollisionData) override
	{
		// Put the new origin at the hit location, but jiggle a bit so we don't collide with ourself
		glm::vec3 newRayOrigin = rayCollisionData.worldPosition + rayCollisionData.worldNormal * 0.0001f;
		glm::vec3 scatteredRayDirection = glm::reflect(ray.GetDirection(), rayCollisionData.worldNormal + m_roughness * Random::VectorInUnitSphere()/*Random::Vec3(-0.5f, 0.5f)*/);

		return { newRayOrigin, scatteredRayDirection };

	}

private:
	glm::vec3 m_lightDirection;
	float m_roughness;
};