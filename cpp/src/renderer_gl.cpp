#include "renderer_gl.h"

Shader* GLRenderer::triangle_shader = nullptr;
glm::mat4 GLRenderer::projection_matrix = glm::mat4(1.0f);
glm::mat4 GLRenderer::projection_view_matrix = glm::mat4(1.0f);
GLuint GLRenderer::VAO;
GLuint GLRenderer::scene_data_ubo;

void GLRenderer::Initialize(){
    triangle_shader = new Shader("../shaders/triangleVS.glsl", "../shaders/triangleFS.glsl");

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &scene_data_ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, scene_data_ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, scene_data_ubo);
    triangle_shader->bindUBO("sceneGlobal", 0);
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
    if(state.cameraChanged || state.windowChanged) {
        projection_view_matrix = projection_matrix * getViewMat();
        glBindBuffer(GL_UNIFORM_BUFFER, scene_data_ubo);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection_view_matrix));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    if (triangle_shader->CompilationSucceeded())
    {
        triangle_shader->use();
        ImVec4 clearCol = state.clearColour;
        ImVec4 triCol = state.triColour;
        glClearColor(clearCol.x, clearCol.y, clearCol.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        triangle_shader->setVec3Uniform("inColour", triCol.x, triCol.y, triCol.z);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
}

void GLRenderer::CleanUp(){
    glDeleteVertexArrays(1, &VAO);
    if(triangle_shader != nullptr){
        free(triangle_shader);
        triangle_shader = nullptr;
    }
}