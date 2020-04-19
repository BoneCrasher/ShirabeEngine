#ifndef __SHIRABE_TEXTURE_DECLARATION_H__
#define __SHIRABE_TEXTURE_DECLARATION_H__

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

    namespace textures
    {
        /**
         * The SMaterialIndex describes all necessary data for a basic material composition
         * in the engine.
         * The specific implementation will be provided in files referenced by the stages member.
         */
        struct STextureFile
                : engine::serialization::ISerializable<documents::IJSONSerializer<STextureFile>>
                , engine::serialization::IDeserializable<documents::IJSONDeserializer<STextureFile>>
        {
        public_constructors:
            SHIRABE_INLINE
            STextureFile()
                    : serialization::ISerializable<documents::IJSONSerializer<STextureFile>>()
                      , serialization::IDeserializable<documents::IJSONDeserializer<STextureFile>>()
                      , uid                   (0 )
                      , name                  ({})
                      , textureSourceFilenames(0)
            {}

            SHIRABE_INLINE
            STextureFile(STextureFile const &aOther)
                    : serialization::ISerializable<documents::IJSONSerializer<STextureFile>>()
                      , serialization::IDeserializable<documents::IJSONDeserializer<STextureFile>>()
                      , uid                   (aOther.uid                  )
                      , name                  (aOther.name                 )
                      , textureSourceFilenames(aOther.textureSourceFilenames)
            {}

            SHIRABE_INLINE
            STextureFile(STextureFile &&aOther) noexcept
                    : serialization::ISerializable<documents::IJSONSerializer<STextureFile>>()
                    , serialization::IDeserializable<documents::IJSONDeserializer<STextureFile>>()
                    , uid                   (aOther.uid                             )
                    , name                  (std::move(aOther.name                  ))
                    , textureSourceFilenames(std::move(aOther.textureSourceFilenames))
            {}

        public_operators:
            SHIRABE_INLINE
            STextureFile &operator=(STextureFile const &aOther)
            {
                uid                    = aOther.uid;
                name                   = aOther.name;
                textureSourceFilenames = aOther.textureSourceFilenames;

                return (*this);
            }

            SHIRABE_INLINE
            STextureFile &operator=(STextureFile &&aOther) noexcept
            {
                uid                    = aOther.uid;
                name                   = std::move(aOther.name);
                textureSourceFilenames = aOther.textureSourceFilenames;

                return (*this);
            }

        public_members:
            uint64_t                      uid;
            std::string                   name;
            Vector<std::filesystem::path> textureSourceFilenames;

        public_methods:
            /**
             * @brief acceptSerializer
             * @param aSerializer
             * @return
             */
            bool acceptSerializer(documents::IJSONSerializer<STextureFile> &aSerializer) const final;

            /**
             * @brief acceptDeserializer
             * @param aSerializer
             * @return
             */
            bool acceptDeserializer(documents::IJSONDeserializer<STextureFile> &aDeserializer) final;
        };

        /**
         * The SMaterialIndex describes all necessary data for a basic material composition
         * in the engine.
         * The specific implementation will be provided in files referenced by the stages member.
         */
        struct STextureAsset
                : engine::serialization::ISerializable<documents::IJSONSerializer<STextureAsset>>
                , engine::serialization::IDeserializable<documents::IJSONDeserializer<STextureAsset>>
        {
        public_constructors:
            SHIRABE_INLINE
            STextureAsset()
                    : serialization::ISerializable<documents::IJSONSerializer<STextureAsset>>()
                    , serialization::IDeserializable<documents::IJSONDeserializer<STextureAsset>>()
                    , uid                 (0 )
                    , name                ({})
                    , textureInfo         ({})
                    , imageLayersBinaryUid(0 )
            {}

            SHIRABE_INLINE
            STextureAsset(STextureAsset const &aOther)
                    : serialization::ISerializable<documents::IJSONSerializer<STextureAsset>>()
                    , serialization::IDeserializable<documents::IJSONDeserializer<STextureAsset>>()
                    , uid                 (aOther.uid                  )
                    , name                (aOther.name                 )
                    , textureInfo         (aOther.textureInfo)
                    , imageLayersBinaryUid(aOther.imageLayersBinaryUid)
            {}

            SHIRABE_INLINE
            STextureAsset(STextureAsset &&aOther) noexcept
                    : serialization::ISerializable<documents::IJSONSerializer<STextureAsset>>()
                    , serialization::IDeserializable<documents::IJSONDeserializer<STextureAsset>>()
                    , uid                 (aOther.uid                  )
                    , name                (std::move(aOther.name       ))
                    , textureInfo         (aOther.textureInfo          )
                    , imageLayersBinaryUid(aOther.imageLayersBinaryUid )
            {}

        public_operators:
            SHIRABE_INLINE
            STextureAsset &operator=(STextureAsset const &aOther)
            {
                uid                  = aOther.uid;
                name                 = aOther.name;
                textureInfo          = aOther.textureInfo;
                imageLayersBinaryUid = aOther.imageLayersBinaryUid;

                return (*this);
            }

            SHIRABE_INLINE
            STextureAsset &operator=(STextureAsset &&aOther) noexcept
            {
                uid                  = aOther.uid;
                name                 = std::move(aOther.name);
                textureInfo          = aOther.textureInfo;
                imageLayersBinaryUid = aOther.imageLayersBinaryUid;

                return (*this);
            }

        public_members:
            uint64_t            uid;
            std::string         name;
            asset::STextureInfo textureInfo;
            asset::AssetId_t    imageLayersBinaryUid;

        public_methods:
            /**
             * @brief acceptSerializer
             * @param aSerializer
             * @return
             */
            bool acceptSerializer(documents::IJSONSerializer<STextureAsset> &aSerializer) const final;

            /**
             * @brief acceptDeserializer
             * @param aSerializer
             * @return
             */
            bool acceptDeserializer(documents::IJSONDeserializer<STextureAsset> &aDeserializer) final;
        };

        /**
         * A material instance describes a configurable and bindable material state which can be imagined
         * to be a single render call for an object having this material assigned.
         *
         * Instances track their master during edit-time, in case the master signature changes due to
         * base configuration value changes or shader file updates.
         */
        class CTextureInstance
        {
            public_static_functions:
                static Shared<CTextureInstance> fromAsset(STextureAsset const &aAsset);

            public_constructors:
                SHIRABE_INLINE
                explicit CTextureInstance()
                    : mName                     ("Unnamed")
                    , mTextureInfo              ({})
                    , mImageLayersBinaryAssetUid(0)
                {}

                SHIRABE_INLINE
                explicit CTextureInstance(  std::string         const &aName
                                          , asset::STextureInfo const &aTextureInfo
                                          , asset::AssetId_t    const &aImageLayersBinaryAssetUIDs)
                    : mName                     ( aName )
                    , mTextureInfo              ( aTextureInfo )
                    , mImageLayersBinaryAssetUid(aImageLayersBinaryAssetUIDs)
                {}

                SHIRABE_INLINE
                explicit CTextureInstance(CTextureInstance &&aOther)
                    : mName                     (std::move(aOther.mName))
                    , mTextureInfo              (aOther.mTextureInfo)
                    , mImageLayersBinaryAssetUid(aOther.mImageLayersBinaryAssetUid)
                {}

            public_destructors:
                ~CTextureInstance() = default;

            public_operators:
                SHIRABE_INLINE
                CTextureInstance &operator=(CTextureInstance &&aOther)
                {
                    mName                      = std::move(aOther.mName);
                    mTextureInfo               = aOther.mTextureInfo;
                    mImageLayersBinaryAssetUid = aOther.mImageLayersBinaryAssetUid;

                    return (*this);
                }

        public_methods:
                SHIRABE_INLINE
                std::string const &name() const
                {
                    return mName;
                }

                SHIRABE_INLINE
                asset::STextureInfo const &textureInfo() const
                {
                    return mTextureInfo;
                }

                SHIRABE_INLINE
                asset::AssetId_t const imageLayersBinaryAssetUid() const
                {
                    return mImageLayersBinaryAssetUid;
                }

        private_members:
            std::string         mName;
            asset::STextureInfo mTextureInfo;
            asset::AssetId_t    mImageLayersBinaryAssetUid;
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        Shared<CTextureInstance> CTextureInstance::fromAsset(STextureAsset const &aAsset)
        {
            auto texture = makeShared<CTextureInstance>();

            texture->mName                      = aAsset.name;
            texture->mTextureInfo               = aAsset.textureInfo;
            texture->mImageLayersBinaryAssetUid = aAsset.imageLayersBinaryUid;

            return texture;
        }
        //<-----------------------------------------------------------------------------
    }
}

#endif
