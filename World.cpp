#include "World.h"

#include "CollidableObjects/Sphere.h"
#include "Materials/Diffuse.h"
#include "Materials/Emissive.h"
#include "Materials/FuzzyMetal.h"
#include "Materials/Lambertian.h"
#include "Materials/Metal.h"

World::~World()
{
	for (IMaterial* material : m_materials)
	{
		delete material;
	}

	for (CollidableObject* object : m_objects)
	{
		delete object;
	}
}

World::World() :
	m_lightDirection(1.0f, 0.73f, 0.0f)
{
	// TODO this should be loaded from a config file or map editor.

	glm::vec3 position1 = { 0.1f, -1.0f, -0.6f };
	float radius1 = { 1.0f };

	CollidableObject* object1 = new Sphere(position1, radius1, 0);
	m_objects.push_back(object1);

	glm::vec3 position2 = { -1.0f, 0.0f, -3.0f };
	float radius2 = { 1.0f };

	CollidableObject* object2 = new Sphere(position2, radius2, 1);
	m_objects.push_back(object2);

	glm::vec3 position3 = { 2.2f, 1.0f, 0.0f };
	float radius3 = { 0.7f };

	CollidableObject* object3 = new Sphere(position3, radius3, 2);
	m_objects.push_back(object3);

	glm::vec3 position4 = { 3.2f, -1.0f, 0.0f };
	float radius4 = { 0.2f };

	CollidableObject* object4 = new Sphere(position4, radius4, 3);
	m_objects.push_back(object4);

	glm::vec3 position5 = { 4.2f, -1.0f, 0.0f };
	float radius5 = { 0.5f };

	CollidableObject* object5 = new Sphere(position5, radius5, 4);
	m_objects.push_back(object5);

	glm::vec3 position6 = { 6.2f, -1.0f, 0.0f };
	float radius6 = { 1.0f };

	CollidableObject* object6 = new Sphere(position6, radius6, 5);
	m_objects.push_back(object6);

	glm::vec3 position7 = { 0.0f, 101.0f, 0.0f };
	float radius7 = { 100.0f };

	CollidableObject* object7 = new Sphere(position7, radius7, 6);
	m_objects.push_back(object7);

	IMaterial* yellowEmissive = new Emissive(2.0f, { 1.0f, 1.0f, 0.2f });
	IMaterial* purpleDiffuse = new Diffuse(m_lightDirection, 1.0f, glm::vec3(1.0f, 0.0f, 1.0f));
	IMaterial* redDiffuse = new Diffuse(m_lightDirection, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	IMaterial* blueLambert = new Lambertian(glm::vec3(0.0f, 0.0f, 1.0f));
	IMaterial* limeGreenFuzzyMetal = new FuzzyMetal(0.4f, glm::vec3(0.0f, 1.0f, 0.0f));
	IMaterial* forestGreenMetal = new Metal(glm::vec3(0.0f, 0.5f, 0.5f));
	IMaterial* greyMetal = new Metal(glm::vec3(0.5f, 0.5f, 0.5f));

	m_materials.emplace_back(yellowEmissive);
	m_materials.emplace_back(purpleDiffuse);
	m_materials.emplace_back(redDiffuse);
	m_materials.emplace_back(blueLambert);
	m_materials.emplace_back(limeGreenFuzzyMetal);
	m_materials.emplace_back(forestGreenMetal);
	m_materials.emplace_back(greyMetal);
}

const CollidableObject& World::GetCollidableObject(int index) const {
	return *m_objects[index];
};

IMaterial* World::GetMaterialPtr(int materialIndex) const {
	assert(materialIndex < m_materials.size());
	return m_materials[materialIndex];

};

void World::UpdateMaterial(IMaterial* newMaterial, int materialIndex) {
	assert(materialIndex < m_materials.size());
	m_materials[materialIndex] = newMaterial;
};
void World::AddMaterial(IMaterial* newMaterial, int materialIndex) {
	assert(materialIndex < m_materials.size());
	m_materials[materialIndex] = newMaterial;
};

void World::SetLightDirection(glm::vec3& lightDirection) {
	m_lightDirection = glm::normalize(lightDirection);
	for (IMaterial* material : m_materials)
	{
		if (material->GetType() == MaterialType::Diffuse)
		{
			Diffuse* diffuseMaterial = dynamic_cast<Diffuse*>(material);
			assert(diffuseMaterial);

			diffuseMaterial->UpdateLightDirection(m_lightDirection);
		}
	}
};
