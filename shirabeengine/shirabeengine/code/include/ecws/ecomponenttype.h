#ifndef __SHIRABE_COMPONENT_TYPE__
#define __SHIRABE_COMPONENT_TYPE__

#include <stdint.h>

namespace engine
{
    enum class EComponentType
            : uint8_t
    {
        UNKNOWN = 0
        , STATIC_MESH
        , SKINNED_MESH
        , MATERIAL
    };
}

#endif
