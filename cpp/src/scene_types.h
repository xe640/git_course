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
    glm::vec4 colour;
};

struct world_element {
    glm::vec3 emission;
    glm::vec3 albedo;
};

#define MAX_LEAF_NODE_ELEMENTS 64
#define TREE_NODE_COUNT (2 * MAX_LEAF_NODE_ELEMENTS - 1)

struct world_node
{
    world_element elements[TREE_NODE_COUNT];
    plane surfaces[TREE_NODE_COUNT];
    uint8_t num_elements;

    void insert(world_element element, plane surface){

    }

    void remove(uint8_t id){

    }

    uint8_t closestLod(glm::vec3 pos, uint8_t lod){

    }

    std::vector<plane> getRenderListLod(uint8_t lod){

    }
};

#endif
