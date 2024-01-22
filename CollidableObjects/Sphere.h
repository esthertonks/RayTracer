#pragma once

#include "CollidableObject.h"

class Sphere : public CollidableObject
{
public:

	Sphere(const glm::vec3& position, float radius, int materialIndex)
		: CollidableObject(position, radius, materialIndex)
	{
	}

	float Intersect(const Ray& ray, const World& world) const override;

private:
};