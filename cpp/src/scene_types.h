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

struct tree_node{
    int16_t parent = -1; // -1 for null
    int16_t left = -1;
    int16_t right = -1;
    int16_t element = -1;
};

#define MAX_LEAF_NODE_ELEMENTS 64
#define TREE_NODE_COUNT (2 * MAX_LEAF_NODE_ELEMENTS - 1)

struct world_node
{
    world_element elements[TREE_NODE_COUNT];
    plane surfaces[TREE_NODE_COUNT];
    tree_node nodes[TREE_NODE_COUNT];
    uint16_t num_nodes;
    int16_t root;

    void insert(world_element element, plane surface){
        if (num_nodes == TREE_NODE_COUNT) {
            return;
        }

        int16_t neighbour = closest(surface.position);
        if (neighbour == -1) {
            num_nodes++;
            elements[0] = element;
            surfaces[0] = surface;
            nodes[0] = {-1, -1, -1, 0};
            root = 0;
            return;
        }
        tree_node nParent = nodes[nodes[neighbour].parent];
    }

    void remove(int16_t id){

    }

    int16_t closest(glm::vec3 pos, uint16_t maxDepth = 65535){
        if (num_nodes == 0) {
            return -1;
        }

        tree_node current = nodes[root];
        int16_t id = root;
        uint16_t depth = 0;

        while (current.left != -1 && current.right != -1 && depth < maxDepth)
        {
            glm::vec3 deltaRight = surfaces[nodes[current.right].element].position - pos;
            glm::vec3 deltaLeft = surfaces[nodes[current.left].element].position - pos;

            id = glm::dot(deltaRight, deltaRight) < glm::dot(deltaLeft, deltaLeft) ? current.right : current.left;
            current =  nodes[id];
            depth++;
        }

        return id;
    }

    std::vector<plane> getRenderListLod(uint8_t lod){

    }
};

#endif
