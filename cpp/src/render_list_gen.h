#include "../include/GLM/glm.hpp"
#include "../include/GLM/gtc/matrix_transform.hpp"
#include "../include/GLM/gtc/type_ptr.hpp"

#define MAX_LIST_ELEMENTS 4096

struct plane {
    glm::vec3 position;
    float size;
    glm::vec3 normal;
    glm::vec4 colour;
};

struct line {
    glm::vec3 position1;
    float size1;
    glm::vec3 position2;
    float size2;
    glm::vec4 colour1;
    glm::vec4 colour2;
};

struct plane_list {
    glm::vec4 values[MAX_LIST_ELEMENTS * 3];
    int numPlanes;
};

struct line_list {
    glm::vec4 values[MAX_LIST_ELEMENTS * 4];
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
    static void renderPlane(render_list* list, plane toAdd);
    static void renderLine(render_list* list, line toAdd);
    static plane renderPlaneTowards(render_list* list, plane toAdd, glm::vec3 targetPos);
    static void renderPlaneWire(render_list* list, plane toAdd, float lineThickness);
    static void renderAABB(render_list* list, AABB toAdd, glm::vec4 colour, float lineThickness);
};
