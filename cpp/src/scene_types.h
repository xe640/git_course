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
    if (cosA + cosB < 0.0f) return -1.0f;   // A+B > pi
    float sinA, sinB;
    sinA = glm::sqrt(glm::max(0.0f, 1.0f - cosA * cosA));
    sinB = glm::sqrt(glm::max(0.0f, 1.0f - cosB * cosB));
    return cosA * cosB - sinA * sinB;
}

inline plane operator+(const plane& a, const plane& b){
    float areaA = a.size * a.size;
    float areaB = b.size * b.size;
    float total = areaA + areaB;
    float coneAreaA = glm::max(2.0f * glm::pi<float>() * (1.0f - a.coneSize), EPSILON_ELEMENT_UNION);
    float coneAreaB = glm::max(2.0f * glm::pi<float>() * (1.0f - b.coneSize), EPSILON_ELEMENT_UNION);

    glm::vec3 weightedNormal = a.normal * areaA * coneAreaA + b.normal * areaB * coneAreaB;
    float weightedLen = glm::length(weightedNormal);
    glm::vec3 midNormal = (weightedLen > 1e-8f) ? (weightedNormal / weightedLen) : a.normal;
    
    float cosExtentA = cosAPlusB(glm::dot(midNormal, a.normal), a.coneSize);
    float cosExtentB = cosAPlusB(glm::dot(midNormal, b.normal), b.coneSize);
    float coneSize = glm::clamp(cosExtentA + cosExtentB, -2.0f, 2.0f) * 0.5f; //average

    float areaInverse = 1.0f / glm::max(EPSILON_ELEMENT_UNION, total);

    return plane {
        (a.position * areaA + b.position * areaB) * areaInverse,
        glm::sqrt(total),
        midNormal,
        coneSize,
        (a.colour * areaA + b.colour * areaB) * areaInverse
    };
}

#define WORLD_ELEMENT_FLAG_LEAF_NODE 1
#define WORLD_ELEMENT_FLAG_RIGHT_CHILD 2
#define WORLD_ELEMENT_FLAG_EMPTY 4

struct world_element {
    glm::vec3 emission;
    glm::vec3 albedo;
    float radius;
    plane surface;
    uint16_t treeHeight;
    uint16_t flags = WORLD_ELEMENT_FLAG_EMPTY;

    inline float averageCoverage(){
        float bound = glm::pi<float>() * radius * radius;
        float cover = 1.0f - glm::exp(-surface.size * surface.size / bound);
        return cover;
    }

    inline int leftChild() const {
        return -1;
    }

    inline int rightChild() const {
        return -(1 << treeHeight);
    }

    inline int parent() const {
        return (flags & WORLD_ELEMENT_FLAG_RIGHT_CHILD) ? (1 << (treeHeight + 1)) : 1;
    }

    inline int sibling() const {
        const int s = (1 << (treeHeight + 1)) - 1;
        return (flags & WORLD_ELEMENT_FLAG_RIGHT_CHILD) ? s : -s;
    }

    inline int size() const {
        return -(1 << (treeHeight + 1)) + 1;
    }

    inline void addChild(){
        treeHeight++;
    }

    inline void removeChild(){
        treeHeight--;
    }

    inline bool isLeaf() const {
        return flags & WORLD_ELEMENT_FLAG_LEAF_NODE;
    }

    inline bool isEmpty() const {
        return flags & WORLD_ELEMENT_FLAG_EMPTY;
    }
};


inline world_element operator+(const world_element& a, const world_element& b){
    float areaA = a.surface.size * a.surface.size;
    float areaB = b.surface.size * b.surface.size;
    float total = areaA + areaB;
    float areaInverse = 1.0f / glm::max(EPSILON_ELEMENT_UNION, total);
    
    plane newSurface = a.surface + b.surface;
    float boundingRadius = glm::max(
        glm::distance(a.surface.position, newSurface.position) + a.surface.size,
        glm::distance(b.surface.position, newSurface.position) + b.surface.size
    );

    return world_element{
        (a.emission * areaA + b.emission * areaB) * areaInverse,
        (a.albedo   * areaA + b.albedo   * areaB) * areaInverse,
        boundingRadius,
        newSurface
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

#define MAX_LEAF_NODE_ELEMENTS 512
#define TREE_NODE_COUNT (2 * MAX_LEAF_NODE_ELEMENTS - 1)

struct world_node
{
    world_element elements[TREE_NODE_COUNT];
    uint16_t num_nodes;
    int16_t root;
    
    void insert(world_element element){
        if (num_nodes == 0) {
            elements[0] = element;
            root = 0;
            return;
        }


    }

    void remove(int16_t id){
        if (id == -1 || !elements[id].isLeaf()){
            return;
        }
        elements[id].flags |= WORLD_ELEMENT_FLAG_EMPTY;

        int16_t parent = id + elements[id].parent();
        int16_t sibling = id + elements[id].sibling();
        elements[parent] = elements[sibling];
        elements[parent].addChild();
    }

    int16_t closest(glm::vec3 pos) const{
        if (num_nodes == 0) {
            return -1;
        }

        world_element current = elements[root];
        int32_t id = root;
        uint16_t depth = 0;

        float best = INFINITY;

        while (!current.isLeaf())
        {
            int16_t leftID = id + current.leftChild();
            int16_t rightID = id + current.rightChild();

            glm::vec3 deltaRight = elements[rightID].surface.position - pos;
            glm::vec3 deltaLeft = elements[leftID].surface.position - pos;

            float distR = glm::dot(deltaRight, deltaRight); //squared distance
            float distL = glm::dot(deltaLeft, deltaLeft); //squared distance

            id = distR < distL ? rightID : leftID;
            best = glm::min(distL, distR);

            current = elements[id];
            depth++;
        }

        uint16_t bestID = id;
        id = root;
        best = glm::sqrt(best); //squared distance to actual distance

        while(id > -1){
            const world_element& node = elements[id];
            float dist = glm::dot(node.surface.position - pos, node.surface.position - pos);
            if (!node.isEmpty() && dist < (best + node.radius) * (best + node.radius)) { //check if node may contain better candidates
                if (node.isLeaf()) {
                    if (dist < best * best) {best = glm::sqrt(dist); bestID = id;}
                    id += node.size(); //skip empty space after leaf
                } else {
                    id += node.leftChild();
                }
            } else {
                id += node.size(); //prune subtree
            }
        }

        return bestID;
    }
/*
    std::vector<plane> getRenderListLod(uint8_t lod) const{

    }
*/
};

#endif