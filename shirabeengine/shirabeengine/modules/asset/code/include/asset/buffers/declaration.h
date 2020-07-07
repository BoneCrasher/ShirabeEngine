#ifndef __SHIRABE_BUFFER_DECLARATION_H__
#define __SHIRABE_BUFFER_DECLARATION_H__

#include <cassert>
#include <string>
#include <vector>
#include <cstdint>
#include <filesystem>
#include <cstring>
#include <unordered_map>

#include <platform/platform.h>

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

    namespace buffers
    {
        /**
         * The SMaterialIndex describes all necessary data for a basic material composition
         * in the engine.
         * The specific implementation will be provided in files referenced by the stages member.
         */
        struct SBufferAsset
                : engine::serialization::ISerializable<documents::IJSONSerializer<SBufferAsset>>
                , engine::serialization::IDeserializable<documents::IJSONDeserializer<SBufferAsset>>
        {
        public_constructors:
            SHIRABE_INLINE
            SBufferAsset()
                : serialization::ISerializable<documents::IJSONSerializer<SBufferAsset>>()
                , serialization::IDeserializable<documents::IJSONDeserializer<SBufferAsset>>()
                , uid           (0 )
                , name          ({})
                , bufferSize    (0)
                , binaryFilename()
            {}

            SHIRABE_INLINE
            SBufferAsset(SBufferAsset const &aOther)
                : serialization::ISerializable<documents::IJSONSerializer<SBufferAsset>>()
                , serialization::IDeserializable<documents::IJSONDeserializer<SBufferAsset>>()
                , uid           (aOther.uid           )
                , name          (aOther.name          )
                , bufferSize    (aOther.bufferSize    )
                , binaryFilename(aOther.binaryFilename)
            {}

            SHIRABE_INLINE
            SBufferAsset(SBufferAsset &&aOther) noexcept
                : serialization::ISerializable<documents::IJSONSerializer<SBufferAsset>>()
                , serialization::IDeserializable<documents::IJSONDeserializer<SBufferAsset>>()
                , uid           (aOther.uid            )
                , name          (std::move(aOther.name))
                , bufferSize    (aOther.bufferSize    )
                , binaryFilename(aOther.binaryFilename)
            {}

        public_operators:
            SHIRABE_INLINE
            SBufferAsset &operator=(SBufferAsset const &aOther)
            {
                uid            = aOther.uid;
                name           = aOther.name;
                bufferSize     = aOther.bufferSize;
                binaryFilename = aOther.binaryFilename;

                return (*this);
            }

            SHIRABE_INLINE
            SBufferAsset &operator=(SBufferAsset &&aOther) noexcept
            {
                uid            = aOther.uid;
                name           = aOther.name;
                bufferSize     = aOther.bufferSize;
                binaryFilename = aOther.binaryFilename;

                return (*this);
            }

        public_members:
            uint64_t              uid;
            std::string           name;
            uint64_t              bufferSize;
            std::filesystem::path binaryFilename;

        public_methods:
            /**
             * @brief acceptSerializer
             * @param aSerializer
             * @return
             */
            bool acceptSerializer(documents::IJSONSerializer<SBufferAsset> &aSerializer) const final;

            /**
             * @brief acceptDeserializer
             * @param aSerializer
             * @return
             */
            bool acceptDeserializer(documents::IJSONDeserializer<SBufferAsset> &aDeserializer) final;
        };

        class CBufferInstance
        {
        public_static_functions:
            static Shared<CBufferInstance> fromAsset(SBufferAsset const &aBufferAsset);

        public_constructors:
            CBufferInstance() = default;

            SHIRABE_INLINE
            CBufferInstance(std::string const &aName)
                : mName(aName)
            {}

            SHIRABE_INLINE
            CBufferInstance(CBufferInstance const &aOther)
                : mName(aOther.mName)
            {}

            SHIRABE_INLINE
            CBufferInstance(CBufferInstance &&aOther)
                : mName(std::move(aOther.mName))
            {}

        public_destructors:
            ~CBufferInstance() = default;

        public_operators:
            SHIRABE_INLINE
            CBufferInstance &operator=(CBufferInstance const &aOther)
            {
                mName = aOther.mName;

                return (*this);
            }

            SHIRABE_INLINE
            CBufferInstance &operator=(CBufferInstance &&aOther) noexcept
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

        private_methods:
            friend class CBufferLoader; // The below private methods are exclusively to be invoked by the material loader. Ensure this...

        private_members:
            std::string           mName;
            uint64_t              mBufferSize;
            std::filesystem::path mBinaryFilename;
        };
    }
}

#endif // MESH_DECLARATION_H
