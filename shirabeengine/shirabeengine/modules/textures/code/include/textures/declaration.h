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
        struct STextureMeta
                : engine::serialization::ISerializable<documents::IJSONSerializer<STextureMeta>>
                , engine::serialization::IDeserializable<documents::IJSONDeserializer<STextureMeta>>
        {
        public_constructors:
            SHIRABE_INLINE
            STextureMeta()
                    : serialization::ISerializable<documents::IJSONSerializer<STextureMeta>>()
                    , serialization::IDeserializable<documents::IJSONDeserializer<STextureMeta>>()
                    , uid                  (0 )
                    , name                 ({})
                    , imageLayersBinaryUids(0 )
            {}

            SHIRABE_INLINE
            STextureMeta(STextureMeta const &aOther)
                    : serialization::ISerializable<documents::IJSONSerializer<STextureMeta>>()
                    , serialization::IDeserializable<documents::IJSONDeserializer<STextureMeta>>()
                    , uid                  (aOther.uid                  )
                    , name                 (aOther.name                 )
                    , imageLayersBinaryUids(aOther.imageLayersBinaryUids)
            {}

            SHIRABE_INLINE
            STextureMeta(STextureMeta &&aOther) noexcept
                    : serialization::ISerializable<documents::IJSONSerializer<STextureMeta>>()
                    , serialization::IDeserializable<documents::IJSONDeserializer<STextureMeta>>()
                    , uid                  (aOther.uid                  )
                    , name                 (std::move(aOther.name       ))
                    , imageLayersBinaryUids(aOther.imageLayersBinaryUids)
            {}

        public_operators:
            SHIRABE_INLINE
            STextureMeta &operator=(STextureMeta const &aOther)
            {
                uid                   = aOther.uid;
                name                  = aOther.name;
                imageLayersBinaryUids = aOther.imageLayersBinaryUids;

                return (*this);
            }

            SHIRABE_INLINE
            STextureMeta &operator=(STextureMeta &&aOther) noexcept
            {
                uid                   = aOther.uid;
                name                  = std::move(aOther.name);
                imageLayersBinaryUids = aOther.imageLayersBinaryUids;

                return (*this);
            }

        public_members:
            uint64_t                 uid;
            std::string              name;
            asset::STextureInfo      textureInfo;
            Vector<asset::AssetId_t> imageLayersBinaryUids;

        public_methods:
            /**
             * @brief acceptSerializer
             * @param aSerializer
             * @return
             */
            bool acceptSerializer(documents::IJSONSerializer<STextureMeta> &aSerializer) const final;

            /**
             * @brief acceptDeserializer
             * @param aSerializer
             * @return
             */
            bool acceptDeserializer(documents::IJSONDeserializer<STextureMeta> &aDeserializer) final;
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
            public_constructors:
                SHIRABE_INLINE
                explicit CTextureInstance(  std::string              const &aName
                                          , Vector<asset::AssetId_t> const &aImageLayersBinaryAssetUIDs)
                    : mName                      ( aName )
                    , mImageLayersBinaryAssetUids(aImageLayersBinaryAssetUIDs)
                {}

                SHIRABE_INLINE
                explicit CTextureInstance(CTextureInstance &&aOther)
                    : mName                      (std::move(aOther.mName))
                    , mImageLayersBinaryAssetUids(aOther.mImageLayersBinaryAssetUids)
                {}

            public_destructors:
                ~CTextureInstance() = default;

            public_operators:
                SHIRABE_INLINE
                CTextureInstance &operator=(CTextureInstance &&aOther)
                {
                    mName                       = std::move(aOther.mName);
                    mImageLayersBinaryAssetUids = aOther.mImageLayersBinaryAssetUids;

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
                Vector<asset::AssetId_t> const imageLayersBinaryAssetUids() const
                {
                    return mImageLayersBinaryAssetUids;
                }

        private_members:
            std::string              mName;
            asset::STextureInfo      mTextureInfo;
            Vector<asset::AssetId_t> mImageLayersBinaryAssetUids;
        };


    }
}

#endif
