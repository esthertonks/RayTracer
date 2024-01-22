#pragma once

#include <glm/glm.hpp>
#include <vector>

class CollidableObject;
class IMaterial;

class World
{
public:
	World();
	~World();

	std::vector<CollidableObject*>& GetObjectsNonConst() { return m_objects; };
	const std::vector<CollidableObject*>& GetCollidableObjects() const { return m_objects; };
	const CollidableObject& GetCollidableObject(int index) const;

	inline int GetNumMaterials() const {
		return (int)m_materials.size();

	};

	IMaterial* GetMaterialPtr(int materialIndex) const;

	void UpdateMaterial(IMaterial* newMaterial, int materialIndex);
	void AddMaterial(IMaterial* newMaterial, int materialIndex);

	inline glm::vec3 GetLightDirection() {
		return m_lightDirection;
	};

	void SetLightDirection(glm::vec3& lightDirection);

private:
	glm::vec3 m_lightDirection;
	std::vector<IMaterial*> m_materials;
	std::vector<CollidableObject*> m_objects;
};

