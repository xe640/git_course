#include "world_storage.h"

std::list<world_node*> WorldStorage::nodes;

void WorldStorage::addToNode(world_node* node, plane surface, world_element element){
    if (node->num_elements == MAX_NODE_ELEMENTS) {
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
    if(nodes.back()->num_elements == MAX_NODE_ELEMENTS){
        nodes.push_back(new world_node());
    }

    addToNode(nodes.back(), surface, element);
}

const std::list<const world_node*>& WorldStorage::getNodes(){
    std::list<const world_node*> view;
    for (world_node* n : nodes) {
        view.push_back(n);
    }
    return view;
}