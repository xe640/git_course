#include <iostream>
#include <stdio.h>

#include "include/glad/glad.h"
#include "include/GLFW/glfw3.h"
#include "include/imgui/imgui.h"
#include "include/imgui/imgui_impl_glfw.h"
#include "include/imgui/imgui_impl_opengl3.h"

#include "src/renderer_gl.h"
#include "src/gui_render.h"

void onWindowResize(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main(int, char **) {
    std::cout << "Progam start" << std::endl;
    glfwInit();

    const char* glsl_version = nullptr;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());

    GLFWwindow* window = glfwCreateWindow(800 * main_scale, 600 * main_scale, "Test", NULL, NULL);
    if (window == NULL) {
        std::cout << "Window creation failed" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, 800 * main_scale, 600 * main_scale);
    glfwSetFramebufferSizeCallback(window, onWindowResize);

    GUIRender::Initialize(main_scale, window, glsl_version);
    GLRenderer::Initialize();
    GUIRender::SetShaderReloadCallback(GLRenderer::ReloadShaders);

    while(!glfwWindowShouldClose(window))
    {
        GUIRender::UpdateInput(window);
        GUIRender::DrawGUI();
        
        GLRenderer::Render(GUIRender::data, GUIRender::getViewTransform);

        GUIRender::RenderGUI();
        glfwSwapBuffers(window);

        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }
    }

    GLRenderer::CleanUp();
    GUIRender::CleanUp();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
