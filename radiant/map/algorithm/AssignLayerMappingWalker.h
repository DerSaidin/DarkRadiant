#pragma once

#include "scenelib.h"
#include "imodel.h"

#include "../InfoFile.h"

namespace map
{

class AssignLayerMappingWalker :
    public scene::NodeVisitor
{
private:
    InfoFile& _infoFile;

public:
    AssignLayerMappingWalker(InfoFile& infoFile) :
        _infoFile(infoFile)
    {}

    virtual ~AssignLayerMappingWalker() {}

    bool pre(const scene::INodePtr& node)
    {
        if (Node_isModel(node) || particles::isParticleNode(node))
        {
            // We have a model or particle, assign the layers of the parent
            scene::INodePtr parent = node->getParent();

            if (parent != NULL)
            {
                assignNodeToLayers(node, parent->getLayers());
            }

            return true;
        }

        // Retrieve the next set of layer mappings and assign them
        assignNodeToLayers(node, _infoFile.getNextLayerMapping());
        return true;
    }
};

} // namespace
