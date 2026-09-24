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

plane& operator+(plane& a, plane& b){
    float areaA = a.size * a.size;
    float areaB = b.size * b.size;
    float total = areaA + areaB;
    plane res = {
        (a.position * areaA + b.position * areaB) / total,
        glm::sqrt(total),
        (a.normal * a.coneSize * areaA + b.normal * b.coneSize * areaB) / total,
        0.0f,
        (a.colour * areaA + b.colour * areaB) / total
    };

    float cosConeHalfAngle = glm::length(res.normal);
    res.normal /= cosConeHalfAngle;
    res.coneSize = cosConeHalfAngle;
    return res;
}

struct world_element {
    glm::vec3 emission;
    glm::vec3 albedo;
};

#define MAX_NODE_ELEMENTS 128

struct world_node
{
    world_element elements[MAX_NODE_ELEMENTS];
    plane surfaces[MAX_NODE_ELEMENTS];
    uint8_t num_elements;
};

#endif
