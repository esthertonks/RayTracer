#include "RayTracer.h"

#include "glm/gtx/scalar_relational.hpp"

#include "Ray.h"
#include "../CollidableObjects/CollidableObject.h"
#include "../Materials/IMaterial.h"
#include "../Utils/Utils.h"
#include "../World.h"

RayTracer::RayTracer()
{

}

RayTracer::~RayTracer()
{

}

void RayTracer::Initialise()
{
}


RayCollisionData RayTracer::TraceRay(const Ray& ray, const World& world) const
{
	if (world.GetCollidableObjects().size() == 0)
		return FillCollisionDataOnMiss(ray);

	const std::vector<CollidableObject*>& objects = world.GetCollidableObjects();
	float collisionDistance;
	float closestCollisionDistance = std::numeric_limits<float>::max();
	int closestSphereIndex = -1;
	for (size_t i = 0; i < objects.size(); i++)
	{
		// Effectively this puts the sphere at zero in relation to the ray.
		collisionDistance = objects[i]->Intersect(ray, world);

		if (glm::greaterThanEqual(collisionDistance, 0.0f))
		{
			if (collisionDistance > 0 && collisionDistance < closestCollisionDistance)
			{
				closestCollisionDistance = collisionDistance;
				closestSphereIndex = (int)i;
			}
		}
	}

	if (closestSphereIndex == -1)
		return FillCollisionDataOnMiss(ray);

	return FillCollisionDataOnHit(ray, world, closestCollisionDistance, closestSphereIndex);

}

glm::vec3 RayTracer::CalculatePixelColour(uint32_t x, uint32_t y, int numBounces, const World& world, const Ray& ray) const
{
	constexpr float attenuation = 0.9f;
	constexpr glm::vec3 colourA(1.0f);
	glm::vec3 colourB(0.0f, 0.0f, 0.0f);
	//TODO pass into RayTracer from Application
	constexpr glm::vec3 backgroundColour(0.5f, 0.7f, 1.0f);
	//constexpr glm::vec3 backgroundColour(0.0f, 0.0f, 0.0f);

	Ray currentRay(ray);

	int bounce = 0;
	for (bounce; bounce < numBounces; bounce++)
	{
		RayCollisionData rayCollisionData = TraceRay(currentRay, world);
		// Move slightly above zero see: Listing 49 https://raytracing.github.io/books/RayTracingInOneWeekend.html
		if (rayCollisionData.collisionDistance < 0.0001f)
		{
			if (bounce == 0) // If we didn't hit anything at all
				return Utils::Lerp(colourA, backgroundColour, currentRay.GetDirection());
			else
				return Utils::Lerp(colourA, colourB, currentRay.GetDirection()) * (float)glm::pow(attenuation, bounce);
		}
		int materialIndex = world.GetCollidableObject(rayCollisionData.objectIndex).GetMaterialIndex();
		IMaterial* material = world.GetMaterialPtr(materialIndex);
		colourB += material->GetColourContribution(rayCollisionData);
		currentRay = material->GetNewRayDirection(currentRay, rayCollisionData);
	}

	// We stopped before we finished hitting so we don't know the colour.
	return glm::vec3(0.0f);
}

RayCollisionData RayTracer::ClosestHit(const Ray& ray, const World& world, float closestCollisionDistance, 
	int objectIndex) const
{
	RayCollisionData rayCollisionData;
	rayCollisionData.objectIndex = objectIndex;
	rayCollisionData.collisionDistance = closestCollisionDistance;

	const CollidableObject& closestObject = world.GetCollidableObject(objectIndex);
	const glm::vec3 origin = ray.GetOrigin() - closestObject.GetPosition();
	rayCollisionData.worldPosition = origin + ray.GetDirection() * closestCollisionDistance;
	//glm::vec3 furthestCollisionPoint = origin + rayDirection * farthestCollisionT;

	rayCollisionData.worldNormal = glm::normalize(rayCollisionData.worldPosition); // Minus sphere origin if it wasn't zero;
	rayCollisionData.worldPosition += closestObject.GetPosition(); // Put it back in place

	return rayCollisionData;
}

RayCollisionData RayTracer::FillCollisionDataOnHit(const Ray& ray, const World& world, float closestCollisionDistance, 
	int objectIndex) const
{
	RayCollisionData collisionData;
	collisionData.objectIndex = objectIndex;
	collisionData.collisionDistance = closestCollisionDistance;

	const CollidableObject& closestObject = world.GetCollidableObject(objectIndex);
	collisionData.worldPosition = ray.GetOrigin() + closestCollisionDistance * ray.GetDirection();
	collisionData.worldNormal = (collisionData.worldPosition - closestObject.GetPosition()) / closestObject.GetRadius();

	return collisionData;
}

RayCollisionData RayTracer::FillCollisionDataOnMiss(const Ray& ray) const
{
	RayCollisionData rayCollisionData;
	rayCollisionData.collisionDistance = -1.0f;
	return rayCollisionData;
}