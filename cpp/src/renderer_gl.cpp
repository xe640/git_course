#include "renderer_gl.h"

Shader* GLRenderer::triangle_shader = nullptr;
Shader* GLRenderer::plane_shader = nullptr;
glm::mat4 GLRenderer::projection_matrix = glm::mat4(1.0f);
glm::mat4 GLRenderer::projection_view_matrix = glm::mat4(1.0f);
GLuint GLRenderer::VAO;
GLuint GLRenderer::scene_data_ubo;
texture_buffer GLRenderer::plane_data_buffer;
render_list* GLRenderer::r_list = nullptr;

void gen_list(render_list* list);

void GLRenderer::Initialize(){
    triangle_shader = new Shader("../shaders/triangleVS.glsl", "../shaders/triangleFS.glsl");
    plane_shader = new Shader("../shaders/planeVS.glsl", "../shaders/planeFS.glsl");
    r_list = new render_list();
    gen_list(r_list);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &scene_data_ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, scene_data_ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, scene_data_ubo);

    gen_tbo(&plane_data_buffer, sizeof(glm::vec4) * 3 * MAX_RENDER_LIST_ELEMENTS);
    triangle_shader->bindUBO("sceneGlobal", 0);
    plane_shader->bindUBO("sceneGlobal", 0);

    glEnable(GL_DEPTH_TEST);
}

void GLRenderer::ReloadShaders(){
    if(triangle_shader != nullptr) {
        triangle_shader->reload();
    }
    if(plane_shader != nullptr) {
        plane_shader->reload();
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

    if (triangle_shader->CompilationSucceeded()) {
        triangle_shader->use();
        ImVec4 clearCol = state.clearColour;
        ImVec4 triCol = state.triColour;
        glClearColor(clearCol.x, clearCol.y, clearCol.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);

        triangle_shader->setVec3Uniform("inColour", triCol.x, triCol.y, triCol.z);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    if(plane_shader->CompilationSucceeded()) {
        glBindBuffer(GL_TEXTURE_BUFFER, plane_data_buffer.tbo);
        glBufferSubData(GL_TEXTURE_BUFFER, 0, sizeof(glm::vec4) * 3 * MAX_RENDER_LIST_ELEMENTS, r_list->planeList->values);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_BUFFER, plane_data_buffer.texture);
        plane_shader->use();
        plane_shader->setTexUniform("instanceData", 0);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, r_list->planeList->numPlanes * 6 - 1);
    }
}

void GLRenderer::CleanUp(){
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &scene_data_ubo);
    if(triangle_shader != nullptr){
        free(triangle_shader);
        triangle_shader = nullptr;
    }
}

void GLRenderer::gen_tbo(texture_buffer* tbuf, int size){
    glGenBuffers(1, &tbuf->tbo);
    glBindBuffer(GL_TEXTURE_BUFFER, tbuf->tbo);
    glBufferData(GL_TEXTURE_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);

    glGenTextures(1, &tbuf->texture);
    glBindTexture(GL_TEXTURE_BUFFER, tbuf->texture);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, tbuf->tbo);
}

void gen_list(render_list* list) {
    for (int i = 0; i < 15; i++)
    {
        glm::vec3 pos = glm::vec3(i - (i % 6) * 2, (i % 3) * 3, i % 7);

        plane testPlane = {
            pos, (float)(i % 5) * 0.2f + 0.2f,
            glm::vec3(0.0f),
            glm::vec4(glm::sin((double)i * 0.2), glm::cos(i), 0.3f, 1.0f)
        };

        RenderListGen::renderPlaneTowards(list, testPlane, glm::vec3(0.0f));
    }
    
}