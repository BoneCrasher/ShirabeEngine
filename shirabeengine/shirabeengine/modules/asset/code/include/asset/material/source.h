#ifndef __SHIRABE_MATERIAL_SOURCE_H__
#define __SHIRABE_MATERIAL_SOURCE_H__

#include <cassert>
#include <string>
#include <vector>
#include <cstdint>
#include <filesystem>
#include <cstring>
#include <unordered_map>

#include <platform/platform.h>

#include <vulkan/vulkan.h>

#include <base/declaration.h>
#include <base/stl_container_helpers.h>
#include <core/enginestatus.h>
#include <core/bitfield.h>
#include <core/serialization/serialization.h>
#include <asset/assettypes.h>

#include "asset/material/asset.h"

namespace engine
{
    namespace documents
    {
        template <typename T>
        class IJSONSerializer;

        template <typename T>
        class IJSONDeserializer;
    }

    namespace material
    {
        /**
         * Describes the stage of the shader to compile.
         */
        enum class EShaderStage
        {
            NotApplicable = 0,
            Vertex,
            TesselationControlPoint,
            TesselationEvaluation,
            Geometry,
            Fragment,
            Compute
        };

        /**
         * The SMaterialIndexStage struct describes individual stage's file
         * references.
         */
        struct SMaterialIndexStage
        {
            std::filesystem::path glslSourceFilename;
        };

        /**
         * The SMaterialMetaStage struct describes individual module's file
         * references.
         */
        struct SMaterialMetaStage
        {
            asset::AssetId_t spvModuleAssetId;
        };

        /**
         * The SMaterialIndex describes all necessary data for a basic material composition
         * in the engine.
         * The specific implementation will be provided in files referenced by the stages member.
         */
        struct SMaterialMasterIndex
                : engine::serialization::ISerializable<documents::IJSONSerializer<SMaterialMasterIndex>>
                , engine::serialization::IDeserializable<documents::IJSONDeserializer<SMaterialMasterIndex>>
        {
            static std::unordered_map<VkPipelineStageFlagBits, SMaterialIndexStage> const sEmptyMasterMap;

        public_constructors:
            SHIRABE_INLINE
            SMaterialMasterIndex()
                : serialization::ISerializable<documents::IJSONSerializer<SMaterialMasterIndex>>()
                , serialization::IDeserializable<documents::IJSONDeserializer<SMaterialMasterIndex>>()
                , uid                  (0 )
                , name                 ({})
                , stages(sEmptyMasterMap)
            {}

            SHIRABE_INLINE
            SMaterialMasterIndex(SMaterialMasterIndex const &aOther)
                : serialization::ISerializable<documents::IJSONSerializer<SMaterialMasterIndex>>()
                , serialization::IDeserializable<documents::IJSONDeserializer<SMaterialMasterIndex>>()
                , uid                  (aOther.uid                  )
                , name                 (aOther.name                 )
                , stages               (aOther.stages               )
            {}

            SHIRABE_INLINE
            SMaterialMasterIndex(SMaterialMasterIndex &&aOther) noexcept
                : serialization::ISerializable<documents::IJSONSerializer<SMaterialMasterIndex>>()
                , serialization::IDeserializable<documents::IJSONDeserializer<SMaterialMasterIndex>>()
                , uid                  (aOther.uid                  )
                , name                 (std::move(aOther.name      ))
                , stages               (std::move(aOther.stages    ))
            {}

        public_operators:
            SHIRABE_INLINE
            SMaterialMasterIndex &operator=(SMaterialMasterIndex const &aOther)
            {
                uid                   = aOther.uid;
                name                  = aOther.name;
                stages                = aOther.stages;

                return (*this);
            }

            SHIRABE_INLINE
            SMaterialMasterIndex &operator=(SMaterialMasterIndex &&aOther) noexcept
            {
                uid                   = aOther.uid;
                name                  = std::move(aOther.name);
                stages                = std::move(aOther.stages);

                return (*this);
            }

        public_members:
            uint64_t                                                         uid;
            std::string                                                      name;
            std::unordered_map<VkPipelineStageFlagBits, SMaterialIndexStage> stages;

        public_methods:
            /**
             * @brief acceptSerializer
             * @param aSerializer
             * @return
             */
            bool acceptSerializer(documents::IJSONSerializer<SMaterialMasterIndex> &aSerializer) const final;

            /**
             * @brief acceptDeserializer
             * @param aSerializer
             * @return
             */
            bool acceptDeserializer(documents::IJSONDeserializer<SMaterialMasterIndex> &aDeserializer) final;
        };
    }
}

#endif // MATERIAL_DECLARATION_H
