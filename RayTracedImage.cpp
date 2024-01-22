#include "RayTracedImage.h"

#include <execution>
#include <iostream>
#include "RayTracing/Ray.h"
#include "RayTracing/RayEmitter.h"
#include "RayTracing/RayTracer.h"
#include "Utils/Utils.h"

RayTracedImage::RayTracedImage() :
	m_pixels(nullptr),
	m_dimensions(0.0f, 0.0f)
{

}

RayTracedImage::~RayTracedImage()
{
	Destroy();
}

void RayTracedImage::Destroy()
{
	if (m_accumulationSettings.m_data)
	{
		delete[] m_accumulationSettings.m_data;
		m_accumulationSettings.m_data = 0;
	}

	if (m_pixels)
	{
		delete[] m_pixels;
	}
}

bool RayTracedImage::Initialise(glm::vec2 renderRect)
{
	m_dimensions = renderRect;

	m_accumulationSettings.m_data = new glm::vec4[(int)renderRect.x * (int)renderRect.y];

	m_xIndexIterator.resize((uint32_t)m_dimensions.x);
	m_yIndexIterator.resize((uint32_t)m_dimensions.y);
	for (uint32_t i = 0; i < m_dimensions.x; i++)
		m_xIndexIterator[i] = i;
	for (uint32_t i = 0; i < m_dimensions.y; i++)
		m_yIndexIterator[i] = i;

	int imageSize = (int)m_dimensions.x * (int)m_dimensions.y;

	// Allocate memory for the pixel data that we will pass to a direct x texture.
	m_pixels = new uint32_t[imageSize];

	return true;
}

uint32_t* RayTracedImage::FillPixels(const RayTracer& rayTracer, const RayEmitter& rayEmitter, const World& world)
{
	assert(m_pixels != nullptr);

	if (!rayEmitter.Ready())
	{
		std::cout << "ray emitter not initialised" << std::endl;
		return nullptr;
	}

	if (m_accumulationSettings.m_frameIndex == 1)
		memset(m_accumulationSettings.m_data, 0, (size_t)m_dimensions.x * (size_t)m_dimensions.y * (size_t)sizeof(glm::vec4));

#define MULTITHREADED 1
#if MULTITHREADED
	std::for_each(std::execution::par, m_yIndexIterator.begin(), m_yIndexIterator.end(),
		[this, &rayTracer, &rayEmitter, &world](uint32_t y)
		{
			std::for_each(std::execution::par, m_xIndexIterator.begin(), m_xIndexIterator.end(),
			[this, &rayTracer, &rayEmitter, &world, y](uint32_t x)
				{
					Ray ray(rayEmitter.GetPosition(), rayEmitter.GetRayDirection(x, y));
					ProcessPixel(m_pixels, x, y, ray, rayTracer, world);
				});
		});

#else

	for (int y = 0; y < m_dimensions.y; y++)
	{
		for (int x = 0; x < m_dimensions.x; x++)
		{
			Ray ray(rayEmitter.GetPosition(), rayEmitter.GetRayDirection(x, y));
			ProcessPixel(m_pixels, x, y, ray, rayTracer, world);
		}
	}
#endif

	if (m_accumulationSettings.m_accumulate)
	{
		m_accumulationSettings.m_frameIndex++;
	}
	else
	{
		m_accumulationSettings.m_frameIndex = 1;
	}

	return m_pixels;
}

bool RayTracedImage::ProcessPixel(uint32_t* pixels, int x, int y, const Ray& ray, const RayTracer& rayTracer, const World &world)
{
	glm::vec3 colour = rayTracer.CalculatePixelColour(x, y, 10, world, ray);

	m_accumulationSettings.m_data[x + y * (int)m_dimensions.x] += glm::vec4(colour, 1.0f);

	// Average the accumulated colour
	glm::vec4 accumulatedColour = m_accumulationSettings.m_data[x + y * (int)m_dimensions.x];
	accumulatedColour /= (float)m_accumulationSettings.m_frameIndex;
	accumulatedColour = glm::clamp(accumulatedColour, glm::vec4(0.0f), glm::vec4(1.0f));

	pixels[x + y * (int)m_dimensions.x] = Utils::ColourToUIntRGBA(accumulatedColour);

	return true;
}

void RayTracedImage::Resize(glm::vec2 renderRect)
{
	ResetFrameIndex();
	Destroy();

	Initialise(renderRect);
}


