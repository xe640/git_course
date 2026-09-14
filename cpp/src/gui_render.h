#ifndef GUI_RENDER_H
#define GUI_RENDER_H

#include "../include/GLFW/glfw3.h"
#include "../include/imgui/imgui.h"
#include "../include/imgui/imgui_impl_glfw.h"
#include "../include/imgui/imgui_impl_opengl3.h"

struct gui_data {
    ImVec2 cameraYawPitch;
    ImVec4 clearColour;
    ImVec4 triColour;
};

class GUIRender
{
private:
    static double last_mouse_x;
    static double last_mouse_y;
    static float look_sensitivity;
    static bool last_mouse_press;
    static void(*shader_reload_callback)();
    static ImGuiIO* io;
    static ImGuiContext* context;
public:
    static gui_data data;

    static void Initialize(float mainScale, GLFWwindow* window, const char* glslVersion);
    static void UpdateInput(GLFWwindow* window);
    static void DrawGUI();
    static void RenderGUI();
    static void SetShaderReloadCallback(void(*shaderReloadCallback)());
    static void CleanUp();
};

#endif
