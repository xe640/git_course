#ifndef GUI_RENDER_H
#define GUI_RENDER_H

#include <iostream>
#include <stdio.h>

#include "../include/GLFW/glfw3.h"
#include "../include/imgui/imgui.h"
#include "../include/imgui/imgui_impl_glfw.h"
#include "../include/imgui/imgui_impl_opengl3.h"
#include "../include/GLM/glm.hpp"
#include "../include/GLM/gtc/matrix_transform.hpp"
#include "../include/GLM/gtc/type_ptr.hpp"
#include "world_storage.h"

struct gui_data {
    ImVec2 cameraYawPitch;
    ImVec4 clearColour;
    ImVec4 triColour;
    float fov;
    glm::vec2 windowAspect;
    bool windowChanged;
    bool cameraChanged;
};

class GUIRender
{
private:
    static double last_mouse_x;
    static double last_mouse_y;
    static float look_sensitivity;
    static float move_speed;
    static bool last_mouse_press;
    static void(*shader_reload_callback)();
    static ImGuiIO* io;
    static ImGuiContext* context;
    static glm::vec3 cam_pos;
    static glm::mat3 cam_basis;
    static float delta_time;
    static double last_frame;
    static void update_cam_basis();
    static bool use_alternate_controls;
    static bool plane_placed;
    static float plane_size;
    static float plane_cooldown;
public:
    static gui_data data;

    static void Initialize(float mainScale, GLFWwindow* window, const char* glslVersion);
    static void UpdateInput(GLFWwindow* window);
    static void DrawGUI();
    static void RenderGUI();
    static void SetShaderReloadCallback(void(*shaderReloadCallback)());
    static void CleanUp();
    static glm::mat4x4 getViewTransform();
};

#endif
