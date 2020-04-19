//
// Created by dotti on 24.11.19.
//

#ifndef __SHIRABEDEVELOPMENT_MESH_ASSETLOADER_H__
#define __SHIRABEDEVELOPMENT_MESH_ASSETLOADER_H__

#include <asset/assetstorage.h>
#include <resources/resourcedescriptions.h>
#include <resources/resourcetypes.h>
#include <resources/cresourcemanager.h>
#include "mesh/loader.h"
#include "mesh/declaration.h"

namespace engine
{
    namespace mesh
    {

        using namespace resources;

        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        using ResourceDescriptionDerivationReturn_t = std::tuple<bool, resources::SBufferDescription, resources::SBufferDescription>;

        static
        ResourceDescriptionDerivationReturn_t deriveResourceDescriptions(Shared<asset::IAssetStorage> const &aAssetStorage
                                                                         , std::string                const &aMeshName
                                                                         , Shared<CMeshInstance>      const &aInstance)
        {
            using namespace resources;

            uint64_t        accumulatedVertexDataSize = 0;
            for( auto const &attributeDescription : aInstance->attributes() )
            {
                accumulatedVertexDataSize += (attributeDescription.length * attributeDescription.bytesPerSample);
            }

            DataSourceAccessor_t dataAccessor = [=]() -> ByteBuffer
            {
                asset::AssetID_t const assetUid = asset::assetIdFromUri(aInstance->binaryFilename());
                auto const[result, buffer] = aAssetStorage->loadAssetData(assetUid);
                if( CheckEngineError(result))
                {
                    CLog::Error("DataSourceAccessor_t::MeshBinaryData", "Failed to load binary data for mesh. Result: {}", result);
                    return {};
                }

                ByteBuffer bufferView = buffer.createView(0, accumulatedVertexDataSize);
                return buffer;
            };

            SBufferDescription dataBufferDescription{};
            dataBufferDescription.name                             = fmt::format("{}_{}_", aMeshName, "vertexbuffer");
            dataBufferDescription.dataSource                       = dataAccessor;
            dataBufferDescription.createInfo.sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            dataBufferDescription.createInfo.pNext                 = nullptr;
            dataBufferDescription.createInfo.flags                 = 0;
            dataBufferDescription.createInfo.size                  = accumulatedVertexDataSize;
            dataBufferDescription.createInfo.pQueueFamilyIndices   = nullptr;
            dataBufferDescription.createInfo.queueFamilyIndexCount = 0;
            dataBufferDescription.createInfo.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
            dataBufferDescription.createInfo.usage                 = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

            DataSourceAccessor_t indexDataAccessor = [=]() -> ByteBuffer
            {
                asset::AssetID_t const assetUid = asset::assetIdFromUri(aInstance->binaryFilename());
                auto const[result, buffer] = aAssetStorage->loadAssetData(assetUid);
                if( CheckEngineError(result))
                {
                    CLog::Error("DataSourceAccessor_t::MeshBinaryData", "Failed to load binary data for mesh. Result: {}", result);
                    return {};
                }

                ByteBuffer bufferView = buffer.createView(aInstance->indices().offset
                                                        , aInstance->indices().length);
                return buffer;
            };

            SBufferDescription indexBufferDescription{};
            indexBufferDescription.name                             = fmt::format("{}_{}", aMeshName, "indexbuffer");
            indexBufferDescription.createInfo.sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            indexBufferDescription.createInfo.pNext                 = nullptr;
            indexBufferDescription.createInfo.flags                 = 0;
            indexBufferDescription.dataSource                       = indexDataAccessor;
            indexBufferDescription.createInfo.size                  = (aInstance->indices().length * aInstance->indices().bytesPerSample);
            indexBufferDescription.createInfo.pQueueFamilyIndices   = nullptr;
            indexBufferDescription.createInfo.queueFamilyIndexCount = 0;
            indexBufferDescription.createInfo.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
            indexBufferDescription.createInfo.usage                 = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

            return {true, dataBufferDescription, indexBufferDescription};
        };
    }

    namespace resources
    {
        using namespace asset;
        using namespace mesh;

        template<>
        class CResourceDescDeriver<CMeshInstance, SMeshDescriptor>
        {
        public:
            static SMeshDescriptor derive(Shared<CAssetStorage> aAssetStorage, Shared<CMeshInstance> aInstance)
            {
                static constexpr char const *SHIRABE_MATERIALSYSTEM_CORE_MATERIAL_RESOURCEID = "Core";

                auto [derivationSuccessful, vertexBufferDescription, indexBufferDescription] = deriveResourceDescriptions(aAssetStorage, aInstance->name(), aInstance);

                SMeshDescriptor meshDescriptor {};
                meshDescriptor.name                   = aInstance->name();
                meshDescriptor.dataBufferDescription  = vertexBufferDescription;
                meshDescriptor.indexBufferDescription = indexBufferDescription;

                meshDescriptor.attributeCount         = aInstance->attributeSampleCount();
                meshDescriptor.indexSampleCount       = aInstance->indexSampleCount();

                Vector<VkDeviceSize> offsets;
                offsets.resize(4);

                VkDeviceSize currentOffset = 0;
                for(uint64_t k=0; k<4; ++k)
                {
                    offsets[k] = currentOffset;
                    VkDeviceSize length = (aInstance->attributes()[k].length * aInstance->attributes()[k].bytesPerSample);
                    currentOffset += length;
                }
                meshDescriptor.offsets = offsets;

                return meshDescriptor;
            }
        };
    }
}

#endif //__SHIRABEDEVELOPMENT_ASSETLOADER_H__
