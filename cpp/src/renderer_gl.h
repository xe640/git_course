#ifndef RENDERER_GL
#define RENDERER_GL

#include "shader.h"
#include "gui_render.h"
#include "render_list_gen.h"
#include "../include/GLM/glm.hpp"
#include "../include/GLM/gtc/matrix_transform.hpp"
#include "../include/GLM/gtc/type_ptr.hpp"

struct texture_buffer{
    GLuint tbo;
    GLuint texture;
};

class GLRenderer{
public:
    static void Initialize();
    static void ReloadShaders();
    static void Render(gui_data state, glm::mat4(*getViewMat)());
    static void CleanUp();
private:
    static Shader* triangle_shader;
    static Shader* plane_shader;
    static Shader* line_shader;
    static glm::mat4 projection_matrix;
    static glm::mat4 projection_view_matrix;
    static GLuint VAO;
    static GLuint scene_data_ubo;
    static texture_buffer plane_data_buffer;
    static texture_buffer line_data_buffer;
    static render_list* r_list;
    static void gen_tbo(texture_buffer* tbuf, int size);
};

#endif