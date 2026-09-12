#ifndef RENDERER_GL
#define RENDERER_GL

#include "shader.h"
#include "gui_render.h"

class GLRenderer{
public:
    static void Initialize();
    static void Render(gui_data state);
    static void CleanUp();
private:
    static Shader* triangle_shader;
};

#endif