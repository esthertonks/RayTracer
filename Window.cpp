#include "Window.h"

#include "ThirdParty/imgui/imgui_impl_sdl2.h"

Window::Window() :
	m_mainWindowRect(1250.0f, 700.0f),
	m_renderWindowRect(893.0f, 647.0f),
	m_waitCursor(nullptr),
	m_window(nullptr)
{
}

Window::~Window()
{
	SDL_FreeCursor(m_waitCursor);

	SDL_DestroyWindow(m_window);
	SDL_Quit();
}

bool Window::Initialise(const std::string& windowTitle)
{
	return InitSDLWindow(windowTitle);
}


bool Window::InitSDLWindow(const std::string& windowTitle)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		std::cout << "Error initializing SDL: " << SDL_GetError() << std::endl;
		system("pause");
		return false;
	}

	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	m_window = SDL_CreateWindow(windowTitle.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, (int)m_mainWindowRect.x, (int)m_mainWindowRect.y, window_flags);
	if (!m_window) {
		std::cout << "Error creating window: " << SDL_GetError() << std::endl;
		system("pause");
		return false;
	}

	m_waitCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT);

	return true;
}

bool Window::IsKeyDown(SDL_Keycode keycode) const
{
	const Uint8* keystates = SDL_GetKeyboardState(0);
	SDL_PumpEvents();

	SDL_Scancode scanCode = SDL_GetScancodeFromKey(keycode);
	if (keystates[scanCode]) {
		return true;
	}

	return false;
}

bool Window::IsLeftMouseButtonDown() const
{
	Uint32 buttonState = SDL_GetMouseState(0, 0);
	if (buttonState & SDL_BUTTON(SDL_BUTTON_LEFT))
		return true;
	
	return false;
}

bool Window::IsRightMouseButtonDown() const
{
	Uint32 buttonState = SDL_GetMouseState(0, 0);
	if (buttonState & SDL_BUTTON(SDL_BUTTON_RIGHT))
		return true;

	return false;
}

bool Window::IsMiddleMouseButtonDown() const
{
	Uint32 buttonState = SDL_GetMouseState(0, 0);
	if (buttonState & SDL_BUTTON(SDL_BUTTON_MIDDLE))
		return true;

	return false;
}

glm::vec2 Window::GetMousePosition() const
{
	int mousePosX;
	int mousePosY;
	Uint32 state = SDL_GetMouseState(&mousePosX, &mousePosY);

	return { (float)mousePosX, (float)mousePosY };
}

void Window::SetDefaultCursor()
{
	SDL_SetCursor(SDL_GetDefaultCursor());
}

void Window::SetWaitCursor()
{
	SDL_SetCursor(m_waitCursor);
}

bool Window::HandleEventLoop(float deltaTime)
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		ImGui_ImplSDL2_ProcessEvent(&event);
		if (event.type == SDL_QUIT)
			return false;
		if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE)
		{
			if (event.window.windowID == SDL_GetWindowID(m_window))
				return false;
		}
		if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED)
		{
			if (event.window.windowID == SDL_GetWindowID(m_window))
				m_mainWindowRect = { event.window.data1, event.window.data2 };
		}
	}

	return true;
}


