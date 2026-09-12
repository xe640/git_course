#include "renderer_gl.h"

Shader* GLRenderer::triangle_shader = nullptr;

void GLRenderer::Initialize(){
    triangle_shader = new Shader("../shaders/triangleVS.glsl", "../shaders/triangleFS.glsl");
    triangle_shader->use();

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
}

void GLRenderer::Render(gui_data state){
    if (triangle_shader->CompilationSucceeded())
    {
        ImVec4 clearCol = state.clearColour;
        ImVec4 triCol = state.triColour;
        glClearColor(clearCol.x, clearCol.y, clearCol.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        triangle_shader->setVec3Uniform("inColour", triCol.x, triCol.y, triCol.z);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
}

void GLRenderer::CleanUp(){
    if(triangle_shader != nullptr){
        free(triangle_shader);
        triangle_shader = nullptr;
    }
}