#include "renderer_gl.h"

Shader* GLRenderer::triangle_shader = nullptr;
glm::mat4 GLRenderer::projection_matrix = glm::mat4(1.0f);
glm::mat4 GLRenderer::projection_view_matrix = glm::mat4(1.0f);

void GLRenderer::Initialize(){
    triangle_shader = new Shader("../shaders/triangleVS.glsl", "../shaders/triangleFS.glsl");

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
}

void GLRenderer::ReloadShaders(){
    if(triangle_shader != nullptr) {
        triangle_shader->reload();
    }
}

void GLRenderer::Render(gui_data state, glm::mat4(*getViewMat)()){
    if(state.windowChanged) {
        projection_matrix = glm::perspective(
            glm::radians(state.fov),
            state.windowAspect.x/state.windowAspect.y,
        0.01f, 200.0f);
    }
    if(state.cameraChanged) {
        projection_view_matrix = projection_matrix * getViewMat();
    }

    if (triangle_shader->CompilationSucceeded())
    {
        triangle_shader->use();
        ImVec4 clearCol = state.clearColour;
        ImVec4 triCol = state.triColour;
        glClearColor(clearCol.x, clearCol.y, clearCol.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        triangle_shader->setVec3Uniform("inColour", triCol.x, triCol.y, triCol.z);
        triangle_shader->setMat4Uniform("projectionViewMat", projection_view_matrix);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
}

void GLRenderer::CleanUp(){
    if(triangle_shader != nullptr){
        free(triangle_shader);
        triangle_shader = nullptr;
    }
}