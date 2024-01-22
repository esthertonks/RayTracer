#include "Sphere.h"
#include "../RayTracing/Ray.h"
#include "../World.h"

// Returns T distance along the ray at the location of the first intersection with a sphere.
// TODO try simplyfying ie: https://raytracing.github.io/books/RayTracingInOneWeekend.html Listing 14 and 16.
float Sphere::Intersect(const Ray& ray, const World& world) const
{
	glm::vec3 origin = ray.GetOrigin() - GetPosition();

	// See https://youtu.be/v9vndyfk2U8
	// a---------------------  + b----------------------- + c------------------------
	// (bx^2 + by^2 + bz^2)t^2 + (2(axbx + ayby + azbz))t + (ax^2 + ay^2 + az^2 - r^2) = 0
	// a = ray origin
	// b = ray direction
	// r= radius
	// t = intersection distance
	float quadraticCoefficientA = glm::dot(ray.GetDirection(), ray.GetDirection());
	float quadraticCoefficientB = 2.0f * glm::dot(origin, ray.GetDirection());
	float quadraticCoefficientC = glm::dot(origin, origin) - GetRadius() * GetRadius();

	// Quadratic formula discriminant
	// b^2 - 4ac - intersect of not.
	// (-b +- glm::sqrt(dscriminant))/(2.0f * a) - find exact intersection coord.
	float discriminant = quadraticCoefficientB * quadraticCoefficientB - 4 * quadraticCoefficientA * quadraticCoefficientC;
	if (discriminant < 0.0f)
	{
		return -1.0f;
	}

	float closestIntersection = (-quadraticCoefficientB - glm::sqrt(discriminant)) / (2.0f * quadraticCoefficientA);
	// Furthest intersection - not currently used.
	//float farthestIntersection = (-quadraticCoefficientB + glm::sqrt(discriminant)) / (2.0f * quadraticCoefficientA);

	return closestIntersection;
}