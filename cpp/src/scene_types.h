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

inline float cosAPlusB(float cosA, float cosB){
    float sinA, sinB;
    sinA = glm::sqrt(glm::max(0.0f, 1.0f - cosA * cosA));
    sinB = glm::sqrt(glm::max(0.0f, 1.0f - cosB * cosB));
    return cosA * cosB - sinA * sinB;
}

inline plane operator+(const plane& a, const plane& b){
    float areaA = a.size * a.size;
    float areaB = b.size * b.size;
    float total = areaA + areaB + 0.00001f;
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
        glm::sqrt(total - 0.00001f),
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
    float total = areaA + areaB + 0.00001f; // guard against divide by zero
    
    return world_element{
        (a.emission * areaA + b.emission * areaB) / total,
        (a.albedo   * areaA + b.albedo   * areaB) / total,
        a.surface + b.surface
    };
}

#define MAX_NODE_ELEMENTS 128

struct world_node
{
    world_element elements[MAX_NODE_ELEMENTS];
    uint8_t num_elements;
};

#endif