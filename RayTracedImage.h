#pragma once

#include <vector>
#include <glm/glm.hpp>

class Ray;
class RayEmitter;
class RayTracer;
class World;

class RayTracedImage
{

public:

    struct AccumulationSettings
    {
        glm::vec4* m_data = nullptr;
        uint32_t m_frameIndex = 0;
        bool m_accumulate = true;
    };

    RayTracedImage();
    ~RayTracedImage();

    bool Initialise(glm::vec2 renderRect);
    uint32_t* FillPixels(const RayTracer& rayTracer, const RayEmitter& rayEmitter, const World& world);
    void Resize(glm::vec2 renderRect);
    void ResetFrameIndex() { m_accumulationSettings.m_frameIndex = 1; };

    inline void SetAccumulate(bool accumulate)
    {
        m_accumulationSettings.m_accumulate = accumulate;
    }

    inline bool GetAccumulate() const
    {
        return m_accumulationSettings.m_accumulate;
    }

private:

    void Destroy();
    bool ProcessPixel(uint32_t* pixels, int x, int y, const Ray& ray, const RayTracer& rayTracer, const World &world);

    AccumulationSettings m_accumulationSettings;
    std::vector<uint32_t> m_xIndexIterator, m_yIndexIterator;
    glm::vec2 m_dimensions;

    uint32_t* m_pixels;
};

