#ifndef __SHIRABE_MATERIAL_DECLARATION_H__
#define __SHIRABE_MATERIAL_DECLARATION_H__

#include <assert.h>
#include <string>
#include <vector>
#include <stdint.h>
#include <filesystem>
#include <cstring>

#include <base/declaration.h>
#include <base/stl_container_helpers.h>
#include <platform/platform.h>
#include <core/enginestatus.h>
#include <core/serialization/serialization.h>

namespace engine
{
    namespace serialization
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
         * The SMaterialIndexStage struct describes individual module's/stage's file
         * references.
         */
        struct SMaterialIndexStage
        {
            std::filesystem::path glslSourceFilename;
            std::filesystem::path spvModuleFilename;
        };

        /**
         * The SMaterialIndex describes all necessary data for a basic material composition
         * in the engine.
         * The specific implementation will be provided in files referenced by the stages member.
         */
        struct SMaterialMasterIndex
                : engine::serialization::ISerializable<serialization::IJSONSerializer<SMaterialMasterIndex>>
                , engine::serialization::IDeserializable<serialization::IJSONDeserializer<SMaterialMasterIndex>>
        {
            static std::unordered_map<EShaderStage, SMaterialIndexStage> const sEmptyMap;

        public_constructors:
            SHIRABE_INLINE
            SMaterialMasterIndex()
                : serialization::ISerializable<serialization::IJSONSerializer<SMaterialMasterIndex>>()
                , serialization::IDeserializable<serialization::IJSONDeserializer<SMaterialMasterIndex>>()
                , uid                      (0 )
                , name                     ({})
                , signatureFilename        (  )
                , baseConfigurationFilename(  )
                , stages                   (sEmptyMap)
            {}

            SHIRABE_INLINE
            SMaterialMasterIndex(SMaterialMasterIndex const &aOther)
                : serialization::ISerializable<serialization::IJSONSerializer<SMaterialMasterIndex>>()
                , serialization::IDeserializable<serialization::IJSONDeserializer<SMaterialMasterIndex>>()
                , uid                      (aOther.uid                      )
                , name                     (aOther.name                     )
                , signatureFilename        (aOther.signatureFilename        )
                , baseConfigurationFilename(aOther.baseConfigurationFilename)
                , stages                   (aOther.stages                   )
            {}

            SHIRABE_INLINE
            SMaterialMasterIndex(SMaterialMasterIndex &&aOther)
                : serialization::ISerializable<serialization::IJSONSerializer<SMaterialMasterIndex>>()
                , serialization::IDeserializable<serialization::IJSONDeserializer<SMaterialMasterIndex>>()
                , uid                      (std::move(aOther.uid                        ))
                , name                     (std::move(aOther.name                       ))
                , signatureFilename        (std::move(aOther.signatureFilename          ))
                , baseConfigurationFilename(std::move(aOther.baseConfigurationFilename  ))
                , stages                   (std::move(aOther.stages                     ))
            {}

        public_operators:
            SHIRABE_INLINE
            SMaterialMasterIndex &operator=(SMaterialMasterIndex const &aOther)
            {
                uid                       = aOther.uid;
                name                      = aOther.name;
                signatureFilename         = aOther.signatureFilename;
                baseConfigurationFilename = aOther.baseConfigurationFilename;
                stages                    = aOther.stages;

                return (*this);
            }

            SHIRABE_INLINE
            SMaterialMasterIndex &operator=(SMaterialMasterIndex &&aOther)
            {
                uid                       = std::move(aOther.uid);
                name                      = std::move(aOther.name);
                signatureFilename         = std::move(aOther.signatureFilename);
                baseConfigurationFilename = std::move(aOther.baseConfigurationFilename);
                stages                    = std::move(aOther.stages);

                return (*this);
            }

        public_members:
            uint64_t                                              uid;
            std::string                                           name;
            std::filesystem::path                                 signatureFilename;
            std::filesystem::path                                 baseConfigurationFilename;
            std::unordered_map<EShaderStage, SMaterialIndexStage> stages;

        public_methods:
            /**
             * @brief acceptSerializer
             * @param aSerializer
             * @return
             */
            bool acceptSerializer(serialization::IJSONSerializer<SMaterialMasterIndex> &aSerializer) const;

            /**
             * @brief acceptDeserializer
             * @param aSerializer
             * @return
             */
            bool acceptDeserializer(serialization::IJSONDeserializer<SMaterialMasterIndex> &aDeserializer);
        };

