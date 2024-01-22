#pragma once

#include <glm/glm.hpp>

class Utils
{
public:
    static uint32_t ColourToUIntRGBA(glm::vec4 colour)
    {
        uint8_t r = (uint8_t)(colour.r * 255);
        uint8_t g = (uint8_t)(colour.g * 255);
        uint8_t b = (uint8_t)(colour.b * 255);
        uint8_t a = (uint8_t)(colour.a * 255);

        return (uint32_t)((a << 24) + (r << 16) + (g << 8) + b);
    }

    static glm::vec4 UIntToColourRGBA(uint32_t colour)
    {
        uint32_t a = (colour >> 24) & 0xFF;
        uint32_t r = (colour >> 16) & 0xFF;
        uint32_t g = (colour >> 8) & 0xFF;
        uint32_t b = colour & 0xFF;

        glm::vec4 tempColour;
        tempColour.r = r / 255.0f;
        tempColour.g = g / 255.0f;
        tempColour.b = b / 255.0f;
        tempColour.a = a / 255.0f;

        return tempColour;
    }

    static glm::vec3 Lerp(const glm::vec3& colourA, const glm::vec3& colourB, glm::vec3 direction)
    {
        glm::vec3 unitDirection = glm::normalize(direction);
        float a = 0.5f * (unitDirection.y + 1.0f);
        return ((1.0f - a) * colourA) + (a * colourB);
    }
};

