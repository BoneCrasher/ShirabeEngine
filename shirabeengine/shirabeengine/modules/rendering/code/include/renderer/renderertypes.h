#ifndef __SHIRABE_RENDERER_TYPES_H__
#define __SHIRABE_RENDERER_TYPES_H__

#include <stdint.h>
#include <string>

#include <core/basictypes.h>
#include <core/enginetypehelper.h>
#include <core/string.h>

namespace engine
{
    namespace rendering
    {

        using MeshId_t     = uint64_t;
        using MaterialId_t = uint64_t;

        /**
         * The SRenderable struct describes all data for a renderable entity,
         * so that a rendercontext can render it.
         */
        struct SRenderable
        {
        public_members:
            std::string  name;
            MeshId_t     meshId;
            MaterialId_t materialId;
        };
        SHIRABE_DECLARE_LIST_OF_TYPE(SRenderable, Renderable);
    }

    /**
     * Overload to_string to make SRenderables printable.
     */
    template <>
    inline std::string to_string<rendering::SRenderable>(rendering::SRenderable const &aRenderable)
    {

        std::string message =
                CString::format(
                    "  Renderable: %0\n"
                    "    MeshId:     %1\n"
                    "    MaterialId: %2\n",
                    aRenderable.name,
                    aRenderable.meshId,
                    aRenderable.materialId);
        return message;
    }
}

#endif
