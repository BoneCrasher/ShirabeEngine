#ifndef __SHIRABE_RENDERER_TYPES_H__
#define __SHIRABE_RENDERER_TYPES_H__

#include <cstdint>
#include <string>

#include <core/basictypes.h>
#include <core/enginetypehelper.h>
#include <base/string.h>
#include <asset/assettypes.h>

namespace engine
{
    namespace rendering
    {
        /**
         * The SRenderable struct describes all data for a renderable entity,
         * so that a rendercontext can render it.
         */
        struct SRenderable
        {
        public_members:
            std::string     name;
            std::string     meshInstanceId;
            asset::AssetId_t meshInstanceAssetId;
            std::string      materialInstanceId;
            asset::AssetId_t materialInstanceAssetId;
        };
        SHIRABE_DECLARE_LIST_OF_TYPE(SRenderable, Renderable)
    }

    /**
     * Overload convert_to_string to make SRenderables printable.
     *
     * @param aRenderable The renderable to convert to string.
     */
    template <>
    inline std::string convert_to_string<rendering::SRenderable>(rendering::SRenderable const &aRenderable)
    {

        std::string message =
                StaticStringHelpers::format(
                    "  Renderable: {}\n"
                    "    MeshId:     {}\n"
                    "    MaterialId: {}\n",
                    aRenderable.name,
                    aRenderable.meshInstanceId,
                    aRenderable.materialInstanceId);
        return message;
    }
}

#endif
