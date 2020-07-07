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
#include <core/memory/allocators/allocators.h>
#include <core/serialization/serialization.h>
#include <asset/assettypes.h>
#include <asset/material/asset.h>

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
         * A material sharedMaterial is composed by a signature and base configuration.
         * It will be used to create instances from this material.
         */
        class CSharedMaterial
        {
        public_static_functions:
            static Shared<CSharedMaterial> fromAsset(SMaterialAsset const &aAsset);

        public_constructors:
            CSharedMaterial() = default;

            SHIRABE_INLINE
            CSharedMaterial(asset::AssetId_t   const &aAssetUID,
                            std::string        const &aName,
                            SMaterialAsset          &&aSignature)
                : mName          (aName)
                , mLayoutInfo    ({})
                , mStages        ({})
                , mUniformBuffers({})
                , mStorageBuffers({})
                , mSampledImages ({})
                , mSubpassInputs ({})
            {}

            SHIRABE_INLINE
            CSharedMaterial(CSharedMaterial const &aOther)
                : mName           (aOther.mName)
                , mLayoutInfo     (aOther.mLayoutInfo)
                , mStages         (aOther.mStages)
                , mUniformBuffers (aOther.mUniformBuffers)
                , mStorageBuffers (aOther.mStorageBuffers)
                , mSampledImages  (aOther.mSampledImages)
                , mSubpassInputs  (aOther.mSubpassInputs)
            {}

            SHIRABE_INLINE
            CSharedMaterial(CSharedMaterial &&aOther)
                : mName         (std::move(aOther.mName))
                , mLayoutInfo    (aOther.mLayoutInfo)
                , mStages        (std::move(aOther.mStages))
                , mUniformBuffers(aOther.mUniformBuffers)
                , mStorageBuffers(aOther.mStorageBuffers)
                , mSampledImages (aOther.mSampledImages)
                , mSubpassInputs (aOther.mSubpassInputs)
            {}

        public_destructors:
            ~CSharedMaterial() = default;

        public_operators:
            SHIRABE_INLINE
            CSharedMaterial &operator=(CSharedMaterial const &aOther)
            {
                mName           = aOther.mName;
                mLayoutInfo     = aOther.mLayoutInfo;
                mStages         = aOther.mStages;
                mUniformBuffers = aOther.mUniformBuffers;
                mStorageBuffers = aOther.mStorageBuffers;
                mSampledImages  = aOther.mSampledImages;
                mSubpassInputs  = aOther.mSubpassInputs;

                return (*this);
            }

            SHIRABE_INLINE
            CSharedMaterial &operator=(CSharedMaterial &&aOther) noexcept
            {
                mName           = std::move(aOther.mName);
                mLayoutInfo     = aOther.mLayoutInfo;
                mStages         = std::move(aOther.mStages);
                mUniformBuffers = aOther.mUniformBuffers;
                mStorageBuffers = aOther.mStorageBuffers;
                mSampledImages  = aOther.mSampledImages;
                mSubpassInputs  = aOther.mSubpassInputs;

                return (*this);
            }

        public_methods:
            EEngineStatus initializeMemory(Shared<memory::allocators::CAllocator> aAllocator);

            SHIRABE_INLINE std::string                 const &name()           const { return mName; }
            SHIRABE_INLINE SMaterialLayoutInfo         const &layoutInfo()     const { return mLayoutInfo; }
            SHIRABE_INLINE StageMap_t                  const &stages()         const { return mStages; }
            SHIRABE_INLINE std::vector<SUniformBuffer> const &uniformBuffers() const { return mUniformBuffers; }
            SHIRABE_INLINE std::vector<SUniformBuffer> const &storageBuffers() const { return mStorageBuffers; }
            SHIRABE_INLINE std::vector<SSampledImage>  const &sampledImages()  const { return mSampledImages; }
            SHIRABE_INLINE std::vector<SSubpassInput>  const &subpassInputs()  const { return mSubpassInputs; }

        private_methods:
            friend class CMaterialLoader; // The below private methods are exclusively to be invoked by the material loader. Ensure this...

        private_members:
            std::string                 mName;
            //std::unordered_map<VkPipelineStageFlagBits, SMaterialMetaStage> stages;
            SMaterialLayoutInfo         mLayoutInfo;
            StageMap_t                  mStages;
            // Although each stage defines uniform buffers individually, they are shared
            // across all stages, due to indexing them with set and binding.
            std::vector<SUniformBuffer> mUniformBuffers;
            std::vector<SUniformBuffer> mStorageBuffers;
            std::vector<SSampledImage>  mSampledImages;
            // Fragment shader only
            std::vector<SSubpassInput>  mSubpassInputs;

            Shared<memory::allocators::CPoolAllocator> mConfigPoolAllocator;
        };

        Shared<CSharedMaterial> CSharedMaterial::fromAsset(SMaterialAsset const &aAsset)
        {
            Shared<CSharedMaterial> instance = makeShared<CSharedMaterial>();

            instance->mName           = aAsset.name;
            instance->mLayoutInfo     = aAsset.layoutInfo;
            instance->mStages         = aAsset.stages;
            instance->mUniformBuffers = aAsset.uniformBuffers;
            instance->mStorageBuffers = aAsset.storageBuffers;
            instance->mSampledImages  = aAsset.sampledImages;
            instance->mSubpassInputs  = aAsset.subpassInputs;

            return instance;
        }

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

        public_typedefs:
            using BufferValueIndex_t = Map<std::string, Shared<SBufferMember>>;
            using BufferIndex_t      = Map<std::string, BufferValueIndex_t>;
            using BufferData_t       = Map<std::string, Shared<void>>;
            using SampledImageMap_t  = Map<std::string, asset::AssetId_t>;

        public_static_functions:
            static CMaterialConfig fromMaterialDesc(CSharedMaterial const &aMaterial, bool aIncludeSystemBuffers = false);

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

            SHIRABE_INLINE void setSampledImage(std::string const &aSlotId, asset::AssetId_t const &aSampledImageResourceId)
            {
                auto const it = std::find_if(mSampledImageIndex.begin(), mSampledImageIndex.end(), [&](std::string const &aCmp) -> bool { return (aCmp == aSlotId); });
                if(mSampledImageIndex.end() != it)
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
         * A material instance describes a configurable and bindable material state which can be imagined
         * to be a single render call for an object having this material assigned.
         *
         * Instances track their master during edit-time, in case the sharedMaterial signature changes due to
         * base configuration value changes or shader file updates.
         */
        class CMaterialInstance
        {
            public_constructors:
                SHIRABE_INLINE
                explicit CMaterialInstance(std::string             const &aName,
                                           Shared<CSharedMaterial>        aSharedMaterial)
                    : mName           ( aName            )
                    , mConfiguration  (                  )
                    , mSharedMaterial(std::move(aSharedMaterial))
                {}

                SHIRABE_INLINE
                explicit CMaterialInstance(CMaterialInstance &&aOther)
                    : mName         (std::move(aOther.mName           ))
                    , mConfiguration(std::move(aOther.mConfiguration  ))
                    , mSharedMaterial(std::move(aOther.mSharedMaterial))
                {}

            public_destructors:
                ~CMaterialInstance() = default;

            public_operators:
                SHIRABE_INLINE
                CMaterialInstance &operator=(CMaterialInstance &&aOther)
                {
                    mName           = std::move(aOther.mName           );
                    mSharedMaterial = std::move(aOther.mSharedMaterial);
                    mConfiguration  = std::move(aOther.mConfiguration  );

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
                Shared<CSharedMaterial> const &sharedMaterial() const
                {
                    return mSharedMaterial;
                }

                EEngineStatus createConfiguration(CSharedMaterial const &aAsset, bool aIncludeSystemBuffers = false);

        private_members:
            std::string                    mName;
            std::optional<CMaterialConfig> mConfiguration;
            Shared<CSharedMaterial>        mSharedMaterial;
        };


    }
}

#endif // MATERIAL_DECLARATION_H
