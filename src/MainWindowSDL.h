#pragma once

#include <entt/fwd.hpp>
#include <memory>

struct SDL_Window;
using SDL_GLContext = void*;

class MainWindowSDL {
public:
    MainWindowSDL(entt::registry& registry);
    ~MainWindowSDL();

    bool StartFrame();
    void EndFrame();

private:
    SDL_Window* m_window;
    SDL_GLContext m_GLContext;

    entt::registry& m_registry;
};