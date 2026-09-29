#include "renderer_gl.h"

Shader* GLRenderer::triangle_shader = nullptr;
Shader* GLRenderer::plane_shader = nullptr;
Shader* GLRenderer::line_shader = nullptr;
GLuint GLRenderer::VAO;
GLuint GLRenderer::scene_data_ubo;
texture_buffer GLRenderer::plane_data_buffer1;
texture_buffer GLRenderer::line_data_buffer1;
texture_buffer GLRenderer::plane_data_buffer2;
texture_buffer GLRenderer::line_data_buffer2;
bool GLRenderer::active_tbo_set = false;
scene_global_data GLRenderer::global_data = {glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f)};
render_list* GLRenderer::r_list = nullptr;
bool GLRenderer::demo_is_generated = false;

void GLRenderer::Initialize(){
    triangle_shader = new Shader("../shaders/triangleVS.glsl", "../shaders/triangleFS.glsl");
    plane_shader = new Shader("../shaders/planeVS.glsl", "../shaders/planeFS.glsl");
    line_shader = new Shader("../shaders/lineVS.glsl", "../shaders/lineFS.glsl");
    r_list = new render_list();

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &scene_data_ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, scene_data_ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 3 + sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, scene_data_ubo);

    gen_tbo(&plane_data_buffer1, sizeof(glm::vec4) * 3 * MAX_RENDER_LIST_ELEMENTS);
    gen_tbo(&line_data_buffer1, sizeof(glm::vec4) * 4 * MAX_RENDER_LIST_ELEMENTS);
    gen_tbo(&plane_data_buffer2, sizeof(glm::vec4) * 3 * MAX_RENDER_LIST_ELEMENTS);
    gen_tbo(&line_data_buffer2, sizeof(glm::vec4) * 4 * MAX_RENDER_LIST_ELEMENTS);

    RenderListGen::generateDemoRenderList(r_list);
    fill_tbo(plane_data_buffer1, r_list->planeList->numPlanes * 3, r_list->planeList->values);
    fill_tbo(line_data_buffer1, r_list->lineList->numLines * 4, r_list->lineList->values);
    fill_tbo(plane_data_buffer2, r_list->planeList->numPlanes * 3, r_list->planeList->values);
    fill_tbo(line_data_buffer2, r_list->lineList->numLines * 4, r_list->lineList->values);

    triangle_shader->bindUBO("sceneGlobal", 0);
    plane_shader->bindUBO("sceneGlobal", 0);
    line_shader->bindUBO("sceneGlobal", 0);

    glEnable(GL_DEPTH_TEST);
}

void GLRenderer::ReloadShaders(){
    if(triangle_shader != nullptr) {
        triangle_shader->reload();
    }
    if(plane_shader != nullptr) {
        plane_shader->reload();
    }
    if(line_shader != nullptr){
        line_shader->reload();
    }
}

void GLRenderer::Render(gui_data state, glm::mat4(*getViewMat)()){
    glBindVertexArray(VAO);

    if(state.windowChanged) {
        global_data.projectionMatrix = glm::perspective(
            glm::radians(state.fov),
            state.windowAspect.x/state.windowAspect.y,
        0.01f, 200.0f);
    }

    if(state.cameraChanged || state.windowChanged) {
        global_data.viewMatrix = getViewMat();
        global_data.projectionViewMatrix = global_data.projectionMatrix * global_data.viewMatrix;
        glm::vec3 camPos = state.camPos;
        glBindBuffer(GL_UNIFORM_BUFFER, scene_data_ubo);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(global_data.projectionViewMatrix));
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(global_data.viewMatrix));
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, sizeof(glm::mat4), glm::value_ptr(global_data.projectionMatrix));
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 3, sizeof(glm::vec3), glm::value_ptr(camPos));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
    
    if (triangle_shader->CompilationSucceeded()) {
        triangle_shader->use();
        ImVec4 clearCol = state.clearColour;
        ImVec4 triCol = state.triColour;
        glClearColor(clearCol.x, clearCol.y, clearCol.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        triangle_shader->setVec3Uniform("inColour", triCol.x, triCol.y, triCol.z);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    if (active_tbo_set) {
        fill_tbo(plane_data_buffer1, r_list->planeList->numPlanes * 3, r_list->planeList->values);
        fill_tbo(line_data_buffer1, r_list->lineList->numLines * 4, r_list->lineList->values);
    } else {
        fill_tbo(plane_data_buffer2, r_list->planeList->numPlanes * 3, r_list->planeList->values);
        fill_tbo(line_data_buffer2, r_list->lineList->numLines * 4, r_list->lineList->values);
    }
    
    if(plane_shader->CompilationSucceeded() && r_list->planeList != nullptr && r_list->planeList->numPlanes > 0) {
        if (active_tbo_set) {
            bind_tbo(plane_data_buffer2, GL_TEXTURE0);
        } else {
            bind_tbo(plane_data_buffer1, GL_TEXTURE0);
        }
        plane_shader->use();
        plane_shader->setTexUniform("instanceData", 0);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, r_list->planeList->numPlanes * 6 - 1);
    }
    
    if(line_shader->CompilationSucceeded() && r_list->lineList != nullptr && r_list->lineList->numLines > 0) {
        if (active_tbo_set) {
            bind_tbo(line_data_buffer2, GL_TEXTURE0);
        } else {
            bind_tbo(line_data_buffer1, GL_TEXTURE0);
        }
        line_shader->use();
        line_shader->setTexUniform("instanceData", 0);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, r_list->lineList->numLines * 10 - 1);
    }

    active_tbo_set = !active_tbo_set;
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

void GLRenderer::fill_tbo(texture_buffer tbuf, int size, const GLvoid *data){
    glBindBuffer(GL_TEXTURE_BUFFER, tbuf.tbo);
    glBufferSubData(GL_TEXTURE_BUFFER, 0, sizeof(glm::vec4) * size, data);
}

void GLRenderer::bind_tbo(texture_buffer tbuf, GLint textureSlot){
    glActiveTexture(textureSlot);
    glBindTexture(GL_TEXTURE_BUFFER, tbuf.texture);
}

void GLRenderer::updateRenderList(const std::list<const world_node*>& world_nodes, gui_data state){
    if(state.demoRender) {
        if (!demo_is_generated) {
            demo_is_generated = true;
            RenderListGen::generateDemoRenderList(r_list);
        }
    } else {
        demo_is_generated = false;
        RenderListGen::generateListFromWorld(r_list, world_nodes);
        RenderListGen::addFromUIList(r_list, state.ui_lines);
    }
}