        /**
         * The SMaterialIndex describes all necessary data for a basic material composition
         * in the engine.
         * The specific implementation will be provided in files referenced by the stages member.
         */
        struct SMaterialInstanceIndex
                : engine::serialization::ISerializable<serialization::IJSONSerializer<SMaterialInstanceIndex>>
                , engine::serialization::IDeserializable<serialization::IJSONDeserializer<SMaterialInstanceIndex>>
        {
            static std::unordered_map<EShaderStage, SMaterialIndexStage> const sEmptyMap;

        public_constructors:
            SHIRABE_INLINE
            SMaterialInstanceIndex()
                : serialization::ISerializable<serialization::IJSONSerializer<SMaterialInstanceIndex>>()
                , serialization::IDeserializable<serialization::IJSONDeserializer<SMaterialInstanceIndex>>()
                , uid                  (0 )
                , name                 ({})
                , masterIndexFilename  (  )
                , configurationFilename(  )
            {}

            SHIRABE_INLINE
            SMaterialInstanceIndex(SMaterialInstanceIndex const &aOther)
                : serialization::ISerializable<serialization::IJSONSerializer<SMaterialInstanceIndex>>()
                , serialization::IDeserializable<serialization::IJSONDeserializer<SMaterialInstanceIndex>>()
                , uid                  (aOther.uid                  )
                , name                 (aOther.name                 )
                , masterIndexFilename  (aOther.masterIndexFilename  )
                , configurationFilename(aOther.configurationFilename)
            {}

            SHIRABE_INLINE
            SMaterialInstanceIndex(SMaterialInstanceIndex &&aOther)
                : serialization::ISerializable<serialization::IJSONSerializer<SMaterialInstanceIndex>>()
                , serialization::IDeserializable<serialization::IJSONDeserializer<SMaterialInstanceIndex>>()
                , uid                  (std::move(aOther.uid                    ))
                , name                 (std::move(aOther.name                   ))
                , masterIndexFilename  (std::move(aOther.masterIndexFilename    ))
                , configurationFilename(std::move(aOther.configurationFilename  ))
            {}

        public_operators:
            SHIRABE_INLINE
            SMaterialInstanceIndex &operator=(SMaterialInstanceIndex const &aOther)
            {
                uid                   = aOther.uid;
                name                  = aOther.name;
                masterIndexFilename   = aOther.masterIndexFilename;
                configurationFilename = aOther.configurationFilename;

                return (*this);
            }

            SHIRABE_INLINE
            SMaterialInstanceIndex &operator=(SMaterialInstanceIndex &&aOther)
            {
                uid                   = std::move(aOther.uid);
                name                  = std::move(aOther.name);
                masterIndexFilename   = std::move(aOther.masterIndexFilename);
                configurationFilename = std::move(aOther.configurationFilename);

                return (*this);
            }

        public_members:
            uint64_t              uid;
            std::string           name;
            std::filesystem::path masterIndexFilename;
            std::filesystem::path configurationFilename;

        public_methods:
            /**
             * @brief acceptSerializer
             * @param aSerializer
             * @return
             */
            bool acceptSerializer(serialization::IJSONSerializer<SMaterialInstanceIndex> &aSerializer) const;

            /**
             * @brief acceptDeserializer
             * @param aSerializer
             * @return
             */
            bool acceptDeserializer(serialization::IJSONDeserializer<SMaterialInstanceIndex> &aDeserializer);
        };

