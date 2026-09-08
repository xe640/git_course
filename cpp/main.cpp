#include <iostream>
#include <stdio.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

const char *vertexShaderSource = "#version 330 core\n"
    "void main()\n"
    "{\n"
    "   int x = gl_VertexID % 3;\n"
    "   x -= 1;\n"
    "   float y = 0.5 - abs(x);\n"
    "   gl_Position = vec4(x * 0.5, y, 0.0, 1.0);\n"
    "}\0";

const char *fragmentShaderSource = "#version 330 core \n"
    "out vec4 FragColor;\n"

    "void main()\n"
    "{\n"
    "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "} \0";

static bool shaderSuccess = true;

void onWindowResize(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

GLuint baseShaderProgram() {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    int  success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        shaderSuccess = false;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        shaderSuccess = false;
    }

    GLuint shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::_LINKING_FAILED\n" << infoLog << std::endl;
        shaderSuccess = false;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
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

    GLuint shaderProgram = baseShaderProgram();
    glUseProgram(shaderProgram);
    GLuint VAO;
    if(shaderSuccess) {  // Empty vertex array object, it is still required despite no actual vertex data being used
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
    }

    static ImVec2 cameraYawPitch = ImVec2(.0f, .0f);
    static ImVec4 clear_colour = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    static double lastMouseX = 0.0;
    static double lastMouseY = 0.0;
    static float look_sensitivity = 1.0f;

    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        if(!io.WantCaptureMouse) {
            int mouseClickState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
            if (mouseClickState == GLFW_PRESS){
                static double curMouseX, curMouseY;
                glfwGetCursorPos(window, &curMouseX, &curMouseY);
                cameraYawPitch.x += (curMouseX - lastMouseX) * look_sensitivity * 0.1f;
                cameraYawPitch.y -= (curMouseY - lastMouseY) * look_sensitivity * 0.1f;
            }
            glfwGetCursorPos(window, &lastMouseX, &lastMouseY);
        }
        if(!io.WantCaptureKeyboard) {

        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            ImGui::Begin("info");
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::Text("Camera angles (%.1f, %.1f)", cameraYawPitch.x, cameraYawPitch.y);
            ImGui::DragFloat("Look sensitivity", &look_sensitivity, 0.01f);
            ImGui::ColorEdit3("Background colour", (float*)&clear_colour);
            ImGui::End();
        }

        glClearColor(clear_colour.x, clear_colour.y, clear_colour.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        if(shaderSuccess) {
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext(context);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
