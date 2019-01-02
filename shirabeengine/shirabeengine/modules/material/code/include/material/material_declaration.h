#ifndef __SHIRABE_MATERIAL_DECLARATION_H__
#define __SHIRABE_MATERIAL_DECLARATION_H__

#include <assert.h>
#include <string>
#include <vector>
#include <stdint.h>

#include <base/declaration.h>
#include <platform/platform.h>
#include <core/enginestatus.h>

namespace engine
{
    namespace material
    {

        /**
         * The CMaterialInterface class defines the public interface into the
         * pipeline behind a material layer, which is basically the vertex shader input description.
         */
        class CMaterialInterface
        {
        public_methods:

        private_members:
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
        class CMaterialData
        {
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

        private_structs:
            /**
             * The SBufferLocation struct describes a specific location within a data buffer used to read
             * or write the respective value.
             */
            struct SBufferLocation
            {
                uint64_t offset;
                uint64_t length;
            };

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
        CEngineResult<TDataType const*> CMaterialData::getBufferValuePointer(
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

            return CEngineResult(EEngineStatus::Error, bufferData);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TDataType>
        CEngineResult<TDataType *> CMaterialData::getBufferValuePointer(
                std::string  const &aBufferName,
                std::string  const &aBufferValue)
        {
            // Dirty hack to reuse the function implementation...
            TDataType                       const *constData = nullptr;
            CEngineResult<TDataType const*> const  result    = static_cast<CMaterialData const*>(this)->getBufferValuePointer<TDataType>(aBufferName, aBufferValue);

            return CEngineResult(result.result(), const_cast<TDataType *>(constData));
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TBufferType>
        CEngineResult<TBufferType const*> CMaterialData::getBuffer(std::string const &aBufferName) const
        {
            bool const has = hasBuffer(aBufferName);
            if(not has)
            {
                return CEngineResult(EEngineStatus::Error, nullptr);
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
        CEngineResult<TDataType const*> CMaterialData::getBufferValue(
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
        CEngineResult<> CMaterialData::setBufferValue(
                std::string const &aBufferName,
                std::string const &aFieldName,
                TDataType   const &aFieldValue)
        {
            CEngineResult<TDataType *> result = getBufferValuePointer<TDataType>(aBufferName, aFieldName);
            if(result.successful())
            {
                *(result.data()) = aFieldValue;
            }
        }
        //<-----------------------------------------------------------------------------

        /**
         * The CMaterialLayer class describes a single layer of a material, which can be imagined
         * to be a single render call for an object having this material assigned.
         */
        class CMaterialLayer
        {
        public_methods:
            /**
             * getBuffer
             *
             * @param aBufferName
             * @param aOutBuffer
             * @return
             */
            template <typename TBufferType>
            CEngineResult<TBufferType const *> getBuffer(std::string const &aBufferName)
            {
                return mData.getBuffer<TBufferType>(aBufferName);
            }

            /**
             * getValue
             *
             * @param aBufferName
             * @param aFieldName
             * @param aOutValue
             * @return
             */
            template <typename TDataType>
            CEngineResult<TDataType const *> getValue(
                    std::string const       &aBufferName,
                    std::string const       &aFieldName)
            {
                return mData.getBufferValue<TDataType>(aBufferName, aFieldName);
            }

            /**
             * setValue
             *
             * @param aBufferName
             * @param aFieldName
             * @param aFieldValue
             * @return
             */
            template <typename TDataType>
            CEngineResult<> setValue(
                    std::string const &aBufferName,
                    std::string const &aFieldName,
                    TDataType   const &aFieldValue)
            {
                return mData.setBufferValue<TDataType>(aBufferName, aFieldName, aFieldValue);
            }


        private_members:
            CMaterialData mData;
        };

        /**
         * The CMaterial class encapsulates at least one material layer
         * and serves as the public access point to the entire material
         * structure.
         */
        class CMaterial
        {
        public_methods:
            /**
             * getLayerBuffer
             *
             * @param aLayerId
             * @param aBufferName
             * @param aOutBuffer
             * @return
             */
            template <typename TBufferType>
            CEngineResult<TBufferType const *> getLayerBuffer(
                    std::string const &aLayerId,
                    std::string const &aBufferName)
            {
                bool const layerAvailable = hasLayer(aLayerId);
                if(not layerAvailable)
                {
                    return CEngineResult(EEngineStatus::Error, nullptr);
                }

                CMaterialLayer &layer = mLayers[aLayerId];

                return layer.getBuffer<TBufferType>(aBufferName);
            }

            /**
             * getLayerValue
             *
             * @param aLayerId
             * @param aBufferName
             * @param aFieldName
             * @param aOutValue
             * @return
             */
            template <typename TDataType>
            CEngineResult<TDataType const *> getLayerValue(
                    std::string const &aLayerId,
                    std::string const &aBufferName,
                    std::string const &aFieldName)
            {
                bool const layerAvailable = hasLayer(aLayerId);
                if(not layerAvailable)
                {
                    return CEngineResult<TDataType const *>(EEngineStatus::Error, nullptr);
                }

                CMaterialLayer &layer = mLayers[aLayerId];

                return layer.getValue<TDataType>(aBufferName, aFieldName);
            }

            /**
             * setLayerValue
             *
             * @param aLayerId
             * @param aBufferName
             * @param aFieldName
             * @param aFieldValue
             * @return
             */
            template <typename TDataType>
            CEngineResult<> setLayerValue(
                    std::string const &aLayerId,
                    std::string const &aBufferName,
                    std::string const &aFieldName,
                    TDataType   const &aFieldValue)
            {
                bool const layerAvailable = hasLayer(aLayerId);
                if(not layerAvailable)
                {
                    return CEngineResult<>(EEngineStatus::Error);
                }

                CMaterialLayer &layer = mLayers[aLayerId];

                return layer.setValue<TDataType>(aBufferName, aFieldName, aFieldValue);
            }

        private_methods:
            SHIRABE_INLINE bool hasLayer(std::string const &aLayerId) const
            {
                bool const has = (mLayers.end() != mLayers.find(aLayerId));
                return has;
            }

        private_members:
            std::string                      mName;
            Map<std::string, CMaterialLayer> mLayers;
        };

        static void foo()
        {
            CMaterial material = {};

            CEngineResult<uint32_t const *> value = material.getLayerValue<uint32_t>("DefaultLayer", "Buffer", "Test");
        }

    }
}

#endif // MATERIAL_DECLARATION_H