        /**
         * @brief The SMaterialType struct
         */
        struct SMaterialType
        {
            std::string name;
            uint32_t    byteSize;
            uint32_t    vectorSize;
            uint32_t    matrixRows;
            uint32_t    matrixColumns;
            uint32_t    matrixColumnStride;
            uint32_t    arraySize;
            uint32_t    arrayStride;
        };

        /**
         * Describes a shader stage input by it's name and explicit location.
         */
        struct SStageInput
        {
            std::string   name;
            uint32_t      location;
            SMaterialType type;
        };

        /**
         * Describes a shader stage input by it's name and explicit location.
         */
        struct SStageOutput
        {
            std::string   name;
            uint32_t      location;
            SMaterialType type;
        };

        /**
         * Describes a fragment shader subpass input. Will be empty for all other shader types.
         */
        struct SSubpassInput
        {
            std::string name;
            uint32_t    attachmentIndex;
            uint32_t    set;
            uint32_t    binding;
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
                bool const offsetEq  = (offset == aOther.offset);
                bool const lengthEq  = (length == aOther.length);
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

        /**
         * @brief The SUniformBufferMember struct
         */
        struct SUniformBufferMember
        {
            std::string     name;
            SBufferLocation location;
        };
        using UniformBufferMemberMap_t = std::unordered_map<std::string, SUniformBufferMember>;

        /**
         * @brief The SUniformBuffer struct
         */
        struct SUniformBuffer
        {
        public_members:
            std::string              name;
            SBufferLocation          location;
            uint64_t                 set;
            uint64_t                 binding;
            UniformBufferMemberMap_t members;

        public_constructors:
            SUniformBuffer() = default;

            SHIRABE_INLINE
            SUniformBuffer(SUniformBuffer const &aOther)
                : name    (aOther.name    )
                , location(aOther.location)
                , set     (aOther.set     )
                , binding (aOther.binding )
                , members (aOther.members )
            {}

        public_operators:
            SUniformBuffer &operator=(SUniformBuffer const &aOther)
            {
                name     = aOther.name;
                location = aOther.location;
                set      = aOther.set;
                binding  = aOther.binding;
                members  = aOther.members;

                return (*this);
            }
        };

        /**
         * Describes a sampled image in a GLSL shader.
         */
        struct SSampledImage
        {
            std::string name;
            uint32_t    set;
            uint32_t    binding;
        };

        /**
         * Describes a single shader stage including all it's inputs, outputs and resources.
         */
        struct SMaterialStage
        {
        public_members:
            EShaderStage                stage;
            std::string                 stageName;
            std::string                 filename;
            std::vector<SStageInput>    inputs;
            std::vector<SStageOutput>   outputs;

        public_constructors:
            SMaterialStage() = default;

            SHIRABE_INLINE
            SMaterialStage(SMaterialStage const &aOther)
                : stage(aOther.stage)
                , stageName(aOther.stageName)
                , filename(aOther.filename)
                , inputs(aOther.inputs)
                , outputs(aOther.outputs)
            {}

        public_operators:
            SMaterialStage &operator=(SMaterialStage const &aOther)
            {
                stage     = aOther.stage;
                stageName = aOther.stageName;
                filename  = aOther.filename;
                inputs    = aOther.inputs;
                outputs   = aOther.outputs;

                return (*this);
            }
        };
        using StageMap_t = std::unordered_map<EShaderStage, SMaterialStage>;

