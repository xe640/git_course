#include "world_storage.h"

std::list<world_node*> WorldStorage::nodes;

void WorldStorage::add_to_node(world_node* node, world_element element){
    if (node->num_elements == MAX_NODE_ELEMENTS) {
        return;
    }

    node->elements[node->num_elements] = element;
    node->num_elements++;
}

void WorldStorage::addElement(world_element element){
    if(nodes.empty()){
        nodes.push_back(new world_node());
    }
    if(nodes.back()->num_elements == MAX_NODE_ELEMENTS){
        nodes.push_back(new world_node());
    }

    add_to_node(nodes.back(), element);
}

const std::list<world_node*>& WorldStorage::getNodes(){
    return nodes;
}

std::list<const world_node*> WorldStorage::getNodesConst(){
    return std::list<const world_node*>(nodes.begin(), nodes.end());
}