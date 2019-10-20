//
// Created by dottideveloper on 20.10.19.
//

#ifndef SHIRABEDEVELOPMENT_RESOURCETYPES_H
#define SHIRABEDEVELOPMENT_RESOURCETYPES_H

#include <vulkan/vulkan.h>
#include <core/enginetypehelper.h>
#include <core/databuffer.h>

namespace engine
{
    namespace resources
    {

        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SBufferDescription
        {
            std::string                       name;
            VkBufferCreateInfo                createInfo;
            std::vector<DataSourceAccessor_t> initialData;
        };

    }
}

#endif //SHIRABEDEVELOPMENT_RESOURCETYPES_H
