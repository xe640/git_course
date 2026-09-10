#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

struct gui_data {
    ImVec2 cameraYawPitch;
    ImVec4 clearColour;
    ImVec4 triColour;
};

class GUIRender
{
private:
    static double lastMouseX;
    static double lastMouseY;
    static float look_sensitivity;
    static bool lastMousePress;
public:
    static gui_data data;

    static void UpdateInput(ImGuiIO& io, GLFWwindow* window);
    static void DrawGUI(ImGuiIO& io);
    static void RenderGUI();
};
