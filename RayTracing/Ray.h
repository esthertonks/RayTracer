#pragma once

#include <glm/glm.hpp>

struct RayCollisionData
{
    // The index of the object that the ray collided with.
    int objectIndex;

    float collisionDistance;

    glm::vec3 worldNormal;
    glm::vec3 worldPosition;
};

class Ray
{
public:

    Ray(const glm::vec3& origin, const glm::vec3& direction)
        : m_origin(origin), m_direction(direction)
    {
    }

    glm::vec3 GetOrigin() const { return m_origin; };
    glm::vec3 GetDirection() const { return m_direction; };

    void SetOrigin(glm::vec3& origin) { m_origin = origin; };
    void SetDirection(glm::vec3& direction) { m_direction = direction; };

private:

    glm::vec3 m_origin;
    glm::vec3 m_direction;
};