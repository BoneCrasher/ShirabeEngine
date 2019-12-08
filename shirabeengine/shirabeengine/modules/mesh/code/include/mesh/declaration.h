#ifndef __SHIRABE_MESH_DECLARATION_H__
#define __SHIRABE_MESH_DECLARATION_H__

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

    namespace mesh
    {
        struct SMeshAttributeDescription
        {
            std::string name;
            uint64_t    index;
            uint64_t    offset;
            uint64_t    length;
            uint64_t    bytesPerSample;
        };

        /**
         * The SMaterialIndex describes all necessary data for a basic material composition
         * in the engine.
         * The specific implementation will be provided in files referenced by the stages member.
         */
        struct SMeshMeta
                : engine::serialization::ISerializable<documents::IJSONSerializer<SMeshMeta>>
                , engine::serialization::IDeserializable<documents::IJSONDeserializer<SMeshMeta>>
        {
        public_constructors:
            SHIRABE_INLINE
            SMeshMeta()
                    : serialization::ISerializable<documents::IJSONSerializer<SMeshMeta>>()
                    , serialization::IDeserializable<documents::IJSONDeserializer<SMeshMeta>>()
                    , uid       (0 )
                    , name      ({})
                    , dataFileId(0)
            {}

            SHIRABE_INLINE
            SMeshMeta(SMeshMeta const &aOther)
                    : serialization::ISerializable<documents::IJSONSerializer<SMeshMeta>>()
                    , serialization::IDeserializable<documents::IJSONDeserializer<SMeshMeta>>()
                    , uid       (aOther.uid                  )
                    , name      (aOther.name                 )
                    , dataFileId(aOther.dataFileId           )
            {}

            SHIRABE_INLINE
            SMeshMeta(SMeshMeta &&aOther) noexcept
                    : serialization::ISerializable<documents::IJSONSerializer<SMeshMeta>>()
                    , serialization::IDeserializable<documents::IJSONDeserializer<SMeshMeta>>()
                    , uid       (aOther.uid                  )
                    , name      (std::move(aOther.name      ))
                    , dataFileId(aOther.dataFileId           )
            {}

        public_operators:
            SHIRABE_INLINE
            SMeshMeta &operator=(SMeshMeta const &aOther)
            {
                uid        = aOther.uid;
                name       = aOther.name;
                dataFileId = aOther.dataFileId;

                return (*this);
            }

            SHIRABE_INLINE
            SMeshMeta &operator=(SMeshMeta &&aOther) noexcept
            {
                uid        = aOther.uid;
                name       = std::move(aOther.name);
                dataFileId = aOther.dataFileId;

                return (*this);
            }

        public_members:
            uint64_t         uid;
            std::string      name;
            asset::AssetId_t dataFileId;

        public_methods:
            /**
             * @brief acceptSerializer
             * @param aSerializer
             * @return
             */
            bool acceptSerializer(documents::IJSONSerializer<SMeshMeta> &aSerializer) const final;

            /**
             * @brief acceptDeserializer
             * @param aSerializer
             * @return
             */
            bool acceptDeserializer(documents::IJSONDeserializer<SMeshMeta> &aDeserializer) final;
        };

        /**
         * The SMaterialIndex describes all necessary data for a basic material composition
         * in the engine.
         * The specific implementation will be provided in files referenced by the stages member.
         */
        struct SMeshDataFile
                : engine::serialization::ISerializable<documents::IJSONSerializer<SMeshDataFile>>
                  , engine::serialization::IDeserializable<documents::IJSONDeserializer<SMeshDataFile>>
        {
        public_constructors:
            SHIRABE_INLINE
            SMeshDataFile()
                    : serialization::ISerializable<documents::IJSONSerializer<SMeshDataFile>>()
                      , serialization::IDeserializable<documents::IJSONDeserializer<SMeshDataFile>>()
                      , uid                  (0 )
                      , name                 ({})
                      , attributes           ({})
                      , indices              ({})
                      , dataBinaryFilename   ()
                      , indexBinaryFilename  ()
            {}

            SHIRABE_INLINE
            SMeshDataFile(SMeshDataFile const &aOther)
                    : serialization::ISerializable<documents::IJSONSerializer<SMeshDataFile>>()
                      , serialization::IDeserializable<documents::IJSONDeserializer<SMeshDataFile>>()
                      , uid                  (aOther.uid                  )
                      , name                 (aOther.name                 )
                      , attributes           (aOther.attributes           )
                      , indices              (aOther.indices              )
                      , dataBinaryFilename   (aOther.dataBinaryFilename   )
                      , indexBinaryFilename  (aOther.indexBinaryFilename  )
            {}

            SHIRABE_INLINE
            SMeshDataFile(SMeshDataFile &&aOther) noexcept
                    : serialization::ISerializable<documents::IJSONSerializer<SMeshDataFile>>()
                      , serialization::IDeserializable<documents::IJSONDeserializer<SMeshDataFile>>()
                      , uid                  (aOther.uid                  )
                      , name                 (std::move(aOther.name      ))
                      , attributes           (std::move(aOther.attributes))
                      , indices              (aOther.indices              )
                      , dataBinaryFilename   (aOther.dataBinaryFilename   )
                      , indexBinaryFilename  (aOther.indexBinaryFilename  )
            {}

        public_operators:
            SHIRABE_INLINE
            SMeshDataFile &operator=(SMeshDataFile const &aOther)
            {
                uid                 = aOther.uid;
                name                = aOther.name;
                attributes          = aOther.attributes;
                indices             = aOther.indices;
                dataBinaryFilename  = aOther.dataBinaryFilename;
                indexBinaryFilename = aOther.indexBinaryFilename;

                return (*this);
            }

            SHIRABE_INLINE
            SMeshDataFile &operator=(SMeshDataFile &&aOther) noexcept
            {
                uid                 = aOther.uid;
                name                = std::move(aOther.name);
                attributes          = std::move(aOther.attributes);
                indices             = aOther.indices;
                dataBinaryFilename  = aOther.dataBinaryFilename;
                indexBinaryFilename = aOther.indexBinaryFilename;

                return (*this);
            }

        public_members:
            uint64_t                               uid;
            std::string                            name;
            std::vector<SMeshAttributeDescription> attributes;
            SMeshAttributeDescription              indices;
            std::filesystem::path                  dataBinaryFilename;
            std::filesystem::path                  indexBinaryFilename;

        public_methods:
            /**
             * @brief acceptSerializer
             * @param aSerializer
             * @return
             */
            bool acceptSerializer(documents::IJSONSerializer<SMeshDataFile> &aSerializer) const final;

            /**
             * @brief acceptDeserializer
             * @param aSerializer
             * @return
             */
            bool acceptDeserializer(documents::IJSONDeserializer<SMeshDataFile> &aDeserializer) final;
        };

        class CMeshInstance
            : public asset::CAssetReference
        {
        public_constructors:
            CMeshInstance() = default;

            SHIRABE_INLINE
            CMeshInstance(asset::AssetId_t   const &aAssetUID,
                          std::string        const &aName,
                          SMeshDataFile           &&aMeshDataFile)
                : asset::CAssetReference (aAssetUID)
                , mName                  (aName                )
                , mMeshDataFile          (std::move(aMeshDataFile))
            {}

            SHIRABE_INLINE
            CMeshInstance(CMeshInstance const &aOther)
                : asset::CAssetReference (aOther.getAssetId()  )
                , mName                  (aOther.mName         )
                , mMeshDataFile          (aOther.mMeshDataFile )
            {}

            SHIRABE_INLINE
            CMeshInstance(CMeshInstance &&aOther)
                : asset::CAssetReference (aOther.getAssetId()            )
                , mName                  (std::move(aOther.mName        ))
                , mMeshDataFile          (std::move(aOther.mMeshDataFile))
            {}

        public_destructors:
            ~CMeshInstance() = default;

        public_operators:
            SHIRABE_INLINE
            CMeshInstance &operator=(CMeshInstance const &aOther)
            {
                asset::CAssetReference::operator=(aOther.getAssetId());

                mName         = aOther.mName;
                mMeshDataFile = aOther.mMeshDataFile;

                return (*this);
            }

            SHIRABE_INLINE
            CMeshInstance &operator=(CMeshInstance &&aOther) noexcept
            {
                asset::CAssetReference::operator=(aOther.getAssetId());

                mName         = std::move(aOther.mName        );
                mMeshDataFile = std::move(aOther.mMeshDataFile);

                return (*this);
            }

        public_methods:
            SHIRABE_INLINE
            std::string const &name() const
            {
                return mName;
            };

            SHIRABE_INLINE
            SMeshDataFile const &dataFile() const
            {
                return mMeshDataFile;
            }

        private_methods:
            friend class CMeshLoader; // The below private methods are exclusively to be invoked by the material loader. Ensure this...

        private_members:
            std::string   mName;
            SMeshDataFile mMeshDataFile;
        };

    }
}

#endif // MESH_DECLARATION_H
