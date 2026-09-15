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