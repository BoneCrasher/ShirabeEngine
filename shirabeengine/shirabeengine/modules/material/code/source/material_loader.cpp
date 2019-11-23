#include <core/enginetypehelper.h>
#include <core/helpers.h>
#include <asset/assetstorage.h>
#include <resources/resourcedescriptions.h>
#include <resources/resourcetypes.h>

#include "material/material_declaration.h"
#include "material/materialserialization.h"
#include "material/material_loader.h"

namespace engine
{
    namespace material
    {
        using namespace asset;

        //<-----------------------------------------------------------------------------
        //;---------------------------------------------------------
        CMaterialLoader::CMaterialLoader(Shared<asset::IAssetStorage> const &aAssetStorage)
            : mStorage(aAssetStorage)
        {
            assert(nullptr != aAssetStorage);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        CEngineResult<T> readMaterialFile(  std::string          const &aLogTag
                                          , asset::IAssetStorage       *aAssetStorage
                                          , asset::AssetId_t     const &aAssetUID)
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
        CEngineResult<SMaterialMasterIndex> readMaterialMasterIndexFile(std::string const &aLogTag, asset::IAssetStorage *aAssetStorage, asset::AssetId_t const &aAssetUID)
        {
            return readMaterialFile<SMaterialMasterIndex>(aLogTag, aAssetStorage, aAssetUID);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<SMaterialInstanceIndex> readMaterialInstanceIndexFile(std::string const &aLogTag, asset::IAssetStorage *aAssetStorage, asset::AssetId_t const &aAssetUID)
        {
            return readMaterialFile<SMaterialInstanceIndex>(aLogTag, aAssetStorage, aAssetUID);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<SMaterialSignature> readMaterialSignature(std::string const &aLogTag, asset::IAssetStorage *aAssetStorage, asset::AssetId_t const &aAssetUID)
        {
            return readMaterialFile<SMaterialSignature>(aLogTag, aAssetStorage, aAssetUID);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<CMaterialConfig> readMaterialConfig(std::string const &aLogTag, asset::IAssetStorage *aAssetStorage, asset::AssetId_t const &aAssetUID)
        {
            return readMaterialFile<CMaterialConfig>(aLogTag, aAssetStorage, aAssetUID);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<SMaterialMeta> readMaterialMeta(std::string const &aLogTag, asset::IAssetStorage *aAssetStorage, asset::AssetId_t const &aAssetUID)
        {
            return readMaterialFile<SMaterialMeta>(aLogTag, aAssetStorage, aAssetUID);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        AssetID_t getAssetUIDForMaterialID(Map<std::string, AssetID_t> const &aRegistry, std::string const &aMaterialId)
        {
            if(aRegistry.end() == aRegistry.find(aMaterialId))
            {
                return 0_uid;
            }

            return aRegistry.at(aMaterialId);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        using ResourceDescriptionDerivationReturn_t = std::tuple<bool, resources::SMaterialPipelineDescriptor, resources::SShaderModuleDescriptor, Vector<resources::SBufferDescription>>;

        static
        ResourceDescriptionDerivationReturn_t deriveResourceDescriptions(Shared<asset::IAssetStorage>        &aAssetStorage
                                                                         , std::string                 const &aMaterialName
                                                                         , SMaterialSignature          const &aSignature
                                                                         , CMaterialConfig             const &aConfig)
        {
            using namespace resources;

            SMaterialPipelineDescriptor pipelineDescriptor      {};
            SShaderModuleDescriptor     shaderModuleDescriptors {};
            Vector<SBufferDescription>  bufferDescriptions      {};
            shaderModuleDescriptors.name = std::string(aMaterialName) + "ShaderModule";

            pipelineDescriptor.inputAssemblyState.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            pipelineDescriptor.inputAssemblyState.pNext                  = nullptr;
            pipelineDescriptor.inputAssemblyState.flags                  = 0;
            pipelineDescriptor.inputAssemblyState.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            pipelineDescriptor.inputAssemblyState.primitiveRestartEnable = false;

            pipelineDescriptor.rasterizerState.sType                     = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            pipelineDescriptor.rasterizerState.pNext                     = nullptr;
            pipelineDescriptor.rasterizerState.flags                     = 0;
            pipelineDescriptor.rasterizerState.cullMode                  = VkCullModeFlagBits::VK_CULL_MODE_NONE;
            pipelineDescriptor.rasterizerState.frontFace                 = VkFrontFace::VK_FRONT_FACE_COUNTER_CLOCKWISE;
            pipelineDescriptor.rasterizerState.polygonMode               = VkPolygonMode::VK_POLYGON_MODE_FILL;
            pipelineDescriptor.rasterizerState.lineWidth                 = 1.0f;
            pipelineDescriptor.rasterizerState.rasterizerDiscardEnable   = VK_FALSE;
            pipelineDescriptor.rasterizerState.depthClampEnable          = VK_FALSE;
            pipelineDescriptor.rasterizerState.depthBiasEnable           = VK_FALSE;
            pipelineDescriptor.rasterizerState.depthBiasSlopeFactor      = 0.0f;
            pipelineDescriptor.rasterizerState.depthBiasConstantFactor   = 0.0f;
            pipelineDescriptor.rasterizerState.depthBiasClamp            = 0.0f;

            pipelineDescriptor.multiSampler.sType                        = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            pipelineDescriptor.multiSampler.pNext                        = nullptr;
            pipelineDescriptor.multiSampler.flags                        = 0;
            pipelineDescriptor.multiSampler.sampleShadingEnable          = VK_FALSE;
            pipelineDescriptor.multiSampler.rasterizationSamples         = VK_SAMPLE_COUNT_1_BIT;
            pipelineDescriptor.multiSampler.minSampleShading             = 1.0f;
            pipelineDescriptor.multiSampler.pSampleMask                  = nullptr;
            pipelineDescriptor.multiSampler.alphaToCoverageEnable        = VK_FALSE;
            pipelineDescriptor.multiSampler.alphaToOneEnable             = VK_FALSE;

            pipelineDescriptor.depthStencilState.sType                   = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
            pipelineDescriptor.depthStencilState.pNext                   = nullptr;
            pipelineDescriptor.depthStencilState.flags                   = 0;
            pipelineDescriptor.depthStencilState.depthTestEnable         = VK_FALSE;
            pipelineDescriptor.depthStencilState.depthWriteEnable        = VK_FALSE;
            pipelineDescriptor.depthStencilState.depthCompareOp          = VkCompareOp::VK_COMPARE_OP_LESS;
            pipelineDescriptor.depthStencilState.stencilTestEnable       = VK_FALSE;
            pipelineDescriptor.depthStencilState.front.passOp            = VkStencilOp::VK_STENCIL_OP_KEEP;
            pipelineDescriptor.depthStencilState.front.failOp            = VkStencilOp::VK_STENCIL_OP_KEEP;
            pipelineDescriptor.depthStencilState.front.depthFailOp       = VkStencilOp::VK_STENCIL_OP_KEEP;
            pipelineDescriptor.depthStencilState.front.compareOp         = VkCompareOp::VK_COMPARE_OP_ALWAYS;
            pipelineDescriptor.depthStencilState.front.compareMask       = 0;
            pipelineDescriptor.depthStencilState.front.writeMask         = 0;
            pipelineDescriptor.depthStencilState.front.reference         = 0;
            pipelineDescriptor.depthStencilState.back                    = pipelineDescriptor.depthStencilState.front;
            pipelineDescriptor.depthStencilState.depthBoundsTestEnable   = VK_FALSE;
            pipelineDescriptor.depthStencilState.minDepthBounds          = 0.0f;
            pipelineDescriptor.depthStencilState.maxDepthBounds          = 1.0f;

            for(auto const &[stageKey, stage] : aSignature.stages)
            {
                if(VkPipelineStageFlagBits::VK_PIPELINE_STAGE_VERTEX_SHADER_BIT == stageKey)
                {
                    for(std::size_t k=0; k<stage.inputs.size(); ++k)
                    {
                        SStageInput const &input = stage.inputs.at(k);

                        // This number has to be equal to the VkVertexInputBindingDescription::binding index which data should be taken from!
                        VkVertexInputBindingDescription binding;
                        binding.binding   = k;
                        binding.stride    = input.type->arrayStride;
                        binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

                        VkVertexInputAttributeDescription attribute;
                        attribute.binding  = k;
                        attribute.location = k;
                        attribute.offset   = 0;
                        attribute.format   = (8 == binding.stride)
                                             ? VkFormat::VK_FORMAT_R32G32_SFLOAT
                                             : (12 == binding.stride)
                                               ? VkFormat::VK_FORMAT_R32G32B32_SFLOAT
                                               : (16 == binding.stride)
                                                 ? VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT
                                                 : VkFormat::VK_FORMAT_UNDEFINED;

                        pipelineDescriptor.vertexInputBindings  .push_back(binding);
                        pipelineDescriptor.vertexInputAttributes.push_back(attribute);
                    }
                }

                if(VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT == stageKey)
                {
                    std::vector<VkPipelineColorBlendAttachmentState> outputs {};
                    outputs.resize(stage.outputs.size());

                    for(auto const &output : stage.outputs)
                    {
                        VkPipelineColorBlendAttachmentState colorBlendAttachmentState{};
                        colorBlendAttachmentState.blendEnable         = VK_TRUE;
                        colorBlendAttachmentState.colorWriteMask      = VkColorComponentFlagBits::VK_COLOR_COMPONENT_R_BIT|
                                                                        VkColorComponentFlagBits::VK_COLOR_COMPONENT_G_BIT|
                                                                        VkColorComponentFlagBits::VK_COLOR_COMPONENT_B_BIT|
                                                                        VkColorComponentFlagBits::VK_COLOR_COMPONENT_A_BIT;
                        colorBlendAttachmentState.srcColorBlendFactor = VkBlendFactor::VK_BLEND_FACTOR_ONE;  // VkBlendFactor::VK_BLEND_FACTOR_SRC_ALPHA;
                        colorBlendAttachmentState.dstColorBlendFactor = VkBlendFactor::VK_BLEND_FACTOR_ZERO; // VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
                        colorBlendAttachmentState.colorBlendOp        = VkBlendOp::VK_BLEND_OP_ADD;
                        colorBlendAttachmentState.srcAlphaBlendFactor = VkBlendFactor::VK_BLEND_FACTOR_ONE;
                        colorBlendAttachmentState.dstAlphaBlendFactor = VkBlendFactor::VK_BLEND_FACTOR_ZERO;
                        colorBlendAttachmentState.alphaBlendOp        = VkBlendOp::VK_BLEND_OP_ADD;

                        outputs[output.location] = colorBlendAttachmentState;
                    }

                    pipelineDescriptor.colorBlendAttachmentStates = outputs;

                    VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo {};
                    colorBlendCreateInfo.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
                    colorBlendCreateInfo.pNext             = nullptr;
                    colorBlendCreateInfo.flags             = 0;
                    colorBlendCreateInfo.logicOpEnable     = VK_FALSE;
                    colorBlendCreateInfo.logicOp           = VK_LOGIC_OP_COPY;
                    colorBlendCreateInfo.blendConstants[0] = 0.0f;
                    colorBlendCreateInfo.blendConstants[1] = 1.0f;
                    colorBlendCreateInfo.blendConstants[2] = 2.0f;
                    colorBlendCreateInfo.blendConstants[3] = 3.0f;

                    pipelineDescriptor.colorBlendState = colorBlendCreateInfo;
                }

                //
                // Derive data accessors for shader module creation from master material
                //
                std::filesystem::path const  stageSpirVFilename = stage.filename;
                bool                  const  isEmptyFilename    = stageSpirVFilename.empty();
                if(not isEmptyFilename)
                {
                    DataSourceAccessor_t dataAccessor = [=] () -> ByteBuffer
                    {
                        asset::AssetID_t const assetUid = asset::assetIdFromUri(stageSpirVFilename);

                        auto const [result, buffer] = aAssetStorage->loadAssetData(assetUid);
                        if(CheckEngineError(result))
                        {
                            CLog::Error("DataSourceAccessor_t::ShaderModule", "Failed to load shader module asset data. Result: {}", result);
                            return {};
                        }

                        return buffer;
                    };

                    shaderModuleDescriptors.shaderStages[stageKey] = dataAccessor;
                }
            }

            std::vector<VkDescriptorSetLayoutCreateInfo> descriptorSets {};
            descriptorSets.resize(aSignature.layoutInfo.setCount);
            pipelineDescriptor.descriptorSetLayoutBindings.resize(aSignature.layoutInfo.setCount);

            for(std::size_t k=0; k<descriptorSets.size(); ++k)
            {
                VkDescriptorSetLayoutCreateInfo &info = descriptorSets[k];

                info.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
                info.pNext        = nullptr;
                info.flags        = 0;
                info.bindingCount = aSignature.layoutInfo.setBindingCount[k];

                pipelineDescriptor.descriptorSetLayoutBindings[k].resize(info.bindingCount);
            }

            pipelineDescriptor.descriptorSetLayoutCreateInfos = descriptorSets;

            for(SSubpassInput const &input : aSignature.subpassInputs)
            {
                VkDescriptorSetLayoutBinding layoutBinding {};
                layoutBinding.binding            = input.binding;
                layoutBinding.descriptorType     = VkDescriptorType::VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
                layoutBinding.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT; // Subpass inputs are only accessibly in fragment shaders.
                layoutBinding.descriptorCount    = 1;
                layoutBinding.pImmutableSamplers = nullptr;
                pipelineDescriptor.descriptorSetLayoutBindings[input.set][input.binding] = layoutBinding;
            }

            for(SUniformBuffer const &uniformBuffer : aSignature.uniformBuffers)
            {
                VkDescriptorSetLayoutBinding layoutBinding {};
                layoutBinding.binding            = uniformBuffer.binding;
                layoutBinding.descriptorType     = VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                layoutBinding.stageFlags         = serialization::shaderStageFromPipelineStage(uniformBuffer.stageBinding.value());
                layoutBinding.descriptorCount    = 1;
                layoutBinding.pImmutableSamplers = nullptr;
                pipelineDescriptor.descriptorSetLayoutBindings[uniformBuffer.set][uniformBuffer.binding] = layoutBinding;

                // TODO: Arrays?
            }

            for(SSampledImage const &sampledImage : aSignature.sampledImages)
            {
                VkDescriptorSetLayoutBinding layoutBinding {};
                layoutBinding.binding            = sampledImage.binding;
                layoutBinding.descriptorType     = VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                layoutBinding.stageFlags         = serialization::shaderStageFromPipelineStage(sampledImage.stageBinding.value());
                layoutBinding.descriptorCount    = 1;
                layoutBinding.pImmutableSamplers = nullptr;
                pipelineDescriptor.descriptorSetLayoutBindings[sampledImage.set][sampledImage.binding] = layoutBinding;
            }

            VkViewport viewPort = {};
            viewPort.x        = 0.0;
            viewPort.y        = 0.0;
            viewPort.width    = 1920.0;
            viewPort.height   = 1080.0;
            viewPort.minDepth = 0.0;
            viewPort.maxDepth = 1.0;
            pipelineDescriptor.viewPort = viewPort;

            return { true, pipelineDescriptor, shaderModuleDescriptors, bufferDescriptions };
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        using MasterMaterialReturn_t = std::tuple<bool, std::string, SMaterialMeta, SMaterialSignature, CMaterialConfig>;

        static
        auto loadMasterMaterialFiles(std::string                                    const &aLogTag,
                                Shared<asset::IAssetStorage>                         &aAssetStorage,
                                Map<asset::AssetID_t, Shared<CMaterialMaster>>       &aMasterMaterialIndex,
                                asset::AssetID_t                               const &aMasterMaterialAssetId)
            -> MasterMaterialReturn_t
        {
            AssetID_t              const masterIndexId    = aMasterMaterialAssetId; // Needs to be here, since it will be shared across both case-blocks.
            CResult<SMaterialMeta>       masterIndexFetch = {};

            MasterMaterialReturn_t const failureReturnValue = { false, {}, {}, {}, {} };

            auto const [metaDataFetchResult, metaData] = readMaterialMeta(aLogTag, aAssetStorage.get(), masterIndexId);
            {
                PrintEngineError(metaDataFetchResult, aLogTag, "Could not fetch master meta data.");
                SHIRABE_RETURN_VALUE_ON_ERROR(metaDataFetchResult, failureReturnValue);
            }

            auto const [signatureAssetFetchResult, signatureAsset] = aAssetStorage->loadAsset(metaData.signatureAssetUid);
            {
                PrintEngineError(signatureAssetFetchResult, aLogTag, "Could not fetch signature asset data.");
                SHIRABE_RETURN_VALUE_ON_ERROR(signatureAssetFetchResult, failureReturnValue);
            }

            auto const [signatureFetchResult, signature] = readMaterialSignature(aLogTag, aAssetStorage.get(), signatureAsset.id);
            {
                PrintEngineError(signatureFetchResult, aLogTag, "Could not fetch signature data.");
                SHIRABE_RETURN_VALUE_ON_ERROR(signatureFetchResult, failureReturnValue);
            }

            auto const [configAssetFetchResult, configAsset] = aAssetStorage->loadAsset(metaData.configurationAssetUid);
            {
                PrintEngineError(configAssetFetchResult, aLogTag, "Could not fetch configuration asset data.");
                SHIRABE_RETURN_VALUE_ON_ERROR(configAssetFetchResult, failureReturnValue);
            }

            auto const [configFetchResult, config] = readMaterialConfig(aLogTag, aAssetStorage.get(), configAsset.id);
            {
                PrintEngineError(configFetchResult, aLogTag, "Could not fetch configuration data.");
                SHIRABE_RETURN_VALUE_ON_ERROR(configFetchResult, failureReturnValue);
            }

            std::string        const  masterName      = metaData.name;
            SMaterialSignature const &masterSignature = signature;
            CMaterialConfig    const &masterConfig    = config;

            return { true, masterName, metaData, masterSignature, masterConfig };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult<Shared<CMaterialInstance>> CMaterialLoader::createMaterialInstance(asset::AssetID_t const &aMasterMaterialAssetId)
        {
            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult<> CMaterialLoader::destroyMaterialInstance(asset::AssetID_t const &aMaterialInstanceAssetId)
        {
            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<SMaterialMeta> CMaterialLoader::loadMaterialMeta(engine::asset::AssetID_t const &aMaterialInstanceAssetId)
        {
            return readMaterialMeta(logTag(), mStorage.get(), aMaterialInstanceAssetId);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<Shared<CMaterialMaster>> CMaterialLoader::loadMaterialInstance(asset::AssetID_t const &aMaterialInstanceAssetId)
        {
            CEngineResult<ByteBuffer> data = {};

            //--------------------------------------------------------------------------------------------------------------------
            // Fetch instance data
            //--------------------------------------------------------------------------------------------------------------------            
            AssetID_t instanceIndexAssetId = aMaterialInstanceAssetId;
            if(0_uid == instanceIndexAssetId)
            {
                return { EEngineStatus::Error };
            }

            // auto const [instanceIndexAssetFetchResult, instanceIndexAsset] = mStorage->loadAsset(instanceIndexAssetId);
            // {
            //     PrintEngineError(instanceIndexAssetFetchResult, logTag(), "Couldn't fetch material instance index asset.");
            //     SHIRABE_RETURN_RESULT_ON_ERROR(instanceIndexAssetFetchResult);
            // }

            // auto const [instanceIndexFetchResult, instanceIndex] = readMaterialInstanceIndexFile(logTag(), mStorage.get(), instanceIndexAssetId);
            // {
            //     PrintEngineError(instanceIndexFetchResult, logTag(), "Couldn't fetch material instance index for ID {}", instanceIndexAssetId);
            //     SHIRABE_RETURN_RESULT_ON_ERROR(instanceIndexFetchResult);
            // }
//
            // auto const [instanceConfigAssetFetchResult, instanceConfigAsset] = mStorage->loadAsset(instanceIndex.configurationAssetId);
            // {
            //     PrintEngineError(instanceConfigAssetFetchResult, logTag(), "Couldn't fetch instance config asset for ID {}.", instanceIndex.configurationAssetId);
            //     SHIRABE_RETURN_RESULT_ON_ERROR(instanceConfigAssetFetchResult);
            // }
//
            // auto [instanceConfigFetchResult, instanceConfig] = readMaterialConfig(logTag(), mStorage.get(), instanceConfigAsset.id);
            // {
            //     PrintEngineError(instanceConfigFetchResult, logTag(), "Couldn't fetch instance config for ID {}.", instanceConfigAsset.id);
            //     SHIRABE_RETURN_RESULT_ON_ERROR(instanceConfigFetchResult);
            // }
//
            // std::string const  instanceName = instanceIndex.name;

            //--------------------------------------------------------------------------------------------------------------------
            // Fetch master data
            //--------------------------------------------------------------------------------------------------------------------
            AssetID_t masterIndexId = aMaterialInstanceAssetId; // instanceIndexAsset.parent;
            // if(0_uid == masterIndexId)
            // {
            //     return { EEngineStatus::Error };
            // }

            //
            // If the material has been loaded already, return it!
            // TODO: Material reload on filesystem change?
            //
            if(mInstantiatedMaterialMasters.end() != mInstantiatedMaterialMasters.find(masterIndexId))
            {
                return { EEngineStatus::Ok, mInstantiatedMaterialMasters.at(masterIndexId) };
            }

            auto [successful, masterName, masterMeta, masterSignature, masterConfig] = loadMasterMaterialFiles(logTag(), mStorage, mInstantiatedMaterialMasters, masterIndexId);
            {
                PrintEngineError(not successful, logTag(), "Couldn't fetch master material data.");
                SHIRABE_RETURN_RESULT_ON_ERROR(not successful);
            }

            //--------------------------------------------------------------------------------------------------------------------
            // Override instance with master config.
            //--------------------------------------------------------------------------------------------------------------------
            // instanceConfig.override(masterConfig);

            //--------------------------------------------------------------------------------------------------------------------
            // Create Material instance
            //--------------------------------------------------------------------------------------------------------------------
            Shared<CMaterialMaster> master = makeShared<CMaterialMaster>  (masterIndexId, masterName, std::move(masterSignature), std::move(masterConfig));
            // Shared<CMaterialInstance> instance = makeShared<CMaterialInstance>(instanceIndexAssetId, instanceName, std::move(instanceConfig),  master);

            auto const [derivationSuccessful, pipelineDescription, shaderModuleDescription, bufferDescriptions] = deriveResourceDescriptions(mStorage, masterName, masterSignature, masterConfig);
            master->setPipelineDescription    (pipelineDescription);
            master->setShaderModuleDescription(shaderModuleDescription);
            master->setBufferDescriptions     (bufferDescriptions);

            mInstantiatedMaterialMasters  [master->getAssetId()] = master;
            // mInstantiatedMaterialInstances[instance->getAssetId()] = instance;

            return { EEngineStatus::Ok, master };
        }
        //<-----------------------------------------------------------------------------

    }
}
