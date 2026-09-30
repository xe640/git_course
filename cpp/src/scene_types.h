#ifndef SCENE_TYPES_H
#define SCENE_TYPES_H

#include "../include/GLM/glm.hpp"
#include "../include/GLM/gtc/matrix_transform.hpp"
#include "../include/GLM/gtc/type_ptr.hpp"
#include <cstdint>

#define EPSILON_ELEMENT_UNION 1e-5f

#define PLANE_TYPE_SIZE_VEC4 3
struct plane {
    glm::vec3 position;
    float size;
    glm::vec3 normal;
    float coneSize;
    glm::vec4 colour;
};

inline float cosAPlusB(float cosA, float cosB){
    float sinA, sinB;
    sinA = glm::sqrt(glm::max(0.0f, 1.0f - cosA * cosA));
    sinB = glm::sqrt(glm::max(0.0f, 1.0f - cosB * cosB));
    return cosA * cosB - sinA * sinB;
}

inline plane operator+(const plane& a, const plane& b){
    float areaA = a.size * a.size;
    float areaB = b.size * b.size;
    float total = areaA + areaB + EPSILON_ELEMENT_UNION;
    float coneAreaA = 2.0f * glm::pi<float>() * (1.0f - a.coneSize);
    float coneAreaB = 2.0f * glm::pi<float>() * (1.0f - b.coneSize);

    glm::vec3 weightedNormal = a.normal * areaA * coneAreaA + b.normal * areaB * coneAreaB;
    float weightedLen = glm::length(weightedNormal);
    glm::vec3 midNormal = (weightedLen > 1e-8f) ? (weightedNormal / weightedLen) : a.normal;
    
    float cosExtentA = cosAPlusB(glm::dot(midNormal, a.normal), a.coneSize);
    float cosExtentB = cosAPlusB(glm::dot(midNormal, b.normal), b.coneSize);
    float coneSize = glm::clamp(cosExtentA + cosExtentB, -2.0f, 2.0f) * 0.5f; //average

    return plane {
        (a.position * areaA + b.position * areaB) / total,
        glm::sqrt(total - EPSILON_ELEMENT_UNION),
        midNormal,
        coneSize,
        (a.colour * areaA + b.colour * areaB) / total
    };
}

struct world_element {
    glm::vec3 emission;
    glm::vec3 albedo;
    plane surface;
};


inline world_element operator+(const world_element& a, const world_element& b){
    float areaA = a.surface.size * a.surface.size;
    float areaB = b.surface.size * b.surface.size;
    float total = areaA + areaB + EPSILON_ELEMENT_UNION; // guard against divide by zero
    
    return world_element{
        (a.emission * areaA + b.emission * areaB) / total,
        (a.albedo   * areaA + b.albedo   * areaB) / total,
        a.surface + b.surface
    };
}

struct AABB {
    glm::vec3 min;
    glm::vec3 max;
};

inline AABB operator+(const AABB& a, const AABB&  b) {
    return AABB{glm::min(a.min, b.min), glm::max(a.max, b.max)};
}

struct tree_node{
    int16_t parent = -1; // -1 for null
    int16_t left = -1;
    int16_t right = -1;
    int16_t element = -1;
};

#define MAX_LEAF_NODE_ELEMENTS 128
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
/*
    void remove(int16_t id){

    }
*/
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
/*
    std::vector<plane> getRenderListLod(uint8_t lod){

    }
*/
};

#endif