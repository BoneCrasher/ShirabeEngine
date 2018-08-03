#ifndef __SHIRABE_RESOURCES_DTO_H__
#define __SHIRABE_RESOURCES_DTO_H__

#include <stdint.h>
#include <vector>
#include <map>
#include <string>

#include "Core/EngineTypeHelper.h"

namespace engine {
  namespace Resources {

    typedef std::string PublicResourceId_t;

    using PublicResourceIdList = std::vector<PublicResourceId_t>;

    template <typename TValue>
    using PublicResourceIdMapTo = std::map<PublicResourceId_t, TValue>;

    using SubjacentResourceId_t = uint64_t;
    DeclareListType(SubjacentResourceId_t, SubjacentResourceId);
  }
}

#endif