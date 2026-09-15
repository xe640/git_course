#include "gui_render.h"

double GUIRender::last_mouse_x = 0.0;
double GUIRender::last_mouse_y = 0.0;
float GUIRender::look_sensitivity = 1.0f;
float GUIRender::move_speed = 5.0;
bool GUIRender::last_mouse_press = false;
void(*GUIRender::shader_reload_callback)() = nullptr;
ImGuiIO* GUIRender::io = nullptr;
ImGuiContext* GUIRender::context = nullptr;
glm::vec3 GUIRender::cam_pos = glm::vec3(0.0f, 0.0f, 0.0f);
glm::mat3 GUIRender::cam_basis = glm::mat3(1.0f);
float GUIRender::delta_time = 0.0f;
double GUIRender::last_frame = 0.0f;

gui_data GUIRender::data = {
    ImVec2(.0f, .0f),
    ImVec4(0.45f, 0.55f, 0.60f, 1.00f), 
    ImVec4(0.2f, 0.2f, 0.2f, 1.0f),
    110.0f,
    glm::vec2(1.0f, 1.0f),
    false,
    true
};

void GUIRender::Initialize(float mainScale, GLFWwindow* window, const char* glslVersion){
    IMGUI_CHECKVERSION();
    context = ImGui::CreateContext();
    io = &ImGui::GetIO(); (void)io;
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(mainScale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = mainScale;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glslVersion);
}

void GUIRender::UpdateInput(GLFWwindow* window){
    glfwPollEvents();
    float currentFrame = glfwGetTime();
    delta_time = currentFrame - last_frame;
    last_frame = currentFrame;

    data.windowChanged = false;
    int windowWidth, windowHeight;
    glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
    glm::vec2 newAspect = glm::vec2((float)windowWidth, (float)windowHeight);
    if(!glm::all(glm::equal(data.windowAspect, newAspect))) {
        data.windowChanged = true;
        data.windowAspect = newAspect;
    }

    if(!io->WantCaptureMouse) {
        int mouseClickState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        if (mouseClickState == GLFW_PRESS){
            data.cameraChanged = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            static double curMouseX, curMouseY;
            glfwGetCursorPos(window, &curMouseX, &curMouseY);
            data.cameraYawPitch.x -= (curMouseX - last_mouse_x) * look_sensitivity * 0.01f;
            data.cameraYawPitch.y += (curMouseY - last_mouse_y) * look_sensitivity * 0.01f;
            data.cameraYawPitch.y = glm::clamp(data.cameraYawPitch.y, -89.9f, 89.9f);
            last_mouse_press = true;

            const float pitch_cos = glm::cos(glm::radians(data.cameraYawPitch.y));
            const glm::vec3 cam_fwd = glm::vec3(
                glm::sin(glm::radians(data.cameraYawPitch.x)) * pitch_cos,
                glm::sin(glm::radians(data.cameraYawPitch.y)),
                glm::cos(glm::radians(data.cameraYawPitch.x)) * pitch_cos
            );

            glm::vec3 cam_up, cam_right;
            cam_right = glm::normalize(glm::cross(cam_fwd, glm::vec3(0.0f, 1.0f, 0.0f)));
            cam_up = glm::cross(cam_right, cam_fwd);
            cam_basis = glm::mat3(cam_right, cam_up, cam_fwd);
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            if(last_mouse_press) {
                glfwSetCursorPos(window, windowWidth * 0.5, windowHeight * 0.5);
            }
            last_mouse_press = false;
        }
        glfwGetCursorPos(window, &last_mouse_x, &last_mouse_y);
    }
    if(!io->WantCaptureKeyboard) {
        glm::vec3 moveDir = glm::vec3(
            (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS? 1.0f : 0.0f)
            - (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS? 1.0f : 0.0f),
            (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS? 1.0f : 0.0f)
            - (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS? 1.0f : 0.0f),
            (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS? 1.0f : 0.0f)
            - (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS? 1.0f : 0.0f)
        );
        if(glm::length(moveDir) > 0.1f) {
            moveDir = glm::normalize(moveDir);
        }
        moveDir *= move_speed * delta_time;
        cam_pos += glm::transpose(cam_basis) * moveDir;
    }
}

void GUIRender::DrawGUI(){
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    {
        ImGui::Begin("info");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io->Framerate, io->Framerate);
        ImGui::Text(
            "Camera angles (%.1f, %.1f)\n Camera pos (%.1f, %.1f, %.1f)", 
            data.cameraYawPitch.x, data.cameraYawPitch.y,
            cam_pos.x, cam_pos.y, cam_pos.z
        );
        ImGui::DragFloat("Look sensitivity", &look_sensitivity, 0.01f);
        if(ImGui::DragFloat("Fov", &data.fov, 1.0f, 30.0f, 160.0f)) {
            data.windowChanged = true;
        }
        ImGui::DragFloat("Move speed", &move_speed);
        ImGui::ColorEdit3("Background colour", (float*)&data.clearColour);
        ImGui::ColorEdit3("Triangle colour", (float*)&data.triColour);

        if(ImGui::Button("Reload shaders") && shader_reload_callback != nullptr) {
            shader_reload_callback();
        }

        ImGui::End();
    }
}

void GUIRender::RenderGUI(){
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUIRender::SetShaderReloadCallback(void(*shaderReloadCallback)()){
    shader_reload_callback = shaderReloadCallback;
}

void GUIRender::CleanUp(){
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext(context);
}

glm::mat4x4 GUIRender::getViewTransform(){
    glm::mat4x4 viewMatrix = glm::mat4(cam_basis);
    viewMatrix[3][3] = 1.0f;
    viewMatrix = glm::translate(viewMatrix, -cam_pos);
    return viewMatrix;
}