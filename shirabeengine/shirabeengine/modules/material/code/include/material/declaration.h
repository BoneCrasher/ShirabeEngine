#ifndef __SHIRABE_MATERIAL_DECLARATION_H__
#define __SHIRABE_MATERIAL_DECLARATION_H__

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
#include <resources/resourcedescriptions.h>

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

        /**
         * The SMaterialIndex describes all necessary data for a basic material composition
         * in the engine.
         * The specific implementation will be provided in files referenced by the stages member.
         */
        struct SMaterialInstanceIndex
                : engine::serialization::ISerializable<documents::IJSONSerializer<SMaterialInstanceIndex>>
                , engine::serialization::IDeserializable<documents::IJSONDeserializer<SMaterialInstanceIndex>>
        {
            static std::unordered_map<EShaderStage, SMaterialIndexStage> const sEmptyMap;

        public_constructors:
            SHIRABE_INLINE
            SMaterialInstanceIndex()
                : serialization::ISerializable<documents::IJSONSerializer<SMaterialInstanceIndex>>()
                , serialization::IDeserializable<documents::IJSONDeserializer<SMaterialInstanceIndex>>()
                , uid                 (0 )
                , name                ({})
                , masterIndexAssetId  (0 )
                , configurationAssetId(0 )
            {}

            SHIRABE_INLINE
            SMaterialInstanceIndex(SMaterialInstanceIndex const &aOther)
                : serialization::ISerializable<documents::IJSONSerializer<SMaterialInstanceIndex>>()
                , serialization::IDeserializable<documents::IJSONDeserializer<SMaterialInstanceIndex>>()
                , uid                 (aOther.uid                 )
                , name                (aOther.name                )
                , masterIndexAssetId  (aOther.masterIndexAssetId  )
                , configurationAssetId(aOther.configurationAssetId)
            {}

            SHIRABE_INLINE
            SMaterialInstanceIndex(SMaterialInstanceIndex &&aOther)
                : serialization::ISerializable<documents::IJSONSerializer<SMaterialInstanceIndex>>()
                , serialization::IDeserializable<documents::IJSONDeserializer<SMaterialInstanceIndex>>()
                , uid                 (std::move(aOther.uid                   ))
                , name                (std::move(aOther.name                  ))
                , masterIndexAssetId  (std::move(aOther.masterIndexAssetId    ))
                , configurationAssetId(std::move(aOther.configurationAssetId  ))
            {}

        public_operators:
            SHIRABE_INLINE
            SMaterialInstanceIndex &operator=(SMaterialInstanceIndex const &aOther)
            {
                uid                  = aOther.uid;
                name                 = aOther.name;
                masterIndexAssetId   = aOther.masterIndexAssetId;
                configurationAssetId = aOther.configurationAssetId;

                return (*this);
            }

            SHIRABE_INLINE
            SMaterialInstanceIndex &operator=(SMaterialInstanceIndex &&aOther)
            {
                uid                  = std::move(aOther.uid);
                name                 = std::move(aOther.name);
                masterIndexAssetId   = std::move(aOther.masterIndexAssetId);
                configurationAssetId = std::move(aOther.configurationAssetId);

                return (*this);
            }

        public_members:
            uint64_t         uid;
            std::string      name;
            asset::AssetId_t masterIndexAssetId;
            asset::AssetId_t configurationAssetId;

        public_methods:
            /**
             * @brief acceptSerializer
             * @param aSerializer
             * @return
             */
            bool acceptSerializer(documents::IJSONSerializer<SMaterialInstanceIndex> &aSerializer) const;

            /**
             * @brief acceptDeserializer
             * @param aSerializer
             * @return
             */
            bool acceptDeserializer(documents::IJSONDeserializer<SMaterialInstanceIndex> &aDeserializer);
        };


        /**
         * The SMaterialIndex describes all necessary data for a basic material composition
         * in the engine.
         * The specific implementation will be provided in files referenced by the stages member.
         */
        struct SMaterialMeta
                : engine::serialization::ISerializable<documents::IJSONSerializer<SMaterialMeta>>
                , engine::serialization::IDeserializable<documents::IJSONDeserializer<SMaterialMeta>>
        {
            static std::unordered_map<VkPipelineStageFlagBits, SMaterialMetaStage> const sEmptyMetaMap;

        public_constructors:
            SHIRABE_INLINE
            SMaterialMeta()
                    : serialization::ISerializable<documents::IJSONSerializer<SMaterialMeta>>()
                    , serialization::IDeserializable<documents::IJSONDeserializer<SMaterialMeta>>()
                    , uid                  (0 )
                    , name                 ({})
                    , signatureAssetUid    (0 )
                    , configurationAssetUid(0 )
                    , stages(sEmptyMetaMap)
            {}

            SHIRABE_INLINE
            SMaterialMeta(SMaterialMeta const &aOther)
                    : serialization::ISerializable<documents::IJSONSerializer<SMaterialMeta>>()
                    , serialization::IDeserializable<documents::IJSONDeserializer<SMaterialMeta>>()
                    , uid                  (aOther.uid                  )
                    , name                 (aOther.name                 )
                    , signatureAssetUid    (aOther.signatureAssetUid    )
                    , configurationAssetUid(aOther.configurationAssetUid)
                    , stages               (aOther.stages               )
            {}

            SHIRABE_INLINE
            SMaterialMeta(SMaterialMeta &&aOther) noexcept
                    : serialization::ISerializable<documents::IJSONSerializer<SMaterialMeta>>()
                    , serialization::IDeserializable<documents::IJSONDeserializer<SMaterialMeta>>()
                    , uid                  (aOther.uid                  )
                    , name                 (std::move(aOther.name      ))
                    , signatureAssetUid    (aOther.signatureAssetUid    )
                    , configurationAssetUid(aOther.configurationAssetUid)
                    , stages               (std::move(aOther.stages    ))
            {}

        public_operators:
            SHIRABE_INLINE
            SMaterialMeta &operator=(SMaterialMeta const &aOther)
            {
                uid                   = aOther.uid;
                name                  = aOther.name;
                signatureAssetUid     = aOther.signatureAssetUid;
                configurationAssetUid = aOther.configurationAssetUid;
                stages                = aOther.stages;

                return (*this);
            }

            SHIRABE_INLINE
            SMaterialMeta &operator=(SMaterialMeta &&aOther) noexcept
            {
                uid                   = aOther.uid;
                name                  = std::move(aOther.name);
                signatureAssetUid     = aOther.signatureAssetUid;
                configurationAssetUid = aOther.configurationAssetUid;
                stages                = std::move(aOther.stages);

                return (*this);
            }

        public_members:
            uint64_t                                                        uid;
            std::string                                                     name;
            asset::AssetId_t                                                signatureAssetUid;
            asset::AssetId_t                                                configurationAssetUid;
            std::unordered_map<VkPipelineStageFlagBits, SMaterialMetaStage> stages;

        public_methods:
            /**
             * @brief acceptSerializer
             * @param aSerializer
             * @return
             */
            bool acceptSerializer(documents::IJSONSerializer<SMaterialMeta> &aSerializer) const final;

            /**
             * @brief acceptDeserializer
             * @param aSerializer
             * @return
             */
            bool acceptDeserializer(documents::IJSONDeserializer<SMaterialMeta> &aDeserializer) final;
        };

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
         * Describes a material signature composed of:
         *
         *     1. Material name
         *     2. A list of stages
         *     3. A list of uniform buffer signatures
         *     4. A list of sampled image signatures
         *     5. A list of subpass input signatures
         *
         * Implements signature de/serialization to .signature files.
         */
        struct SMaterialSignature
                : public asset::CAssetReference
                , public engine::serialization::ISerializable<documents::IJSONSerializer<SMaterialSignature>>
                , public engine::serialization::IDeserializable<documents::IJSONDeserializer<SMaterialSignature>>
        {
        public_members:
            std::string                 name;
            SMaterialLayoutInfo         layoutInfo;
            StageMap_t                  stages;
            // Although each stage defines uniform buffers individually, they are shared
            // across all stages, due to indexing them with set and binding.
            std::vector<SUniformBuffer> uniformBuffers;
            std::vector<SSampledImage>  sampledImages;
            // Fragment shader only
            std::vector<SSubpassInput>  subpassInputs;

        public_constructors:
            SHIRABE_INLINE
            SMaterialSignature(asset::AssetId_t const &aAssetUid = 0)
                : asset::CAssetReference(aAssetUid)
                , engine::serialization::ISerializable<documents::IJSONSerializer<SMaterialSignature>>()
                , engine::serialization::IDeserializable<documents::IJSONDeserializer<SMaterialSignature>>()
                , name          ({})
                , layoutInfo    ({})
                , stages        ({})
                , uniformBuffers({})
                , sampledImages ({})
                , subpassInputs ({})
            {}

            SHIRABE_INLINE
            SMaterialSignature(SMaterialSignature const &aOther)
                : asset::CAssetReference(aOther.getAssetId())
                , engine::serialization::ISerializable<documents::IJSONSerializer<SMaterialSignature>>()
                , engine::serialization::IDeserializable<documents::IJSONDeserializer<SMaterialSignature>>()
                , name          (aOther.name)
                , layoutInfo    (aOther.layoutInfo)
                , stages        (aOther.stages)
                , uniformBuffers(aOther.uniformBuffers)
                , sampledImages (aOther.sampledImages)
                , subpassInputs (aOther.subpassInputs)
            {}

        public_operators:
            SHIRABE_INLINE
            SMaterialSignature &operator=(SMaterialSignature const &aOther)
            {
                asset::CAssetReference::operator=(aOther);

                name           = aOther.name;
                layoutInfo     = aOther.layoutInfo;
                stages         = aOther.stages;
                uniformBuffers = aOther.uniformBuffers;
                sampledImages  = aOther.sampledImages;
                subpassInputs  = aOther.subpassInputs;

                return (*this);
            }

        public_methods:
            SHIRABE_INLINE bool hasVertexStage()          const { return stages.end() != stages.find(VkPipelineStageFlagBits::VK_PIPELINE_STAGE_VERTEX_SHADER_BIT);                  }
            SHIRABE_INLINE bool hasTessControlStage()     const { return stages.end() != stages.find(VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT);    }
            SHIRABE_INLINE bool hasTessEvalutationStage() const { return stages.end() != stages.find(VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT); }
            SHIRABE_INLINE bool hasGeometryStage()        const { return stages.end() != stages.find(VkPipelineStageFlagBits::VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT);                }
            SHIRABE_INLINE bool hasFragmentStage()        const { return stages.end() != stages.find(VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);                }
            SHIRABE_INLINE bool hasComputeStage()         const { return stages.end() != stages.find(VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);                 }

            /**
             * @brief acceptSerializer
             * @param aSerializer
             * @return
             */
            bool acceptSerializer(documents::IJSONSerializer<SMaterialSignature> &aSerializer) const;

            /**
             * @brief acceptDeserializer
             * @param aSerializer
             * @return
             */
            bool acceptDeserializer(documents::IJSONDeserializer<SMaterialSignature> &aDeserializer);
        };

        // Fwd-Declare for friending...
        class CMaterialConfig;

        /**
         * The SBufferData class encapsulates a set buffer locations for a single
         * buffer, which in turn is stored in a larger data buffer.
         *
         * "getLocation" will provide information on where the buffer is effectively
         * located in the larger data buffer.
         *
         * Individual data points can be read/written using "getValueLocation(...)".
         */
        class SBufferData
        {
            friend class CMaterialConfig;

        public_methods:

            /**
             * Return the location of this buffer in the larger enclosing buffer.
             *
             * @return See brief.
             */
            SHIRABE_INLINE SBufferLocation getMember(std::string const &aAccessor) const
            {
                if(mValueIndex.end() == mValueIndex.find(aAccessor))
                {
                    return {};
                }

                return mValueIndex.at(aAccessor).location;
            }

            /**
             * @brief getValues
             * @return
             */
            SHIRABE_INLINE
            Map<std::string, SBufferMember> const &getValues() const
            {
                return mValueIndex;
            }

            /**
             * Check, whether a desired value is available for a specific buffer.
             *
             * @param aBufferValue The name of the value to check for.
             * @return True, if found. False otherwise.
             */
            SHIRABE_INLINE bool hasValue(std::string const &aBufferValue) const
            {
                bool const has = (mValueIndex.end() != mValueIndex.find(aBufferValue));
                return has;
            }

        private_members:
            Map<std::string, SBufferMember> mValueIndex;
        };

        /**
         * The CMaterialConfig class describes all material data of the material layer,
         * which includes uniforms (push constants), uniform buffers, textures and samplers.
         *
         * The class will store all data and permitts read/write access to individual
         * data buffers by name.
         *
         * It will also provide access to data buffers pointers and sizes by their respective
         * names.
         *
         * Implements de/serialization to .config files.
         */
        class CMaterialConfig
                : public asset::CAssetReference
                , public engine::serialization::ISerializable<documents::IJSONSerializer<CMaterialConfig>>
                , public engine::serialization::IDeserializable<documents::IJSONDeserializer<CMaterialConfig>>
        {

        private_typedefs:
            using BufferValueIndex_t = Map<std::string, Shared<SBufferMember>>;
            using BufferIndex_t      = Map<std::string, BufferValueIndex_t>;
            using BufferData_t       = Map<std::string, Shared<void>>;
            using SampledImageMap_t  = Map<std::string, resources::ResourceId_t>;

        public_static_functions:
            static CMaterialConfig fromMaterialDesc(SMaterialSignature const &aMaterial, bool aIncludeSystemBuffers = false);

        public_constructors:
            /**
             * Create a material config, optionally from another.
             */
            SHIRABE_INLINE
            CMaterialConfig(asset::AssetId_t const &aAssetUID = 0)
                : asset::CAssetReference(aAssetUID)
                , serialization::ISerializable<documents::IJSONSerializer<CMaterialConfig>>()
                , serialization::IDeserializable<documents::IJSONDeserializer<CMaterialConfig>>()
                , mBufferIndex({})
                , mData({})
            { }

            SHIRABE_INLINE
            CMaterialConfig(CMaterialConfig const &aOther)
                : asset::CAssetReference(aOther.getAssetId())
                , serialization::ISerializable<documents::IJSONSerializer<CMaterialConfig>>()
                , serialization::IDeserializable<documents::IJSONDeserializer<CMaterialConfig>>()
                , mBufferIndex(aOther.mBufferIndex)
                , mData       (aOther.mData)
            { }

            SHIRABE_INLINE
            CMaterialConfig(CMaterialConfig  &&aOther)
                : asset::CAssetReference(aOther.getAssetId())
                , serialization::ISerializable<documents::IJSONSerializer<CMaterialConfig>>()
                , serialization::IDeserializable<documents::IJSONDeserializer<CMaterialConfig>>()
                , mBufferIndex(std::move(aOther.mBufferIndex))
                , mData       (std::move(aOther.mData))
            { }

        public_destructors:
            ~CMaterialConfig() = default;

        public_operators:
            /**
             * Assign another material config.
             */
            SHIRABE_INLINE
            CMaterialConfig &operator=(CMaterialConfig const &aOther)
            {
                mBufferIndex = aOther.mBufferIndex;
                mData        = aOther.mData;

                return (*this);
            }

            SHIRABE_INLINE
            CMaterialConfig &operator=(CMaterialConfig &&aOther)
            {
                mBufferIndex = std::move(aOther.mBufferIndex);
                mData        = std::move(aOther.mData);

                return (*this);
            }

        public_methods:
            /**
             * Return a pointer to a specific data reinterpreted as TBufferType.
             * The buffer is identified by it's name.
             *
             * @tparam TBufferType Type of the buffer to be returned.
             * @param aBufferName  The name of the buffer to fetch.
             * @return             EEngineStatus::Ok and a valid pointer to the buffer, if successful.
             * @return             EEngineStatus::Error and nullptr or any error code on failure.
             */
            template <typename TBufferType>
            CEngineResult<TBufferType const*> getBufferTyped(std::string const &aBufferName) const;

            /**
             * Return a pointer to a specific data.
             * The buffer is identified by it's name.
             *
             * @param aBufferName  The name of the buffer to fetch.
             * @return             EEngineStatus::Ok and a valid pointer to the buffer, if successful.
             * @return             EEngineStatus::Error and nullptr or any error code on failure.
             */
            CEngineResult<void const *const> getBuffer(std::string const &aBufferName) const;
            /**
             * Return a pointer to nutable specific data.
             * The buffer is identified by it's name.
             *
             * @param aBufferName  The name of the buffer to fetch.
             * @return             EEngineStatus::Ok and a valid pointer to the buffer, if successful.
             * @return             EEngineStatus::Error and nullptr or any error code on failure.
             */
            CEngineResult<void *const> getBuffer(std::string const &aBufferName);

            /**
             * Return the value of a buffer member identified by buffername/fieldname.
             *
             * @param aBufferName  The name of the buffer containing the value to fetch.
             * @param aFieldName   The name of the value to fetch.
             * @return             EEngineStatus::Ok and a valid pointer to the data, if successful.
             * @return             EEngineStatus::Error and nullptr or any error code on failure.
             */
            template <typename TDataType>
            CEngineResult<TDataType const*> getBufferValue(
                    std::string const &aBufferName,
                    std::string const &aFieldName) const;

            /**
             * setValue
             *
             * @tparam TDataType   Type of the data to be set.
             * @param aBufferName  The name of the buffer containing the value to fetch.
             * @param aFieldName   The name of the value to fetch.
             * @param aFieldValue  The typed data to set.
             * @return             EEngineStatus::Ok, if successful.
             * @return             EEngineStatus::<ErrorCode> on error.
             */
            template <typename TDataType>
            CEngineResult<> setBufferValue(
                    std::string const &aBufferName,
                    std::string const &aFieldName,
                    TDataType   const &aFieldValue);

            SHIRABE_INLINE void setSampledImage(std::string const &aSlotId, resources::ResourceId_t const &aSampledImageResourceId)
            {
                if(mSampledImageIndex.end() == std::find(mSampledImageIndex.begin(), mSampledImageIndex.end(), [&](std::string const &aCmp) -> bool { return (aCmp == aSlotId); }))
                {
                    return;
                }

                mSampledImageMap[aSlotId] = aSampledImageResourceId;
            }

            SHIRABE_INLINE SampledImageMap_t const &getSampledImageAssignment() const
            {
                return mSampledImageMap;
            }

            /**
             * @brief acceptSerializer
             * @param aSerializer
             * @return
             */
            bool acceptSerializer(documents::IJSONSerializer<CMaterialConfig> &aSerializer) const;

            /**
             * @brief acceptDeserializer
             * @param aSerializer
             * @return
             */
            bool acceptDeserializer(documents::IJSONDeserializer<CMaterialConfig> &aDeserializer);

        private_methods:
            /**
             * Where there is a will there is a way
             *
             * @param aBufferName
             * @param aBufferValue
             * @param aOutPointer
             * @return
             */
            template <typename TDataType>
            CEngineResult<TDataType const*> getBufferValuePointer(
                    std::string const &aBufferName,
                    std::string const &aBufferValue) const;

            /**
             * Where there is a will there is a way
             *
             * @param aBufferName
             * @param aBufferValue
             * @param aOutPointer
             * @return
             */
            template <typename TDataType>
            CEngineResult<TDataType *> getBufferValuePointer(
                    std::string const &aBufferName,
                    std::string const &aBufferValue);

            /**
             * Check, whether a desired buffer is available.
             *
             * @param aBufferName The name of the buffer desired.
             * @return True, if found. False otherwise.
             */
            SHIRABE_INLINE bool hasBuffer(std::string const &aBufferName) const
            {
                bool const has = (mBufferIndex.end() != mBufferIndex.find(aBufferName));
                return has;
            }

        private_members:
            BufferIndex_t       mBufferIndex;
            BufferData_t        mData;
            Vector<std::string> mSampledImageIndex;
            SampledImageMap_t   mSampledImageMap;

        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TDataType>
        CEngineResult<TDataType const*> CMaterialConfig::getBufferValuePointer(
                std::string  const        &aBufferName,
                std::string  const        &aBufferValue) const
        {
            bool const hasBuffer = this->hasBuffer(aBufferName);
            if(not hasBuffer)
            {
                return CEngineResult<TDataType const *>(EEngineStatus::Error, nullptr);
            }

            BufferValueIndex_t const &bufferIndex = mBufferIndex.at(aBufferName);

            std::string const combinedValuePath = fmt::format("{}.{}", aBufferName, aBufferValue);

            bool const hasValue = (bufferIndex.end() != bufferIndex.find(combinedValuePath));
            if(not hasValue)
            {
                return CEngineResult<TDataType const *>(EEngineStatus::Error, nullptr);
            }

            Shared<SBufferMember> const &bufferValue = bufferIndex.at(combinedValuePath);

            Shared<void>      alignedData = mData.at(aBufferName);
            auto const *const bufferData  = reinterpret_cast<int8_t const *>(alignedData.get());
            auto const *const adjusted    = reinterpret_cast<TDataType const *>(bufferData + bufferValue->location.offset); // (bufferData + (bufferValue->location.offset / sizeof(TDataType)));
            return { EEngineStatus::Ok, adjusted };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TDataType>
        CEngineResult<TDataType *> CMaterialConfig::getBufferValuePointer(
                std::string  const &aBufferName,
                std::string  const &aBufferValue)
        {
            // Dirty hack to reuse the function implementation...
            CEngineResult<TDataType const*> const  result    = static_cast<CMaterialConfig const*>(this)->getBufferValuePointer<TDataType>(aBufferName, aBufferValue);
            TDataType                       const *constData = result.data();

            return { result.result(), const_cast<TDataType *>(constData) };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TBufferType>
        CEngineResult<TBufferType const*> CMaterialConfig::getBufferTyped(std::string const &aBufferName) const
        {
            bool const has = hasBuffer(aBufferName);
            if(not has)
            {
                return CEngineResult<TBufferType const*>(EEngineStatus::Error, nullptr);
            }

            BufferValueIndex_t const &bufferIndex = mBufferIndex.at(aBufferName);
            SBufferLocation    const &location    = bufferIndex.at(aBufferName)->location;

            // uint8_t     const *const data       = (mData.data() + location.offset);
            Shared<void>             alignedData = mData.at(aBufferName);
            TBufferType const *const bufferData  = static_cast<TBufferType const *const>(alignedData.get());

            return CEngineResult(EEngineStatus::Ok, bufferData);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TDataType>
        CEngineResult<TDataType const*> CMaterialConfig::getBufferValue(
                std::string const &aBufferName,
                std::string const &aFieldName) const
        {
            return getBufferValuePointer<TDataType>(aBufferName, aFieldName);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TDataType>
        CEngineResult<> CMaterialConfig::setBufferValue(
                std::string const &aBufferName,
                std::string const &aFieldName,
                TDataType   const &aFieldValue)
        {
            CEngineResult<TDataType *> result = getBufferValuePointer<TDataType>(aBufferName, aFieldName);
            if(result.successful())
            {
                TDataType *buffer = result.data();
                *buffer = aFieldValue;
            }

            return result.result();
        }
        //<-----------------------------------------------------------------------------

        /**
         * A material master is composed by a signature and base configuration.
         * It will be used to create instances from this material.
         */
        class CMaterialMaster
                : public asset::CAssetReference
        {
        public_constructors:
            CMaterialMaster() = default;

            SHIRABE_INLINE
            CMaterialMaster(asset::AssetId_t   const &aAssetUID,
                            std::string        const &aName,
                            SMaterialSignature      &&aSignature,
                            CMaterialConfig         &&aConfig)
                : asset::CAssetReference (aAssetUID)
                , mName                  (aName                )
                , mSignature             (std::move(aSignature))
            {}

            SHIRABE_INLINE
            CMaterialMaster(CMaterialMaster const &aOther)
                : asset::CAssetReference (aOther.getAssetId())
                , mName                  (aOther.mName         )
                , mSignature             (aOther.mSignature    )
            {}

            SHIRABE_INLINE
            CMaterialMaster(CMaterialMaster &&aOther)
                : asset::CAssetReference (aOther.getAssetId())
                , mName                  (std::move(aOther.mName         ))
                , mSignature             (std::move(aOther.mSignature    ))
            {}

        public_destructors:
            ~CMaterialMaster() = default;

        public_operators:
            SHIRABE_INLINE
            CMaterialMaster &operator=(CMaterialMaster const &aOther)
            {
                asset::CAssetReference::operator=(aOther.getAssetId());

                mName      = aOther.mName;
                mSignature = aOther.mSignature;

                return (*this);
            }

            SHIRABE_INLINE
            CMaterialMaster &operator=(CMaterialMaster &&aOther) noexcept
            {
                asset::CAssetReference::operator=(aOther.getAssetId());

                mName      = std::move(aOther.mName     );
                mSignature = std::move(aOther.mSignature);

                return (*this);
            }

        public_methods:
            SHIRABE_INLINE
            std::string const &name() const
            {
                return mName;
            }

            SHIRABE_INLINE
            SMaterialSignature const &signature() const
            {
                return mSignature;
            }

        private_methods:
            friend class CMaterialLoader; // The below private methods are exclusively to be invoked by the material loader. Ensure this...

        private_members:
            std::string        mName;
            SMaterialSignature mSignature;
        };

        /**
         * A material instance describes a configurable and bindable material state which can be imagined
         * to be a single render call for an object having this material assigned.
         *
         * Instances track their master during edit-time, in case the master signature changes due to
         * base configuration value changes or shader file updates.
         */
        class CMaterialInstance
        {
            public_constructors:
                SHIRABE_INLINE
                explicit CMaterialInstance(std::string             const &aName,
                                           Shared<CMaterialMaster>        aMaster)
                    : mName           ( aName            )
                    , mConfiguration  (                  )
                    , mMasterReference(std::move(aMaster))
                {}

                SHIRABE_INLINE
                explicit CMaterialInstance(CMaterialInstance &&aOther)
                    : mName                  (std::move(aOther.mName           ))
                    , mConfiguration         (std::move(aOther.mConfiguration  ))
                    , mMasterReference       (std::move(aOther.mMasterReference))
                {}

            public_destructors:
                ~CMaterialInstance() = default;

            public_operators:
                SHIRABE_INLINE
                CMaterialInstance &operator=(CMaterialInstance &&aOther)
                {
                    mName            = std::move(aOther.mName           );
                    mMasterReference = std::move(aOther.mMasterReference);
                    mConfiguration   = std::move(aOther.mConfiguration  );

                    return (*this);
                }

        public_methods:
                SHIRABE_INLINE
                std::string const &name() const
                {
                    return mName;
                }

                SHIRABE_INLINE
                CMaterialConfig const &config() const
                {
                    return *mConfiguration;
                }

                SHIRABE_INLINE
                CMaterialConfig &getMutableConfiguration()
                {
                    return mConfiguration.value();
                }

                SHIRABE_INLINE
                Shared<CMaterialMaster> const &master() const
                {
                    return mMasterReference;
                }

                EEngineStatus createConfiguration(bool aIncludeSystemBuffers = false);

        private_members:
            std::string                    mName;
            std::optional<CMaterialConfig> mConfiguration;
            Shared<CMaterialMaster>        mMasterReference;
        };


    }
}

#endif // MATERIAL_DECLARATION_H
