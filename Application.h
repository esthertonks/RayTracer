#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <functional>

class Renderer;
class TextureRenderer;
class ShaderManager;
class Camera;
class RayEmitter;
class RayTracer;
class RayTracedImage;
class Window;
class World;
struct ID3D11DeviceContext;

class Application
{
public:

	Application();
	~Application();

	bool Initialise();
	bool Run();

private:
	void InitImGuiStyle();

	bool Update(float deltaTime);
	bool Render();
	bool Resize();

	bool UpdateFromMouse();
	bool UpdateFromKeyPress(float deltaTime);

	bool m_initialised;
	bool m_needsResize;

	std::unique_ptr<TextureRenderer> m_textureRenderer;
	std::unique_ptr<RayTracedImage> m_rayTracedImage;
	std::unique_ptr<RayEmitter> m_rayEmitter;
	std::unique_ptr<RayTracer> m_rayTracer;
	std::unique_ptr<Window> m_window;
	std::unique_ptr<World> m_world;

	glm::vec2 m_lastMousePosition;
	float m_mouseSensitivity;

	float m_lastFrameTime;
	float m_generationTime;

	bool m_imageViewHovered;
};

