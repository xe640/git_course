#ifndef SCENE_TYPES_H
#define SCENE_TYPES_H

#include "../include/GLM/glm.hpp"
#include "../include/GLM/gtc/matrix_transform.hpp"
#include "../include/GLM/gtc/type_ptr.hpp"
#include <cstdint>

struct plane {
    glm::vec3 position;
    float size;
    glm::vec3 normal;
    float coneSize;
    glm::vec4 colour;
};

struct world_element {
    glm::vec3 emission;
    glm::vec3 albedo;
};

#define MAX_NODE_ELEMENTS 64

struct world_node
{
    world_element elements[MAX_NODE_ELEMENTS];
    plane surfaces[MAX_NODE_ELEMENTS];
    uint8_t num_elements;
};

#endif
