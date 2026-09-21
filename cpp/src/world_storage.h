#ifndef WORLD_STORAGE
#define WORLD_STORAGE

#include "scene_types.h"
#include <list>

class WorldStorage{
private:
    static std::list<world_node*> nodes;
    static void addToNode(world_node* node, plane surface, world_element element);
public:
    static void addElement(plane surface, world_element element);
    static const std::list<world_node*>& getNodes();
    static std::list<const world_node*> getNodesConst();
};

#endif