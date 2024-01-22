#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <random>

//TODO Random is a bottleneck, could speed it up. See:
//https://www.reedbeta.com/blog/quick-and-easy-gpu-random-numbers-in-d3d11/
//https://www.reedbeta.com/blog/hash-functions-for-gpu-rendering/
// See also https://www.youtube.com/watch?v=5_RAHZQCPjE&list=PLlrATfBNZ98edc5GshdBtREv5asFW3yXl&index=16

class Random
{
public:

	static std::mt19937& GetRandomEngine() {
		// Initialize the random engine for each thread
		thread_local static std::mt19937 s_randomEngine(std::random_device{}());
		return s_randomEngine;
	}

	// See https://learn.microsoft.com/en-us/events/goingnative-2013/rand-considered-harmful
	// Faster than the real distributition
	static float Float()
	{
		static std::uniform_int_distribution<std::mt19937::result_type> s_distribution;
		return (float)s_distribution(GetRandomEngine()) / (float)std::numeric_limits<uint32_t>::max();
	}

	static float Float(float min, float max)
	{
		return Float() * (max - min) + min;
	}

	static glm::vec3 Vec3()
	{
		return glm::vec3(Float(), Float(), Float());
	}

	static glm::vec3 Vec3(float min, float max)
	{
		return glm::vec3(Float() * (max - min) + min, Float() * (max - min) + min, Float() * (max - min) + min);
	}

	static glm::vec3 RandomInUnitSphere() {
		while (true) {
			auto p = Random::Vec3(-1, 1);
			if (glm::length2(p) < 1)
				return p;
		}
	}

	static glm::vec3 RandomUnitVector() {
		return glm::normalize(RandomInUnitSphere());
	}

    //https://brsr.github.io/2021/05/01/vector-spherical-geometry.html
	static glm::vec3 VectorInUnitSphere()
	{
		return glm::normalize(Vec3(-1.0f, 1.0f));
	}

	// See https://stackoverflow.com/questions/71161952/how-random-is-glmsphericalrand-some-angles-seem-more-common-than-others
	static glm::vec3 Spherical()
	{
		float rho = 1.0;
		float theta = 2 * glm::pi<float>() * Float();
		float phi = acos(2.0f * (Float() - 1.0f));

		glm::vec3 cartesianCoords(0.0);
		cartesianCoords.x = rho * sin(phi) * cos(theta);
		cartesianCoords.y = rho * sin(phi) * sin(theta);
		cartesianCoords.z = rho * cos(phi);
		return cartesianCoords;

	}

	static glm::vec3 UnitSphereWithOnHemisphereCheck(const glm::vec3& normal)
	{
		glm::vec3 on_unit_sphere = Random::VectorInUnitSphere();
		if (dot(on_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
			return on_unit_sphere;
		else
			return -on_unit_sphere;
	}
};


