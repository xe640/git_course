#include "gui_render.h"

double GUIRender::last_mouse_x = 0.0;
double GUIRender::last_mouse_y = 0.0;
float GUIRender::look_sensitivity = 1.0f;
bool GUIRender::last_mouse_press = false;
void(*GUIRender::shader_reload_callback)() = nullptr;

gui_data GUIRender::data = {
    ImVec2(.0f, .0f),
    ImVec4(0.45f, 0.55f, 0.60f, 1.00f), 
    ImVec4(0.2f, 0.2f, 0.2f, 1.0f)
};


void GUIRender::UpdateInput(ImGuiIO& io, GLFWwindow* window){
    glfwPollEvents();

    if(!io.WantCaptureMouse) {
        int windowWidth, windowHeight;
        glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
        int mouseClickState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        if (mouseClickState == GLFW_PRESS){
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            static double curMouseX, curMouseY;
            glfwGetCursorPos(window, &curMouseX, &curMouseY);
            data.cameraYawPitch.x += (curMouseX - last_mouse_x) * look_sensitivity * 0.1f;
            data.cameraYawPitch.y -= (curMouseY - last_mouse_x) * look_sensitivity * 0.1f;
            last_mouse_press = true;
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            if(last_mouse_press) {
                glfwSetCursorPos(window, windowWidth * 0.5, windowHeight * 0.5);
            }
            last_mouse_press = false;
        }
        glfwGetCursorPos(window, &last_mouse_x, &last_mouse_y);
    }
    if(!io.WantCaptureKeyboard) {

    }
}

void GUIRender::DrawGUI(ImGuiIO& io){
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    {
        ImGui::Begin("info");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::Text("Camera angles (%.1f, %.1f)", data.cameraYawPitch.x, data.cameraYawPitch.y);
        ImGui::DragFloat("Look sensitivity", &look_sensitivity, 0.01f);
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