#pragma once

#include <iostream>
#include <glm/glm.hpp>
#include <SDL.h>
#include <SDL_syswm.h>

struct SDL_Window;

class Window
{
public:
    Window();
    ~Window();

    bool Initialise(const std::string& windowTitle);

    float GetWidth() const { return m_mainWindowRect.x; };
    float GetHeight() const { return m_mainWindowRect.y; };
    glm::vec2 GetRect() const { return m_mainWindowRect; };

    SDL_Window* GetInternalWindow() const { return m_window; };

    HWND GetHWND() {
        SDL_SysWMinfo wmInfo;
        SDL_VERSION(&wmInfo.version);
        SDL_GetWindowWMInfo(m_window, &wmInfo);
        return (HWND)wmInfo.info.win.window;
    }

    bool IsLeftMouseButtonDown() const;
    bool IsRightMouseButtonDown() const;
    bool IsMiddleMouseButtonDown() const;

    glm::vec2 GetMousePosition() const;

    bool IsKeyDown(SDL_Keycode keycode) const;

    void SetDefaultCursor();
    void SetWaitCursor();

    bool HandleEventLoop(float deltaTime);

    glm::vec2 m_mainWindowRect;
    glm::vec2 m_renderWindowRect;

private:

    bool InitSDLWindow(const std::string& windowTitle);

    SDL_Cursor* m_waitCursor;
    SDL_Window* m_window;

};
