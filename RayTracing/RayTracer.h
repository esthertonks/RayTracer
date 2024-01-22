#pragma once

#include <glm/glm.hpp>

class Ray;
class RayEmitter;
class World;

struct RayCollisionData;

class RayTracer
{
public:

	RayTracer();
	~RayTracer();

	void Initialise();

	glm::vec3 CalculatePixelColour(uint32_t x, uint32_t y, int numBounces, const World& world, const Ray& ray) const;

private:

	RayCollisionData FillCollisionDataOnHit(const Ray& ray, const World& world, float closestCollisionDistance, int objectIndex) const;
	RayCollisionData TraceRay(const Ray& ray, const World& world) const;
	RayCollisionData ClosestHit(const Ray& ray, const World& world, float closestCollisionDistance, int objectIndex) const;
	RayCollisionData FillCollisionDataOnMiss(const Ray& ray) const;

};

