#include <iostream>
#include <stdio.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include "src/shader.h"
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

    IMGUI_CHECKVERSION();
    ImGuiContext* context = ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    static Shader triangleShader = Shader("../shaders/triangleVS.glsl", "../shaders/triangleFS.glsl");
    triangleShader.use();

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    while(!glfwWindowShouldClose(window))
    {
        GUIRender::UpdateInput(io, window);
        GUIRender::DrawGUI(io);
        
        glClearColor(GUIRender::data.clearColour.x, GUIRender::data.clearColour.y, GUIRender::data.clearColour.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        triangleShader.setVec3Uniform("inColour", GUIRender::data.triColour.x, GUIRender::data.triColour.y, GUIRender::data.triColour.z);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        GUIRender::RenderGUI();
        glfwSwapBuffers(window);

        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext(context);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
