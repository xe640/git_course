#ifndef RENDERER_GL
#define RENDERER_GL

#include "shader.h"
#include "gui_render.h"
#include "../include/GLM/glm.hpp"
#include "../include/GLM/gtc/matrix_transform.hpp"
#include "../include/GLM/gtc/type_ptr.hpp"

class GLRenderer{
public:
    static void Initialize();
    static void ReloadShaders();
    static void Render(gui_data state, glm::mat4(*getViewMat)());
    static void CleanUp();
private:
    static Shader* triangle_shader;
    static glm::mat4 projection_matrix;
    static glm::mat4 projection_view_matrix;
};

#endif