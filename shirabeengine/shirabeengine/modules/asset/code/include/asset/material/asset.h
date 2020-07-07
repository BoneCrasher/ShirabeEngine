#ifndef __SHIRABE_MATERIAL_ASSET_H__
#define __SHIRABE_MATERIAL_ASSET_H__

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
        using MutableMaterialTypeMap_t = std::unordered_map<std::string, Shared<struct SMaterialType>>;
        using MaterialTypeMap_t        = std::unordered_map<std::string, Shared<struct SMaterialType const>>;

        /**
         * The SMaterialType struct describes a data type of a SPIR-V module and it's memory properties.
         */
        struct SMaterialType
        {
            uint32_t          byteSize;
            uint32_t          vectorSize;
            uint32_t          matrixRows;
            uint32_t          matrixColumns;
            uint32_t          matrixColumnStride;
            uint32_t          arraySize;
            uint32_t          arrayStride;
            std::string       name;
            MaterialTypeMap_t members;
        };

        /**
         * A named resource is any resource with a name assigned.
         */
        struct SNamedResource
        {
            std::string name;
        };

        /**
         * Describes a shader stage input by it's name and explicit location.
         */
        struct SStageInput
                : public SNamedResource
        {
            uint32_t                    location;
            Shared<SMaterialType const> type;
        };

        /**
         * Describes a shader stage input by it's name and explicit location.
         */
        struct SStageOutput
                : public SNamedResource
        {
            uint32_t                    location;
            Shared<SMaterialType const> type;
        };

        /**
         * A bound resource is any resource, which is part of a set and has an assigned binding.
         */
        struct SBoundResource
                : public SNamedResource
        {
            uint32_t set;
            uint32_t binding;
        };

        /**
         * Describes a fragment shader subpass input. Will be empty for all other shader types.
         */
        struct SSubpassInput
                : public SBoundResource
        {
            uint32_t attachmentIndex;
        };

        /**
         * The SBufferLocation struct describes a specific location within a data buffer used to read
         * or write the respective value.
         */
        struct SBufferLocation
        {
            uint64_t offset;
            uint64_t length;
            uint64_t padding;

            SHIRABE_INLINE
            bool equals(SBufferLocation const &aOther) const
            {
                bool const offsetEq  = (offset  == aOther.offset);
                bool const lengthEq  = (length  == aOther.length);
                bool const paddingEq = (padding == aOther.padding);

                return (offsetEq and lengthEq and paddingEq);
            }

            SHIRABE_INLINE
            bool overlapsWith(SBufferLocation const &aOther) const
            {
                // Consider:
                //
                // I)          x1 <= C <= x2
                // II)         y1 <= C <= y2
                // -------------------------
                // III) x1 <= y2 && y1 <= x2

                uint64_t const x1 = (offset);
                uint64_t const x2 = (offset + length + padding);
                uint64_t const y1 = (aOther.offset);
                uint64_t const y2 = (aOther.offset + aOther.length + aOther.padding);

                bool const overlap = (x1 <= y2 && y1 <= x2);
                return overlap;
            }
        };

        struct SBufferArray
        {
            uint64_t layers;
            uint64_t stride;
        };

        /**
         * Describes a single uniform buffer member by name and buffer location.
         */
        using MutableBufferMemberMap_t = std::unordered_map<std::string, Shared<struct SBufferMember>>;
        using BufferMemberMap_t        = std::unordered_map<std::string, Shared<struct SBufferMember>>;
        struct SBufferMember
        {
            std::string       name;
            SBufferLocation   location;
            SBufferArray      array;
            // SMaterialType     baseType;
            // SMaterialType     type;
            BufferMemberMap_t members;
        };

        struct SMaterialBuffer
                : public SBoundResource
        {
        public_members:
            SBufferLocation   location;
            SBufferArray      array;
            // SMaterialType     baseType;
            // SMaterialType     type;
            BufferMemberMap_t members;

        public_constructors:
            SMaterialBuffer() = default;

            SHIRABE_INLINE
            SMaterialBuffer(SMaterialBuffer const &aOther)
                    : SBoundResource(aOther)
                      , location(aOther.location)
                      , members (aOther.members )
                      , array   (aOther.array   )
            {}

        public_operators:
            SHIRABE_INLINE
            SMaterialBuffer &operator=(SMaterialBuffer const &aOther)
            {
                name     = aOther.name;
                location = aOther.location;
                set      = aOther.set;
                binding  = aOther.binding;
                members  = aOther.members;
                array    = aOther.array;

                return (*this);
            }
        };

        /**
         * Describes a uniform buffer, it's name, location, set and binding as well
         * as a collection of buffer members.
         */
        struct SUniformBuffer
                : public SMaterialBuffer
        {
        public_members:
            core::CBitField<VkPipelineStageFlagBits> stageBinding;

        public_constructors:
            SUniformBuffer() = default;

            SHIRABE_INLINE
            SUniformBuffer(SUniformBuffer const &aOther)
                    : SMaterialBuffer(aOther)
                      , stageBinding(aOther.stageBinding)
            {}

        public_operators:
            SHIRABE_INLINE
            SUniformBuffer &operator=(SUniformBuffer const &aOther)
            {
                SMaterialBuffer::operator=(aOther);
                stageBinding = aOther.stageBinding;

                return (*this);
            }
        };

        /**
         * Describes a sampled image in a GLSL shader, as well as its set and binding.
         */
        struct SSampledImage
                : public SBoundResource
        {
            core::CBitField<VkPipelineStageFlagBits> stageBinding;
        };

        /**
         * Describes a single shader stage including all it's inputs, outputs and resources.
         */
        struct SMaterialStage
        {
        public_members:
            VkPipelineStageFlagBits     stage;
            std::string                 stageName;
            std::string                 filename;
            std::vector<SStageInput>    inputs;
            std::vector<SStageOutput>   outputs;

        public_constructors:
            SMaterialStage() = default;

            SMaterialStage(SMaterialStage const &aOther) = default;

            SHIRABE_INLINE
            SMaterialStage(SMaterialStage &&aOther) noexcept = default;

        public_operators:
            SHIRABE_INLINE
            SMaterialStage &operator=(SMaterialStage const &aOther)
            {
                stage     = aOther.stage;
                stageName = aOther.stageName;
                filename  = aOther.filename;
                inputs    = aOther.inputs;
                outputs   = aOther.outputs;

                return (*this);
            }

            SHIRABE_INLINE
            SMaterialStage &operator=(SMaterialStage &&aOther)
            {
                stage     = aOther.stage;
                stageName = std::move(aOther.stageName);
                filename  = std::move(aOther.filename);
                inputs    = std::move(aOther.inputs);
                outputs   = std::move(aOther.outputs);

                return (*this);
            }
        };
        using StageMap_t = std::unordered_map<VkPipelineStageFlagBits, SMaterialStage>;

        /**
         * This struct stores basic layout structural information
         * to ease recreation of materials.
         */
        struct SMaterialLayoutInfo
        {
        public_members:
            uint32_t              setCount;
            std::vector<uint32_t> setBindingCount;
        };

        /**
         * The SMaterialIndex describes all necessary data for a basic material composition
         * in the engine.
         * The specific implementation will be provided in files referenced by the stages member.
         */
        struct SMaterialAsset
                : engine::serialization::ISerializable<documents::IJSONSerializer<SMaterialAsset>>
                , engine::serialization::IDeserializable<documents::IJSONDeserializer<SMaterialAsset>>
        {
            static std::unordered_map<VkPipelineStageFlagBits, SMaterialStage> const sEmptyMetaMap;

        public_constructors:
            SHIRABE_INLINE
            SMaterialAsset()
                    : serialization::ISerializable<documents::IJSONSerializer<SMaterialAsset>>()
                    , serialization::IDeserializable<documents::IJSONDeserializer<SMaterialAsset>>()
                    , uid                  (0 )
                    , name                 ({})
                    , signatureAssetUid    (0 )
                    , configurationAssetUid(0 )
                    , layoutInfo           ({})
                    , stages               (sEmptyMetaMap)
                    , uniformBuffers       ({})
                    , storageBuffers       ({})
                    , sampledImages        ({})
                    , subpassInputs        ({})
            {}

            SHIRABE_INLINE
            SMaterialAsset(SMaterialAsset const &aOther)
                    : serialization::ISerializable<documents::IJSONSerializer<SMaterialAsset>>()
                    , serialization::IDeserializable<documents::IJSONDeserializer<SMaterialAsset>>()
                    , uid                  (aOther.uid                  )
                    , name                 (aOther.name                 )
                    , signatureAssetUid    (aOther.signatureAssetUid    )
                    , configurationAssetUid(aOther.configurationAssetUid)
                    , layoutInfo           (aOther.layoutInfo)
                    , stages               (aOther.stages               )
                    , uniformBuffers       (aOther.uniformBuffers)
                    , storageBuffers       (aOther.storageBuffers)
                    , sampledImages        (aOther.sampledImages)
                    , subpassInputs        (aOther.subpassInputs)
            {}

            SHIRABE_INLINE
            SMaterialAsset(SMaterialAsset &&aOther) noexcept
                    : serialization::ISerializable<documents::IJSONSerializer<SMaterialAsset>>()
                    , serialization::IDeserializable<documents::IJSONDeserializer<SMaterialAsset>>()
                    , uid                  (aOther.uid                  )
                    , name                 (std::move(aOther.name      ))
                    , signatureAssetUid    (aOther.signatureAssetUid    )
                    , configurationAssetUid(aOther.configurationAssetUid)
                    , layoutInfo           (aOther.layoutInfo)
                    , stages               (std::move(aOther.stages    ))
                    , uniformBuffers       (aOther.uniformBuffers)
                    , storageBuffers       (aOther.storageBuffers)
                    , sampledImages        (aOther.sampledImages)
                    , subpassInputs        (aOther.subpassInputs)
            {}

        public_operators:
            SHIRABE_INLINE
            SMaterialAsset &operator=(SMaterialAsset const &aOther)
            {
                uid                   = aOther.uid;
                name                  = aOther.name;
                signatureAssetUid     = aOther.signatureAssetUid;
                configurationAssetUid = aOther.configurationAssetUid;
                layoutInfo            = aOther.layoutInfo;
                stages                = aOther.stages;
                uniformBuffers        = aOther.uniformBuffers;
                storageBuffers        = aOther.storageBuffers;
                sampledImages         = aOther.sampledImages;
                subpassInputs         = aOther.subpassInputs;

                return (*this);
            }

            SHIRABE_INLINE
            SMaterialAsset &operator=(SMaterialAsset &&aOther) noexcept
            {
                uid                   = aOther.uid;
                name                  = std::move(aOther.name);
                signatureAssetUid     = aOther.signatureAssetUid;
                configurationAssetUid = aOther.configurationAssetUid;
                layoutInfo            = aOther.layoutInfo;
                stages                = std::move(aOther.stages);
                uniformBuffers        = aOther.uniformBuffers;
                storageBuffers        = aOther.storageBuffers;
                sampledImages         = aOther.sampledImages;
                subpassInputs         = aOther.subpassInputs;

                return (*this);
            }

        public_members:
            uint64_t                    uid;
            std::string                 name;
            asset::AssetId_t            signatureAssetUid;
            asset::AssetId_t            configurationAssetUid;
            //std::unordered_map<VkPipelineStageFlagBits, SMaterialMetaStage> stages;
            SMaterialLayoutInfo         layoutInfo;
            StageMap_t                  stages;
            // Although each stage defines uniform buffers individually, they are shared
            // across all stages, due to indexing them with set and binding.
            std::vector<SUniformBuffer> uniformBuffers;
            std::vector<SUniformBuffer> storageBuffers;
            std::vector<SSampledImage>  sampledImages;
            // Fragment shader only
            std::vector<SSubpassInput>  subpassInputs;

        public_methods:
            /**
             * @brief acceptSerializer
             * @param aSerializer
             * @return
             */
            bool acceptSerializer(documents::IJSONSerializer<SMaterialAsset> &aSerializer) const final;

            /**
             * @brief acceptDeserializer
             * @param aSerializer
             * @return
             */
            bool acceptDeserializer(documents::IJSONDeserializer<SMaterialAsset> &aDeserializer) final;

            SHIRABE_INLINE bool hasVertexStage()          const { return stages.end() != stages.find(VkPipelineStageFlagBits::VK_PIPELINE_STAGE_VERTEX_SHADER_BIT);                  }
            SHIRABE_INLINE bool hasTessControlStage()     const { return stages.end() != stages.find(VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT);    }
            SHIRABE_INLINE bool hasTessEvalutationStage() const { return stages.end() != stages.find(VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT); }
            SHIRABE_INLINE bool hasGeometryStage()        const { return stages.end() != stages.find(VkPipelineStageFlagBits::VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT);                }
            SHIRABE_INLINE bool hasFragmentStage()        const { return stages.end() != stages.find(VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);                }
            SHIRABE_INLINE bool hasComputeStage()         const { return stages.end() != stages.find(VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);                 }
        };
    }
}

#endif // MATERIAL_DECLARATION_H
