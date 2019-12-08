#include <core/enginetypehelper.h>
#include <core/helpers.h>
#include <asset/assetstorage.h>
#include <resources/resourcedescriptions.h>
#include <resources/resourcetypes.h>
#include <util/crc32.h>
#include <util/documents/json.h>

#include "mesh/declaration.h"
#include "mesh/serialization.h"
#include "mesh/loader.h"

namespace engine
{
    namespace mesh
    {
        using namespace asset;
        using namespace engine::documents;

        //<-----------------------------------------------------------------------------
        CMeshLoader::CMeshLoader()
        {
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        CEngineResult<T> readMeshFile(  std::string                  const &aLogTag
                                      , Shared<asset::IAssetStorage> const &aAssetStorage
                                      , asset::AssetId_t             const &aAssetUID)
        {
            using namespace serialization;

            CJSONDeserializer<T> deserializer {};
            deserializer.initialize();

            auto const [dataFetchResult, dataBuffer] = aAssetStorage->loadAssetData(aAssetUID);
            {
                PrintEngineError(dataFetchResult, aLogTag, "Could not load asset data for asset {}", aAssetUID);
                SHIRABE_RETURN_RESULT_ON_ERROR(dataFetchResult);
            }

            std::string rawInput(reinterpret_cast<char const *>(dataBuffer.data()),
                                                                dataBuffer.size());

            auto const [deserializationSuccessful, resultData] = deserializer.deserialize(rawInput);
            {
                PrintEngineError(not deserializationSuccessful, aLogTag, "Could not load material file '{}'", aAssetUID);
                SHIRABE_RETURN_RESULT_ON_ERROR(not deserializationSuccessful);
            }

            CEngineResult<T> const index = { EEngineStatus::Ok, resultData->asT().data() };

            deserializer.deinitialize();

            return index;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<SMeshMeta> readMeshMeta(std::string const &aLogTag, Shared<asset::IAssetStorage> const &aAssetStorage, asset::AssetId_t const &aAssetUID)
        {
            return readMeshFile<SMeshMeta>(aLogTag, aAssetStorage, aAssetUID);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<SMeshDataFile> readMeshData(std::string const &aLogTag, Shared<asset::IAssetStorage> const &aAssetStorage, asset::AssetId_t const &aAssetUID)
        {
            return readMeshFile<SMeshDataFile>(aLogTag, aAssetStorage, aAssetUID);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        AssetID_t getAssetUIDForMeshID(Map<std::string, AssetID_t> const &aRegistry, std::string const &aMaterialId)
        {
            if(aRegistry.end() == aRegistry.find(aMaterialId))
            {
                return 0_uid;
            }

            return aRegistry.at(aMaterialId);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        using MasterMaterialReturn_t = std::tuple<bool, std::string, SMeshMeta, SMeshDataFile>;

        static
        auto loadMeshFiles(std::string                                    const &aLogTag,
                           Shared<asset::IAssetStorage>                   const &aAssetStorage,
                           Map<asset::AssetID_t, Shared<CMeshInstance>>         &aMeshIndex,
                           asset::AssetID_t                               const &aMeshAssetId)
            -> MasterMaterialReturn_t
        {
            AssetID_t const meshId = aMeshAssetId; // Needs to be here, since it will be shared across both case-blocks.

            MasterMaterialReturn_t const failureReturnValue = { false, {}, {}, {} };

            auto const [metaDataFetchResult, metaData] = readMeshMeta(aLogTag, aAssetStorage, meshId);
            {
                PrintEngineError(metaDataFetchResult, aLogTag, "Could not fetch master meta data.");
                SHIRABE_RETURN_VALUE_ON_ERROR(metaDataFetchResult, failureReturnValue);
            }

            auto const [meshAssetFetchResult, meshAsset] = aAssetStorage->loadAsset(metaData.dataFileId);
            {
                PrintEngineError(meshAssetFetchResult, aLogTag, "Could not fetch signature asset data.");
                SHIRABE_RETURN_VALUE_ON_ERROR(meshAssetFetchResult, failureReturnValue);
            }

            auto const [meshDataFetchResult, meshData] = readMeshData(aLogTag, aAssetStorage, meshAsset.id);
            {
                PrintEngineError(meshDataFetchResult, aLogTag, "Could not fetch master meta data.");
                SHIRABE_RETURN_VALUE_ON_ERROR(meshDataFetchResult, failureReturnValue);
            }

            std::string const  meshName = metaData.name;

            return { true, meshName, metaData, meshData };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult<Shared<CMeshInstance>> CMeshLoader::createMeshInstance(asset::AssetID_t const &aMeshAssetId)
        {
            SHIRABE_UNUSED(aMeshAssetId);
            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult<> CMeshLoader::destroyMeshInstance(engine::asset::AssetID_t const &aMeshAssetId)
        {
            SHIRABE_UNUSED(aMeshAssetId);
            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<SMeshMeta> CMeshLoader::loadMeshMeta(engine::Shared<engine::asset::IAssetStorage> const &aAssetStorage
                                                         , engine::asset::AssetID_t                     const &aMeshAssetId)
        {
            return readMeshMeta(logTag(), aAssetStorage, aMeshAssetId);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<Shared<CMeshInstance>> CMeshLoader::loadMeshInstance( std::string                  const &aMeshInstanceId
                                                                          , Shared<asset::IAssetStorage> const &aAssetStorage
                                                                          , asset::AssetID_t             const &aMeshInstanceAssetId)
        {
            if(mInstantiatedMeshes.end() != mInstantiatedMeshes.find(aMeshInstanceAssetId))
            {
                return { EEngineStatus::Ok, mInstantiatedMeshes.at(aMeshInstanceAssetId) };
            }

            return loadMeshInstance(aAssetStorage, aMeshInstanceAssetId);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult<Shared<CMeshInstance>> CMeshLoader::loadMeshInstance( Shared<asset::IAssetStorage> const &aAssetStorage
                                                                          , asset::AssetID_t             const &aMeshInstanceAssetId)
        {
            CEngineResult<ByteBuffer> data = {};

            //--------------------------------------------------------------------------------------------------------------------
            // Fetch instance data
            //--------------------------------------------------------------------------------------------------------------------
            if(0_uid == aMeshInstanceAssetId)
            {
                return { EEngineStatus::Error };
            }

            //
            // If the material has been loaded already, return it!
            //
            Shared<CMeshInstance> instance = nullptr;

            if(mInstantiatedMeshes.end() != mInstantiatedMeshes.find(aMeshInstanceAssetId))
            {
                instance = mInstantiatedMeshes.at(aMeshInstanceAssetId);
            }
            else
            {
                auto[successful, meshName, meshMeta, meshDataFile] = loadMeshFiles(logTag(), aAssetStorage, mInstantiatedMeshes, aMeshInstanceAssetId);
                {
                    PrintEngineError(not successful, logTag(), "Couldn't fetch master material data.");
                    SHIRABE_RETURN_RESULT_ON_ERROR(not successful);
                }

                instance = makeShared<CMeshInstance>(aMeshInstanceAssetId, meshName, std::move(meshDataFile));
                mInstantiatedMeshes[instance->getAssetId()] = instance;
            }

            if(nullptr == instance)
            {
                return { EEngineStatus::Error, nullptr };
            }

            return { EEngineStatus::Ok, instance };
        }
        //<-----------------------------------------------------------------------------

    }
}
