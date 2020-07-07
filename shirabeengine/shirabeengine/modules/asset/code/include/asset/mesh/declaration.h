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
        struct SMeshAsset
                : engine::serialization::ISerializable<documents::IJSONSerializer<SMeshAsset>>
                , engine::serialization::IDeserializable<documents::IJSONDeserializer<SMeshAsset>>
        {
        public_members:
            uint64_t                               uid;
            std::string                            name;
            std::vector<SMeshAttributeDescription> attributes;
            SMeshAttributeDescription              indices;
            uint32_t                               attributeSampleCount;
            uint32_t                               indexSampleCount;
            std::filesystem::path                  binaryFilename;

        public_constructors:
            SHIRABE_INLINE
            SMeshAsset()
                : serialization::ISerializable<documents::IJSONSerializer<SMeshAsset>>()
                , serialization::IDeserializable<documents::IJSONDeserializer<SMeshAsset>>()
                , uid                  (0 )
                , name                 ({})
                , attributes           ({})
                , indices              ({})
                , binaryFilename       ()
                , attributeSampleCount (0)
                , indexSampleCount     (0)
            {}

            SHIRABE_INLINE
            SMeshAsset(SMeshAsset const &aOther)
                : serialization::ISerializable<documents::IJSONSerializer<SMeshAsset>>()
                , serialization::IDeserializable<documents::IJSONDeserializer<SMeshAsset>>()
                , uid                  (aOther.uid                  )
                , name                 (aOther.name                 )
                , attributes           (aOther.attributes           )
                , indices              (aOther.indices              )
                , binaryFilename       (aOther.binaryFilename       )
                , attributeSampleCount (aOther.attributeSampleCount )
                , indexSampleCount     (aOther.indexSampleCount     )
            {}

            SHIRABE_INLINE
            SMeshAsset(SMeshAsset &&aOther) noexcept
                : serialization::ISerializable<documents::IJSONSerializer<SMeshAsset>>()
                , serialization::IDeserializable<documents::IJSONDeserializer<SMeshAsset>>()
                , uid                  (aOther.uid                  )
                , name                 (std::move(aOther.name      ))
                , attributes           (std::move(aOther.attributes))
                , indices              (aOther.indices              )
                , binaryFilename       (aOther.binaryFilename       )
                , attributeSampleCount (aOther.attributeSampleCount )
                , indexSampleCount     (aOther.indexSampleCount     )
            {}

        public_operators:
            SHIRABE_INLINE
            SMeshAsset &operator=(SMeshAsset const &aOther)
            {
                uid                  = aOther.uid;
                name                 = aOther.name;
                attributes           = aOther.attributes;
                indices              = aOther.indices;
                binaryFilename       = aOther.binaryFilename;
                attributeSampleCount = aOther.attributeSampleCount;
                indexSampleCount     = aOther.indexSampleCount;

                return (*this);
            }

            SHIRABE_INLINE
            SMeshAsset &operator=(SMeshAsset &&aOther) noexcept
            {
                uid                  = aOther.uid;
                name                 = std::move(aOther.name);
                attributes           = std::move(aOther.attributes);
                indices              = aOther.indices;
                binaryFilename       = aOther.binaryFilename;
                attributeSampleCount = aOther.attributeSampleCount;
                indexSampleCount     = aOther.indexSampleCount;

                return (*this);
            }

        public_methods:
            /**
             * @brief acceptSerializer
             * @param aSerializer
             * @return
             */
            bool acceptSerializer(documents::IJSONSerializer<SMeshAsset> &aSerializer) const final;

            /**
             * @brief acceptDeserializer
             * @param aSerializer
             * @return
             */
            bool acceptDeserializer(documents::IJSONDeserializer<SMeshAsset> &aDeserializer) final;
        };

        class CMeshInstance
        {
        public_static_functions:
            static Shared<CMeshInstance> fromAsset(SMeshAsset const &aMeshAsset);

        public_constructors:
            CMeshInstance() = default;

            SHIRABE_INLINE
            CMeshInstance(asset::AssetId_t const &aAssetUID,
                          std::string      const &aName)
                : mName(aName)
            {}

            SHIRABE_INLINE
            CMeshInstance(CMeshInstance const &aOther)
                : mName(aOther.mName)
            {}

            SHIRABE_INLINE
            CMeshInstance(CMeshInstance &&aOther)
                : mName(std::move(aOther.mName))
            {}

        public_destructors:
            ~CMeshInstance() = default;

        public_operators:
            SHIRABE_INLINE
            CMeshInstance &operator=(CMeshInstance const &aOther)
            {
                mName = aOther.mName;

                return (*this);
            }

            SHIRABE_INLINE
            CMeshInstance &operator=(CMeshInstance &&aOther) noexcept
            {

                mName = std::move(aOther.mName);

                return (*this);
            }

        public_methods:
            SHIRABE_INLINE
            std::string const &name() const
            {
                return mName;
            };

            SHIRABE_INLINE std::vector<SMeshAttributeDescription> const &attributes()           const { return mAttributes;           }
            SHIRABE_INLINE SMeshAttributeDescription              const &indices()              const { return mIndices;              }
            SHIRABE_INLINE uint32_t                               const  attributeSampleCount() const { return mAttributeSampleCount; }
            SHIRABE_INLINE uint32_t                               const  indexSampleCount()     const { return mIndexSampleCount;     }
            SHIRABE_INLINE std::filesystem::path                  const  binaryFilename()       const { return mBinaryFilename;       }

        private_methods:
            friend class CMeshLoader; // The below private methods are exclusively to be invoked by the material loader. Ensure this...

        private_members:
            std::string                            mName;
            std::vector<SMeshAttributeDescription> mAttributes;
            SMeshAttributeDescription              mIndices;
            uint32_t                               mAttributeSampleCount;
            uint32_t                               mIndexSampleCount;
            std::filesystem::path                  mBinaryFilename;
        };
    }
}

#endif // MESH_DECLARATION_H
