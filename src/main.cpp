#include "MainWindowSDL.h"

#include "Systems/GUISystem.h"
#include "Systems/Physics.h"

#include <entt/entt.hpp>
#include <imgui.h>

#include <iostream>
void draw_imgui_demos();

int main(int argc, char** argv)
{
    entt::registry registry;

    auto window = MainWindowSDL(registry);
    bool done = false;
    
    // This will run at a fixed time step.  If we want it to run matched with our
    // system time then we can add just a little std::chrono's magic
    double currentTime = 0;
    double dt = 1.0/1000.0;
    double lastFrameTime = -1;
    constexpr double frameDt = 1.0 / 60.0; // 60fps

    while (!done) {
        Systems::PhysicsUpdate(registry, currentTime, dt);

        // Limit GUI redraw rate to 60hz
        if (currentTime - lastFrameTime > frameDt) {
            done = window.StartFrame();
            //draw_imgui_demos();
            done |= !(GUISystem::Update(registry));
            window.EndFrame();
            lastFrameTime = currentTime;
        }
        
        currentTime += dt;
    }
    
    return 0;
}

// Stolen from ImGui example applications
void draw_imgui_demos() {
    static bool show_demo_window = true;
    static bool show_another_window = true;
    ImGui::ShowDemoWindow(&show_demo_window);

// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &show_another_window);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }

    // 3. Show another simple window.
    if (show_another_window)
    {
        ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            show_another_window = false;
        ImGui::End();
    }
}