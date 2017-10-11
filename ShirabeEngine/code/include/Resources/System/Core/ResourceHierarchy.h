#ifndef __SHIRABE_RESOURCEHIERARCHY_H__
#define __SHIRABE_RESOURCEHIERARCHY_H__

#include "Core/EngineTypeHelper.h"
#include "Resources/System/Core/Handle.h"

namespace Engine {
	namespace Resources {

		struct DependerTreeNode {
			std::vector<DependerTreeNode> children;

			ResourceHandle resourceHandle;
		};
		DeclareListType(DependerTreeNode, DependerTreeNode);
	}
}

#endif