        /**
         * @brief The SMaterial struct
         */
        struct SMaterialSignature
                : engine::serialization::ISerializable<serialization::IJSONSerializer<SMaterialSignature>>
                , engine::serialization::IDeserializable<serialization::IJSONDeserializer<SMaterialSignature>>
        {
        public_members:
            std::string                 name;
            StageMap_t                  stages;
            // Although each stage defines uniform buffers individually, they are shared
            // across all stages, due to indexing them with set and binding.
            std::vector<SUniformBuffer> uniformBuffers;
            std::vector<SSampledImage>  sampledImages;
            // Fragment shader only
            std::vector<SSubpassInput>  subpassInputs;

        public_constructors:
            SMaterialSignature() = default;

            SHIRABE_INLINE
            SMaterialSignature(SMaterialSignature const &aOther)
                : engine::serialization::ISerializable<serialization::IJSONSerializer<SMaterialSignature>>()
                , engine::serialization::IDeserializable<serialization::IJSONDeserializer<SMaterialSignature>>()
                , name(aOther.name)
                , stages(aOther.stages)
                , uniformBuffers(aOther.uniformBuffers)
                , sampledImages(aOther.sampledImages)
                , subpassInputs(aOther.subpassInputs)
            {}

        public_operators:
            SMaterialSignature &operator=(SMaterialSignature const &aOther)
            {
                name           = aOther.name;
                stages         = aOther.stages;
                uniformBuffers = aOther.uniformBuffers;
                sampledImages  = aOther.sampledImages;
                subpassInputs  = aOther.subpassInputs;

                return (*this);
            }

        public_methods:
            SHIRABE_INLINE bool hasVertexStage()          const { return stages.end() != stages.find(EShaderStage::Vertex);                  }
            SHIRABE_INLINE bool hasTessControlStage()     const { return stages.end() != stages.find(EShaderStage::TesselationControlPoint); }
            SHIRABE_INLINE bool hasTessEvalutationStage() const { return stages.end() != stages.find(EShaderStage::TesselationEvaluation);   }
            SHIRABE_INLINE bool hasGeometryStage()        const { return stages.end() != stages.find(EShaderStage::Geometry);                }
            SHIRABE_INLINE bool hasFragmentStage()        const { return stages.end() != stages.find(EShaderStage::Fragment);                }
            SHIRABE_INLINE bool hasComputeStage()         const { return stages.end() != stages.find(EShaderStage::Compute);                 }

            /**
             * @brief acceptSerializer
             * @param aSerializer
             * @return
             */
            bool acceptSerializer(serialization::IJSONSerializer<SMaterialSignature> &aSerializer) const;

            /**
             * @brief acceptDeserializer
             * @param aSerializer
             * @return
             */
            bool acceptDeserializer(serialization::IJSONDeserializer<SMaterialSignature> &aDeserializer);
        };

        /**
         * The CMaterialInterface class defines the public interface into the
         * pipeline behind a material layer, which is basically the vertex shader input description.
         */
        class CMaterialInterface
        {
        public_methods:

        private_members:
        };

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
            SHIRABE_INLINE SBufferLocation const &getLocation() const
            {
                return mLocation;
            }

            /**
             * @brief getValues
             * @return
             */
            Map<std::string, SBufferLocation> const &getValues() const
            {
                return mValueIndex;
            }

            /**
             * Return the location information for the desired buffer value.
             *
             * @param aBufferValue The name of the desired buffer value.
             * @return See brief.
             */
            SHIRABE_INLINE SBufferLocation const &getValueLocation(std::string const &aBufferValue) const
            {
                return mValueIndex.at(aBufferValue);
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
            SBufferLocation                   mLocation;
            Map<std::string, SBufferLocation> mValueIndex;
        };

