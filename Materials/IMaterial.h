#pragma once

#include "glm/vec3.hpp"

class Ray;
struct RayCollisionData;

enum class MaterialType
{
	Lambertian,
	Emissive,
	Diffuse,
	Metal,
	FuzzyMetal
};

class IMaterial abstract
{
public:

	IMaterial() = delete;

	IMaterial(const glm::vec3 &albedo) :
		m_albedo(albedo)
	{
	}

	inline virtual MaterialType GetType() const = 0;

	virtual inline glm::vec3 GetAlbedo() const
	{
		return m_albedo;
	};
	virtual inline void SetAlbedo(const glm::vec3 &albedo)
	{
		m_albedo = albedo;
	}

	virtual glm::vec3 GetColourContribution(const RayCollisionData &rayCollisionData) = 0;
	virtual Ray GetNewRayDirection(const Ray& ray, const RayCollisionData &rayCollisionData) = 0;

	glm::vec3 m_albedo;
};