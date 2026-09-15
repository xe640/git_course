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

    list->planeList->values[list->planeList->numPlanes * 3] = glm::vec4(toAdd.position, toAdd.size);
    list->planeList->values[list->planeList->numPlanes * 3 + 1] = glm::vec4(toAdd.normal, 0.0f);
    list->planeList->values[list->planeList->numPlanes * 3 + 2] = toAdd.colour;
}

void RenderListGen::renderLine(render_list* list, line toAdd){
    if(list == nullptr) {
        return;
    }
    if(list->lineList == nullptr) {
        list->lineList = new line_list();
        list->lineList->numLines = 0;
    }

    list->lineList->values[list->lineList->numLines * 4] = glm::vec4(toAdd.position1, toAdd.size1);
    list->lineList->values[list->lineList->numLines * 4 + 1] = glm::vec4(toAdd.position2, toAdd.size2);
    list->lineList->values[list->lineList->numLines * 4 + 2] = toAdd.colour1;
    list->lineList->values[list->lineList->numLines * 4 + 3] = toAdd.colour2;
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

    if(toAdd.normal.y < 0.5f && toAdd.normal.y > -0.5f) {
        planeRight = glm::normalize(glm::cross(toAdd.normal, glm::vec3(0.0f, 1.0f, 0.0f)));
        planeUp = glm::cross(planeRight, toAdd.normal);
    } else if (glm::abs(toAdd.normal.z) > glm::abs(toAdd.normal.x)) {
        planeUp = glm::normalize(glm::cross(glm::vec3(1.0f, 0.0f , 0.0f), toAdd.normal));
        planeRight = glm::cross(toAdd.normal, planeUp);
    } else {
        planeUp = glm::normalize(glm::cross(glm::vec3(0.0f, 0.0f , 1.0f), toAdd.normal));
        planeRight = glm::cross(toAdd.normal, planeUp);
    }

    glm::vec3 min, max, corner1, corner2;
    max = planeUp + planeRight;
    max *= toAdd.size * 0.5f;
    min = -max;

    corner1 = planeUp - planeRight;
    corner1 *= toAdd.size * 0.5f;
    corner2 = -corner1;

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