        /**
         * The CMaterialData class describes all material data of the material layer,
         * which includes uniforms (push constants), uniform buffers, textures and samplers.
         *
         * The class will store all data and permitts read/write access to individual
         * data buffers by name.
         *
         * It will also provide access to data buffers pointers and sizes by their respective
         * names.
         */
        class CMaterialConfig
                : engine::serialization::ISerializable<serialization::IJSONSerializer<CMaterialConfig>>
                , engine::serialization::IDeserializable<serialization::IJSONDeserializer<CMaterialConfig>>
        {
        public_static_functions:
            static CMaterialConfig fromMaterialDesc(SMaterialSignature const &aMaterial);

        public_constructors:
            /**
             * Create a material config, optionally from another.
             */
            SHIRABE_INLINE
            CMaterialConfig()
                : serialization::ISerializable<serialization::IJSONSerializer<CMaterialConfig>>()
                , serialization::IDeserializable<serialization::IJSONDeserializer<CMaterialConfig>>()
                , mBufferIndex({})
                , mData({})
            { }

            SHIRABE_INLINE
            CMaterialConfig(CMaterialConfig const &aOther)
                : serialization::ISerializable<serialization::IJSONSerializer<CMaterialConfig>>()
                , serialization::IDeserializable<serialization::IJSONDeserializer<CMaterialConfig>>()
                , mBufferIndex(aOther.mBufferIndex)
                , mData       (aOther.mData)
            { }

            SHIRABE_INLINE
            CMaterialConfig(CMaterialConfig  &&aOther)
                : serialization::ISerializable<serialization::IJSONSerializer<CMaterialConfig>>()
                , serialization::IDeserializable<serialization::IJSONDeserializer<CMaterialConfig>>()
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

            CMaterialConfig &operator=(CMaterialConfig &&aOther)
            {
                mBufferIndex = std::move(aOther.mBufferIndex);
                mData        = std::move(aOther.mData);

                return (*this);
            }

        public_methods:
            /**
             * getBuffer
             *
             * @param aBufferName
             * @param aOutBuffer
             * @return
             */
            template <typename TBufferType>
            CEngineResult<TBufferType const*> getBuffer(std::string const &aBufferName) const;

            /**
             * getValue
             *
             * @param aBufferName
             * @param aFieldName
             * @param aOutValue
             * @return
             */
            template <typename TDataType>
            CEngineResult<TDataType const*> getBufferValue(
                    std::string const &aBufferName,
                    std::string const &aFieldName) const;

            /**
             * getValue
             *
             * @param aBufferName
             * @param aFieldName
             * @param aOutValue
             * @return
             */
            SHIRABE_INLINE
            CEngineResult<uint8_t const*> getBufferValue(SBufferLocation const &aLocation) const;

            /**
             * setValue
             *
             * @param aBufferName
             * @param aFieldName
             * @param aFieldValue
             * @return
             */
            template <typename TDataType>
            CEngineResult<> setBufferValue(
                    std::string const &aBufferName,
                    std::string const &aFieldName,
                    TDataType   const &aFieldValue);

            /**
             * setValue
             *
             * @param aBufferName
             * @param aFieldName
             * @param aFieldValue
             * @return
             */
            SHIRABE_INLINE
            CEngineResult<> setBufferValue(
                    SBufferLocation const       &aLocation,
                    uint8_t         const *const aData);

            /**
             * Override individual data points in the configuration in order to implement
             * material instance behaviour.
             *
             * @param aOther
             * @return
             */
            CEngineResult<> override(CMaterialConfig const &aOther);

            /**
             * @brief acceptSerializer
             * @param aSerializer
             * @return
             */
            bool acceptSerializer(serialization::IJSONSerializer<CMaterialConfig> &aSerializer) const;

            /**
             * @brief acceptDeserializer
             * @param aSerializer
             * @return
             */
            bool acceptDeserializer(serialization::IJSONDeserializer<CMaterialConfig> &aDeserializer);

        private_methods:
            /**
             *
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
            Map<std::string, SBufferData> mBufferIndex;
            std::vector<uint8_t>          mData;
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

            SBufferData const &buffer = mBufferIndex.at(aBufferName);

            bool const hasValue = buffer.hasValue(aBufferValue);
            if(not hasValue)
            {
                return CEngineResult<TDataType const *>(EEngineStatus::Error, nullptr);
            }

            SBufferLocation const &location = buffer.getValueLocation(aBufferValue);

            uint8_t   const *const data       = (mData.data() + location.offset);
            TDataType const *const bufferData = reinterpret_cast<TDataType const *>(data);

            return CEngineResult<TDataType const *>(EEngineStatus::Ok, bufferData);
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
            TDataType                       const *constData = nullptr;
            CEngineResult<TDataType const*> const  result    = static_cast<CMaterialConfig const*>(this)->getBufferValuePointer<TDataType>(aBufferName, aBufferValue);

            return CEngineResult(result.result(), const_cast<TDataType *>(constData));
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TBufferType>
        CEngineResult<TBufferType const*> CMaterialConfig::getBuffer(std::string const &aBufferName) const
        {
            bool const has = hasBuffer(aBufferName);
            if(not has)
            {
                return CEngineResult<TBufferType const*>(EEngineStatus::Error, nullptr);
            }

            SBufferData     const &buffer = mBufferIndex.at(aBufferName);
            SBufferLocation const &location = buffer.getLocation();

            uint8_t     const *const data       = (mData.data() + location.offset);
            TBufferType const *const bufferData = static_cast<TBufferType const *const>(data);

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
        CEngineResult<uint8_t const*> CMaterialConfig::getBufferValue(SBufferLocation const &aLocation) const
        {
            bool const validRequest = (0 < aLocation.offset && mData.size() > (aLocation.offset + aLocation.length));
            if(not validRequest)
            {
                return { EEngineStatus::Error };
            }

            return { EEngineStatus::Ok, (mData.data() + aLocation.offset) };
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
                *(result.data()) = aFieldValue;
            }

            return result;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CMaterialConfig::setBufferValue(
                SBufferLocation const       &aLocation,
                uint8_t         const *const aData)
        {
            std::memcpy(mData.data() + aLocation.offset, aData, sizeof(uint8_t) * aLocation.length);
            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        /**
         * A material instance describes a configurable and bindable material state which can be imagined
         * to be a single render call for an object having this material assigned.
         */
        class CMaterialInstance
        {
            public_constructors:
                CMaterialInstance() = default;

                SHIRABE_INLINE
                CMaterialInstance(std::string        const &aName,
                                  SMaterialSignature      &&aSignature,
                                  CMaterialConfig         &&aConfig)
                    : mName         (aName                )
                    , mSignature    (std::move(aSignature))
                    , mConfiguration(std::move(aConfig)   )
                {}

                SHIRABE_INLINE
                CMaterialInstance(CMaterialInstance const &aOther)
                    : mName         (aOther.mName         )
                    , mSignature    (aOther.mSignature    )
                    , mConfiguration(aOther.mConfiguration)
                {}

                SHIRABE_INLINE
                CMaterialInstance(CMaterialInstance &&aOther)
                    : mName         (std::move(aOther.mName         ))
                    , mSignature    (std::move(aOther.mSignature    ))
                    , mConfiguration(std::move(aOther.mConfiguration))
                {}

            public_destructors:
                ~CMaterialInstance() = default;

            public_operators:
                SHIRABE_INLINE
                CMaterialInstance &operator=(CMaterialInstance const &aOther)
                {
                    mName          = aOther.mName;
                    mSignature     = aOther.mSignature;
                    mConfiguration = aOther.mConfiguration;

                    return (*this);
                }

                SHIRABE_INLINE
                CMaterialInstance &operator=(CMaterialInstance &&aOther)
                {
                    mName          = std::move(aOther.mName         );
                    mSignature     = std::move(aOther.mSignature    );
                    mConfiguration = std::move(aOther.mConfiguration);

                    return (*this);
                }

        public_methods:

        private_members:
            std::string        mName;
            SMaterialSignature mSignature;
            CMaterialConfig    mConfiguration;
        };

        /**
         * The CMaterialLayer class describes a single layer of a material container, referencing a material instance.
         */
        class CMaterialLayer
        {
        public_constructors:
            CMaterialLayer() = default;

            SHIRABE_INLINE
            CMaterialLayer(std::string const &aName)
                : mName(aName)
                , mMaterialInstance(nullptr)
                , mEnabled(false)
            {}

            SHIRABE_INLINE
            CMaterialLayer(CMaterialLayer const &aOther)
                : mName            (aOther.mName            )
                , mMaterialInstance(aOther.mMaterialInstance)
                , mEnabled         (aOther.mEnabled         )
            {}

            SHIRABE_INLINE
            CMaterialLayer(CMaterialLayer &&aOther)
                : mName            (std::move(aOther.mName            ))
                , mMaterialInstance(std::move(aOther.mMaterialInstance))
                , mEnabled         (std::move(aOther.mEnabled         ))
            {
                aOther.mMaterialInstance = nullptr;
            }

        public_destructors:
            ~CMaterialLayer() = default;

        public_operators:
            SHIRABE_INLINE
            CMaterialLayer &operator=(CMaterialLayer const &aOther)
            {
                mName             = aOther.mName;
                mMaterialInstance = aOther.mMaterialInstance;
                mEnabled          = aOther.mEnabled;

                return (*this);
            }

            SHIRABE_INLINE
            CMaterialLayer &operator=(CMaterialLayer &&aOther)
            {
                mName             = std::move(aOther.mName            );
                mMaterialInstance = std::move(aOther.mMaterialInstance);
                mEnabled          = std::move(aOther.mEnabled         );

                mMaterialInstance = nullptr;

                return (*this);
            }
        public_methods:
            SHIRABE_INLINE
            void assignMaterialInstance(CStdSharedPtr_t<CMaterialInstance> const &aInstance)
            {
                mMaterialInstance = aInstance;
            }

            SHIRABE_INLINE
            CStdSharedPtr_t<CMaterialInstance> getAssignedMaterialInstance()
            {
                return mMaterialInstance;
            }

        private_members:
            std::string                        mName;
            CStdSharedPtr_t<CMaterialInstance> mMaterialInstance;
            bool                               mEnabled;
        };

        /**
         * The CMaterial class encapsulates at least one material layer
         * and serves as the public access point to the entire material
         * structure.
         */
        class CMaterial
        {
        public_constructors:
        public_destructors:
        public_operators:
        public_methods:
            /**
             *
             *
             * @param aLayerId
             * @return
             */
            SHIRABE_INLINE
            CEngineResult<CMaterialLayer *> addLayer(std::string const &aLayerId)
            {
                bool const has = hasLayer(aLayerId);
                if(has)
                {
                    return { EEngineStatus::Error, nullptr };
                }

                CMaterialLayer &layer = mLayers[aLayerId] = { aLayerId };
                return { EEngineStatus::Ok,  &layer };
            }

            /**
             *
             *
             * @param aLayerId
             * @return
             */
            SHIRABE_INLINE
            CEngineResult<CMaterialLayer *> getLayer(std::string const &aLayerId)
            {
                bool const has = hasLayer(aLayerId);
                if(not has)
                {
                    return { EEngineStatus::Error, nullptr };
                }

                CMaterialLayer &layer = mLayers.at(aLayerId);
                return { EEngineStatus::Ok, &layer };
            }

        private_methods:
            /**
             *
             *
             * @param aLayerId
             * @return
             */
            SHIRABE_INLINE bool hasLayer(std::string const &aLayerId) const
            {
                bool const has = (mLayers.end() != mLayers.find(aLayerId));
                return has;
            }

        private_static_fields:
            static CMaterialLayer const sEmptyLayer;

        private_members:
            std::string                      mName;
            Map<std::string, CMaterialLayer> mLayers;
        };

    }
}

#endif // MATERIAL_DECLARATION_H
