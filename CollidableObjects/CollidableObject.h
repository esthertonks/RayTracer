#pragma once
#include <glm/glm.hpp>

class Ray;
class World;

class CollidableObject 
{
public:
    CollidableObject(const glm::vec3& position, float radius, int materialIndex)
        : m_position(position), m_radius(radius), m_materialIndex(materialIndex)
    {
    }

    virtual ~CollidableObject() 
    {
    }

    float GetRadius() const { return m_radius; };
    void SetRadius(float radius)
    {
        m_radius = radius;
    };

    glm::vec3 GetPosition() const { return m_position; };
    virtual void SetPosition(glm::vec3& position)
    {
        m_position = position;
    };

    int GetMaterialIndex() const
    {
        return m_materialIndex;
    };
    void SetMaterialIndex(int materialIndex)
    {
        m_materialIndex = materialIndex;
    };

    virtual float Intersect(const Ray& ray, const World& world) const = 0;

private:
    glm::vec3 m_position{ 0.0f, 0.0f, 0.0f };
    int m_materialIndex = 0;
    float m_radius = 0.5f;
};