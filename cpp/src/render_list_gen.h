#ifndef RENDER_LIST_GEN
#define RENDER_LIST_GEN

#include "scene_types.h"
#include <list>

#define MAX_RENDER_LIST_ELEMENTS 4096
#define MAX_UI_RENDER_LIST_ELEMENTS 512

struct line {
    glm::vec3 position1;
    float size1;
    glm::vec3 position2;
    float size2;
    glm::vec4 colour1;
    glm::vec4 colour2;
};

struct plane_list {
    glm::vec4 values[MAX_RENDER_LIST_ELEMENTS * 3];
    int numPlanes;
};

struct line_list {
    glm::vec4 values[MAX_RENDER_LIST_ELEMENTS * 4];
    int numLines;
};

struct line_list_ui {
    glm::vec4 values[MAX_UI_RENDER_LIST_ELEMENTS * 4];
    int numLines;
};

struct render_list {
    plane_list* planeList;
    line_list* lineList;
};

struct AABB {
    glm::vec3 min;
    glm::vec3 max;
};

class RenderListGen {
public:
    static void clearList(render_list* list);
    static void clearList(line_list_ui* list);
    static void renderPlane(render_list* list, plane toAdd);
    static void renderLine(render_list* list, line toAdd);
    static void renderLine(line_list_ui* list, line toAdd);
    static plane renderPlaneTowards(render_list* list, plane toAdd, glm::vec3 targetPos);
    static void renderPlaneWire(render_list* list, plane toAdd, float lineThickness);
    static void renderPlaneWire(line_list_ui* list, plane toAdd, float lineThickness);
    static void renderAABB(render_list* list, AABB toAdd, glm::vec4 colour, float lineThickness);
    static void generateDemoRenderList(render_list* list);
    static void generateListFromWorld(render_list* list, const std::list<const world_node*>& world_nodes);
    static void addFromUIList(render_list* list, line_list_ui* toAdd);
};
#endif