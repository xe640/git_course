#ifndef WORLD_STORAGE
#define WORLD_STORAGE
#include "../include/GLM/glm.hpp"
#include "../include/GLM/gtc/matrix_transform.hpp"
#include "../include/GLM/gtc/type_ptr.hpp"
#include "render_list_gen.h"
#include <list>

#define MAX_NODE_ELEMENTS 64

struct world_element {
    glm::vec3 emission;
    glm::vec3 albedo;
};

struct world_node
{
    world_element elements[MAX_NODE_ELEMENTS];
    plane surfaces[MAX_NODE_ELEMENTS];
    uint8_t num_elements;
};


class WorldStorage{
private:
    static std::list<world_node*> nodes;
    static void addToNode(world_node* node, plane surface, world_element element);
public:
    static void addElement(plane surface, world_element element);
};

#endif