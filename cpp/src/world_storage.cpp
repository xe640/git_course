#include "world_storage.h"

std::list<world_node*> WorldStorage::nodes;

void WorldStorage::addToNode(world_node* node, plane surface, world_element element){
    if (node->num_elements == MAX_LEAF_NODE_ELEMENTS) {
        return;
    }

    node->elements[node->num_elements] = element;
    node->surfaces[node->num_elements] = surface;
    node->num_elements++;
}

void WorldStorage::addElement(plane surface, world_element element){
    if(nodes.empty()){
        nodes.push_back(new world_node());
    }
    if(nodes.back()->num_elements == MAX_LEAF_NODE_ELEMENTS){
        nodes.push_back(new world_node());
    }

    addToNode(nodes.back(), surface, element);
}

const std::list<world_node*>& WorldStorage::getNodes(){
    return nodes;
}

std::list<const world_node*> WorldStorage::getNodesConst(){
    return std::list<const world_node*>(nodes.begin(), nodes.end());
}