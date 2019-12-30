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
#include "mesh/serialization.h"

namespace engine::mesh
{
    using namespace resources;

    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    using ResourceDescriptionDerivationReturn_t = std::tuple<bool, resources::SBufferDescription, resources::SBufferDescription>;

    static
    ResourceDescriptionDerivationReturn_t deriveResourceDescriptions(Shared<asset::IAssetStorage>  const &aAssetStorage
                                                                     , std::string                 const &aMeshName
                                                                     , SMeshDataFile               const &aDataFile)
    {
        using namespace resources;

        uint64_t accumulatedVertexDataSize = 0;
        for(auto const &attributeDescription : aDataFile.attributes)
        {
            accumulatedVertexDataSize += (attributeDescription.length * attributeDescription.bytesPerSample);
        }

        DataSourceAccessor_t dataAccessor = [=] () -> ByteBuffer
        {
            asset::AssetID_t const assetUid = asset::assetIdFromUri(aDataFile.dataBinaryFilename);
            auto const [result, buffer] = aAssetStorage->loadAssetData(assetUid);
            if(CheckEngineError(result))
            {
                CLog::Error("DataSourceAccessor_t::MeshBinaryData", "Failed to load binary data for mesh. Result: {}", result);
                return {};
            }

            ByteBuffer bufferView = buffer.createView(0, accumulatedVertexDataSize);
            return buffer;
        };

        SBufferDescription dataBufferDescription {};
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

        DataSourceAccessor_t indexDataAccessor = [=] () -> ByteBuffer
        {
            asset::AssetID_t const assetUid = asset::assetIdFromUri(aDataFile.indexBinaryFilename);
            auto const [result, buffer] = aAssetStorage->loadAssetData(assetUid);
            if(CheckEngineError(result))
            {
                CLog::Error("DataSourceAccessor_t::MeshBinaryData", "Failed to load binary data for mesh. Result: {}", result);
                return {};
            }

            ByteBuffer bufferView = buffer.createView(aDataFile.indices.offset, aDataFile.indices.length);
            return buffer;
        };

        SBufferDescription indexBufferDescription   {};
        indexBufferDescription.name                             = fmt::format("{}_{}", aMeshName, "indexbuffer");
        indexBufferDescription.createInfo.sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        indexBufferDescription.createInfo.pNext                 = nullptr;
        indexBufferDescription.createInfo.flags                 = 0;
        indexBufferDescription.dataSource                       = indexDataAccessor;
        indexBufferDescription.createInfo.size                  = (aDataFile.indices.length * aDataFile.indices.bytesPerSample);
        indexBufferDescription.createInfo.pQueueFamilyIndices   = nullptr;
        indexBufferDescription.createInfo.queueFamilyIndexCount = 0;
        indexBufferDescription.createInfo.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
        indexBufferDescription.createInfo.usage                 = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

        return { true, dataBufferDescription, indexBufferDescription };
    };
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    static Shared<CResourceFromAssetResourceObjectCreator<SMesh>> getAssetLoader(Shared<resources::CResourceManager> const &aResourceManager
                                                                                   , Shared<asset::IAssetStorage>    const &aAssetStorage
                                                                                   , Shared<mesh::CMeshLoader>       const &aMeshLoader)
    {
        static constexpr char const *SHIRABE_MATERIALSYSTEM_CORE_MATERIAL_RESOURCEID = "Core";
        auto const loader = [=] (ResourceId_t const &aResourceId, AssetId_t const &aAssetId) -> Shared<ILogicalResourceObject>
        {
            auto const &[result, instance] = aMeshLoader->loadMeshInstance(aResourceId, aAssetStorage, aAssetId);
            if(CheckEngineError(result))
            {
                return nullptr;
            }

            SMeshDataFile const &dataFile = instance->dataFile();

            auto [derivationSuccessful, vertexBufferDescription, indexBufferDescription] = deriveResourceDescriptions(aAssetStorage, instance->name(), instance->dataFile());

            SMeshDescriptor meshDescriptor {};
            meshDescriptor.name                   = aResourceId;
            meshDescriptor.dataBufferDescription  = vertexBufferDescription;
            meshDescriptor.indexBufferDescription = indexBufferDescription;

            meshDescriptor.attributeCount         = dataFile.attributeSampleCount;
            meshDescriptor.indexSampleCount       = dataFile.indexSampleCount;

            Vector<VkDeviceSize> offsets;
            offsets.resize(4);

            VkDeviceSize currentOffset = 0;
            for(uint64_t k=0; k<4; ++k)
            {
                offsets[k] = currentOffset;
                VkDeviceSize length = (dataFile.attributes[k].length * dataFile.attributes[k].bytesPerSample);
                currentOffset += length;
            }
            meshDescriptor.offsets = offsets;

            CEngineResult<Shared<ILogicalResourceObject>> bufferResourceObject = aResourceManager->useDynamicResource<SBuffer>(meshDescriptor.dataBufferDescription.name, meshDescriptor.dataBufferDescription);
            EngineStatusPrintOnError(bufferResourceObject.result(),  "Mesh::AssetLoader", "Failed to create vertex buffer.");
            Shared<SBuffer> vertexBuffer = std::static_pointer_cast<SBuffer>(bufferResourceObject.data());

            CEngineResult<Shared<ILogicalResourceObject>> indexBufferResourceObject = aResourceManager->useDynamicResource<SBuffer>(meshDescriptor.indexBufferDescription.name, meshDescriptor.indexBufferDescription);
            EngineStatusPrintOnError(indexBufferResourceObject.result(),  "Mesh::AssetLoader", "Failed to create index buffer.");
            Shared<SBuffer> indexBuffer = std::static_pointer_cast<SBuffer>(indexBufferResourceObject.data());

            std::vector<std::string> meshDependencies {};
            meshDependencies.push_back(meshDescriptor.dataBufferDescription.name);
            meshDependencies.push_back(meshDescriptor.indexBufferDescription.name);

            CEngineResult<Shared<ILogicalResourceObject>> meshObject = aResourceManager->useDynamicResource<SMesh>(meshDescriptor.name, meshDescriptor);
            Shared<SMesh> mesh = std::static_pointer_cast<SMesh>(meshObject.data());
            mesh->vertexDataBufferResource = vertexBuffer;
            mesh->indexBufferResource      = indexBuffer;

            return mesh;
        };

        return makeShared<CResourceFromAssetResourceObjectCreator<SMesh>>(loader);
    }
}

#endif //__SHIRABEDEVELOPMENT_ASSETLOADER_H__
