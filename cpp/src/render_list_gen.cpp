#include "render_list_gen.h"

void RenderListGen::clearList(render_list* list){
    if(list == nullptr) {
        return;
    }
    if (list->lineList != nullptr) {
        list->lineList->numLines = 0;
    }
    if(list->planeList != nullptr) {
        list->planeList->numPlanes = 0;
    }
}

void RenderListGen::renderPlane(render_list* list, plane toAdd){
    if(list == nullptr) { //this check can be removed if list building is slow
        return;
    }
    if(list->planeList == nullptr) {
        list->planeList = new plane_list();
        list->planeList->numPlanes = 0;
    }
    if(list->planeList->numPlanes == MAX_RENDER_LIST_ELEMENTS){
        return;
    }

    list->planeList->values[list->planeList->numPlanes * 3] = glm::vec4(toAdd.position, toAdd.size);
    list->planeList->values[list->planeList->numPlanes * 3 + 1] = glm::vec4(toAdd.normal, toAdd.coneSize);
    list->planeList->values[list->planeList->numPlanes * 3 + 2] = toAdd.colour;
    list->planeList->numPlanes++;
}

void RenderListGen::renderLine(render_list* list, line toAdd){
    if(list == nullptr) {
        return;
    }
    if(list->lineList == nullptr) {
        list->lineList = new line_list();
        list->lineList->numLines = 0;
    }
    if(list->planeList->numPlanes == MAX_RENDER_LIST_ELEMENTS){
        return;
    }

    list->lineList->values[list->lineList->numLines * 4] = glm::vec4(toAdd.position1, toAdd.size1);
    list->lineList->values[list->lineList->numLines * 4 + 1] = glm::vec4(toAdd.position2, toAdd.size2);
    list->lineList->values[list->lineList->numLines * 4 + 2] = toAdd.colour1;
    list->lineList->values[list->lineList->numLines * 4 + 3] = toAdd.colour2;
    list->lineList->numLines++;
}

plane RenderListGen::renderPlaneTowards(render_list* list, plane toAdd, glm::vec3 targetPos){
    toAdd.normal = glm::normalize(targetPos - toAdd.position);
    if(list != nullptr) {
        renderPlane(list, toAdd);
    }
    return toAdd;
}

void RenderListGen::renderPlaneWire(render_list* list, plane toAdd, float lineThickness){
    glm::vec3 planeUp, planeRight;

    if(glm::abs(toAdd.normal.y) > 0.9f ) {
        planeRight = glm::normalize(glm::cross(toAdd.normal, glm::vec3(0.0f, 1.0f, 0.0f)));
        planeUp = glm::cross(planeRight, toAdd.normal);
    } else {
        planeUp = glm::normalize(glm::cross(glm::vec3(0.0f, 0.0f , 1.0f), toAdd.normal));
        planeRight = glm::cross(toAdd.normal, planeUp);
    }

    glm::vec3 min, max, corner1, corner2;
    max = planeUp + planeRight;
    max *= toAdd.size * 0.5f;
    min = -max;
    min += toAdd.position;
    max += toAdd.position;

    corner1 = planeUp - planeRight;
    corner1 *= toAdd.size * 0.5f;
    corner2 = -corner1;
    corner1 += toAdd.position;
    corner2 += toAdd.position;

    glm::vec4 col = toAdd.colour;
    renderLine(list, {
        min, lineThickness,
        corner1, lineThickness,
        col, col
    });
    renderLine(list, {
        corner1, lineThickness,
        max, lineThickness,
        col, col
    });
    renderLine(list, {
        max, lineThickness,
        corner2, lineThickness,
        col, col
    });
    renderLine(list, {
        corner2, lineThickness,
        min, lineThickness,
        col, col
    });
}

uint32_t pcg_hash(uint32_t input) //from https://www.reedbeta.com/blog/hash-functions-for-gpu-rendering/
{
    uint32_t state = input * 747796405u + 2891336453u;
    uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

float rand_f(uint32_t* state) {
    *state = pcg_hash(*state);
    return (float)*state / 134217728.0f - 16.0f;
}

glm::vec3 rand_vec3(uint32_t* state){
    float x, y, z;
    x = rand_f(state);
    y = rand_f(state);
    z = rand_f(state);

    return glm::vec3(x, y, z);
}

void RenderListGen::generateDemoRenderList(render_list* list) { // random generation test code for debugging purposes
    if (list == nullptr) {
        return;
    }

    clearList(list);
    uint32_t rand = pcg_hash(82187u);

    for (int i = 0; i < 15; i++)
    {
        glm::vec3 pos = glm::vec3(i - (i % 6) * 2, (i % 3) * 3, i % 7);

        plane testPlane = {
            pos, (float)(i % 5) * 0.2f + 0.2f,
            glm::vec3(0.0f),
            rand_f(&rand) * 0.015625f + 0.75f,
            glm::vec4(glm::sin((double)i * 0.2), glm::cos(i), 0.3f, 1.0f)
        };

        if (i < 7) {
            RenderListGen::renderPlaneTowards(list, testPlane, glm::vec3(0.0f));
        } else {
            RenderListGen::renderPlaneWire(list, RenderListGen::renderPlaneTowards(nullptr, testPlane, glm::vec3(0.0f)), 0.1f);
        }
    }

    for (int32_t i = 0; i < 30; i++)
    {
        glm::vec3 pos1, pos2;
        float s1, s2;
        glm::vec4 col1, col2;

        pos1 = rand_vec3(&rand);
        pos2 = rand_vec3(&rand);
        s1 = rand_f(&rand) * 0.0312f + 0.5f;
        s2 = rand_f(&rand) * 0.0312f + 0.5f;
        col1 = glm::vec4(rand_vec3(&rand) * 0.0312f + 0.5f, 1.0);
        col2 = glm::vec4(rand_vec3(&rand) * 0.0312f + 0.5f, 1.0);
        
        line testLine = {
            pos1, s1,
            pos2, s2,
            col1, col2
        };

        RenderListGen::renderLine(list, testLine);
    }
}

void RenderListGen::generateListFromWorld(render_list* list, const std::list<const world_node*>& world_nodes){
    RenderListGen::clearList(list);
    for(const world_node* node : world_nodes){
        for(uint8_t i = 0; i < node->num_elements; i++){
            plane toAdd = node->surfaces[i];
            toAdd.colour = glm::vec4(node->elements[i].emission, 1.0);
            renderPlane(list, toAdd);
        }
    }
}