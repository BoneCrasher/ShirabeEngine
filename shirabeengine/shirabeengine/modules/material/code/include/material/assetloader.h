//
// Created by dotti on 24.11.19.
//

#ifndef __SHIRABEDEVELOPMENT_MATERIAL_ASSETLOADER_H__
#define __SHIRABEDEVELOPMENT_MATERIAL_ASSETLOADER_H__

#include <asset/assetstorage.h>
#include <resources/resourcedescriptions.h>
#include <resources/resourcetypes.h>
#include <resources/cresourcemanager.h>
#include "material/declaration.h"
#include "material/serialization.h"

namespace engine::material
{
    using namespace resources;

    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    using ResourceDescriptionDerivationReturn_t = std::tuple<bool, resources::SMaterialPipelineDescriptor, resources::SShaderModuleDescriptor, Vector<resources::SBufferDescription>>;

    static
    ResourceDescriptionDerivationReturn_t deriveResourceDescriptions(Shared<asset::IAssetStorage>  const &aAssetStorage
                                                                     , std::string                 const &aMaterialName
                                                                     , SMaterialSignature          const &aSignature
                                                                     , CMaterialConfig             const &aConfiguration
                                                                     , bool                               aIncludeSystemBuffers = false)
    {
        using namespace resources;

        bool const isCoreMaterial = ("Core" == aMaterialName);

        SMaterialPipelineDescriptor     pipelineDescriptor     {};
        SShaderModuleDescriptor         shaderModuleDescriptor {};
        Vector<SBufferDescription>      bufferDescriptions     {};

        pipelineDescriptor    .name = fmt::format("{}_{}", aMaterialName, "pipeline");
        shaderModuleDescriptor.name = fmt::format("{}_{}", aMaterialName, "shadermodule");

        pipelineDescriptor.includesSystemBuffers                     = aIncludeSystemBuffers;

        pipelineDescriptor.inputAssemblyState.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        pipelineDescriptor.inputAssemblyState.pNext                  = nullptr;
        pipelineDescriptor.inputAssemblyState.flags                  = 0;
        pipelineDescriptor.inputAssemblyState.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        pipelineDescriptor.inputAssemblyState.primitiveRestartEnable = false;

        pipelineDescriptor.rasterizerState.sType                     = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        pipelineDescriptor.rasterizerState.pNext                     = nullptr;
        pipelineDescriptor.rasterizerState.flags                     = 0;
        pipelineDescriptor.rasterizerState.cullMode                  = VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT;
        pipelineDescriptor.rasterizerState.frontFace                 = VkFrontFace::VK_FRONT_FACE_COUNTER_CLOCKWISE;
        pipelineDescriptor.rasterizerState.polygonMode               = VkPolygonMode::VK_POLYGON_MODE_FILL;
        pipelineDescriptor.rasterizerState.lineWidth                 = 1.0f;
        pipelineDescriptor.rasterizerState.rasterizerDiscardEnable   = VK_FALSE; // isCoreMaterial ? VK_TRUE : VK_FALSE;
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
        pipelineDescriptor.depthStencilState.depthTestEnable         = VK_TRUE;
        pipelineDescriptor.depthStencilState.depthWriteEnable        = VK_TRUE;
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
                std::vector<SStageInput> stageInputs(stage.inputs);
                std::sort(stageInputs.begin(), stageInputs.end(), [] (SStageInput const &aLHS, SStageInput const &aRHS) -> bool { return aLHS.location < aRHS.location; });

                for(std::size_t k=0; k<stage.inputs.size(); ++k)
                {
                    SStageInput const &input = stageInputs.at(k);

                    // This number has to be equal to the VkVertexInputBindingDescription::binding index which data should be taken from!
                    VkVertexInputBindingDescription binding;
                    binding.binding   = input.location;
                    binding.stride    = (input.type->byteSize * input.type->vectorSize);
                    binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

                    VkVertexInputAttributeDescription attribute;
                    attribute.binding  = k;
                    attribute.location = input.location;
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

                shaderModuleDescriptor.shaderStages[stageKey] = dataAccessor;
            }
        }

        uint32_t const setSubtractionValue = aIncludeSystemBuffers ? 0 : 2;
        uint32_t const setCount            = aSignature.layoutInfo.setCount - setSubtractionValue;

        std::vector<VkDescriptorSetLayoutCreateInfo> descriptorSets {};
        descriptorSets.resize(setCount);
        pipelineDescriptor.descriptorSetLayoutBindings.resize(setCount);

        for(std::size_t k=0; k<descriptorSets.size(); ++k)
        {
            VkDescriptorSetLayoutCreateInfo &info = descriptorSets[k];

            info.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            info.pNext        = nullptr;
            info.flags        = 0;
            info.bindingCount = aSignature.layoutInfo.setBindingCount[k + setSubtractionValue];

            pipelineDescriptor.descriptorSetLayoutBindings[k].resize(info.bindingCount);
        }

        pipelineDescriptor.descriptorSetLayoutCreateInfos = descriptorSets;

        for(SSubpassInput const &input : aSignature.subpassInputs)
        {
            if(not aIncludeSystemBuffers && 2 > input.set)
            {
                continue;
            }

            VkDescriptorSetLayoutBinding layoutBinding {};
            layoutBinding.binding            = input.binding;
            layoutBinding.descriptorType     = VkDescriptorType::VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
            layoutBinding.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT; // Subpass inputs are only accessibly in fragment shaders.
            layoutBinding.descriptorCount    = 1;
            layoutBinding.pImmutableSamplers = nullptr;
            pipelineDescriptor.descriptorSetLayoutBindings[input.set - setSubtractionValue][input.binding] = layoutBinding;
        }

        for(SUniformBuffer const &uniformBuffer : aSignature.uniformBuffers)
        {
            if(not aIncludeSystemBuffers && 2 > uniformBuffer.set)
            {
                continue;
            }

            VkDescriptorSetLayoutBinding layoutBinding {};
            layoutBinding.binding            = uniformBuffer.binding;
            layoutBinding.descriptorType     = VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            layoutBinding.stageFlags         = VkShaderStageFlagBits::VK_SHADER_STAGE_ALL; // serialization::shaderStageFromPipelineStage(uniformBuffer.stageBinding.value());
            layoutBinding.descriptorCount    = uniformBuffer.array.layers;
            layoutBinding.pImmutableSamplers = nullptr;
            pipelineDescriptor.descriptorSetLayoutBindings[uniformBuffer.set - setSubtractionValue][uniformBuffer.binding] = layoutBinding;

            CEngineResult<void const *const> bufferDataFetch = aConfiguration.getBuffer(uniformBuffer.name);
            if(CheckEngineError(bufferDataFetch.result()))
            {
                CLog::Debug("AssetLoader - Materials", "Can't find buffer w/ name {} in config.", uniformBuffer.name);
                continue;
            }

            auto  const *const data = static_cast<uint8_t const *const>(bufferDataFetch.data());
            std::size_t  const size = uniformBuffer.location.length;

            auto const dataSource =  [data, size] () -> ByteBuffer
            {
                return ByteBuffer(data, size);
            };

            SBufferDescription desc {};
            desc.name                             = fmt::format("{}_uniformbuffer_{}", aMaterialName, uniformBuffer.name);
            desc.dataSource                       = dataSource;
            desc.createInfo.sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            desc.createInfo.pNext                 = nullptr;
            desc.createInfo.flags                 = 0;
            desc.createInfo.size                  = uniformBuffer.location.length;
            desc.createInfo.pQueueFamilyIndices   = nullptr;
            desc.createInfo.queueFamilyIndexCount = 0;
            desc.createInfo.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
            desc.createInfo.usage                 = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

            bufferDescriptions.push_back(desc);
        }

        for(SSampledImage const &sampledImage : aSignature.sampledImages)
        {
            if(not aIncludeSystemBuffers && 2 > sampledImage.set)
            {
                continue;
            }

            VkDescriptorSetLayoutBinding layoutBinding {};
            layoutBinding.binding            = sampledImage.binding;
            layoutBinding.descriptorType     = VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            layoutBinding.stageFlags         = serialization::shaderStageFromPipelineStage(sampledImage.stageBinding.value());
            layoutBinding.descriptorCount    = 1;
            layoutBinding.pImmutableSamplers = nullptr;
            pipelineDescriptor.descriptorSetLayoutBindings[sampledImage.set - setSubtractionValue][sampledImage.binding] = layoutBinding;
        }

        VkViewport viewPort = {};
        viewPort.x        = 0.0;
        viewPort.y        = 0.0;
        viewPort.width    = 1920.0;
        viewPort.height   = 1080.0;
        viewPort.minDepth = 0.0;
        viewPort.maxDepth = 1.0;
        pipelineDescriptor.viewPort = viewPort;

        return {true, pipelineDescriptor, shaderModuleDescriptor, bufferDescriptions };
    };
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    static Shared<CResourceFromAssetResourceObjectCreator<SMaterial>> getAssetLoader(Shared<resources::CResourceManager> const &aResourceManager
                                                                                   , Shared<asset::IAssetStorage>        const &aAssetStorage
                                                                                   , Shared<material::CMaterialLoader>   const &aMaterialLoader)
    {
        static constexpr char const *SHIRABE_MATERIALSYSTEM_CORE_MATERIAL_RESOURCEID = "Core";
        auto const loader = [=] (ResourceId_t const &aResourceId, AssetId_t const &aAssetId) -> Shared<ILogicalResourceObject>
        {
            auto const &[result, instance] = aMaterialLoader->loadMaterialInstance(aResourceId, aAssetStorage, aAssetId, true);
            if(CheckEngineError(result))
            {
                return nullptr;
            }

            Shared<CMaterialMaster> const &master    = instance->master();
            SMaterialSignature      const &signature = master  ->signature();
            CMaterialConfig         const &config    = instance->config();

            bool const includeSystemBuffers =  (SHIRABE_MATERIALSYSTEM_CORE_MATERIAL_RESOURCEID == master->name());
            auto [derivationSuccessful, pipelineDescription, shaderModuleDescription, bufferDescriptions] = deriveResourceDescriptions(aAssetStorage, master->name(), master->signature(), instance->config(), includeSystemBuffers);

            Vector<SSampledImage> sampledImages = signature.sampledImages;
            std::sort(sampledImages.begin(), sampledImages.end(), [] (SSampledImage const &aLHS, SSampledImage const &aRHS) -> bool { return (aLHS.binding < aRHS.binding); });

            Vector<ResourceId_t>  sampledImageResources {};
            for(auto const &sampledImage : sampledImages)
            {
                ResourceId_t const &resourceId = config.getSampledImageAssignment().at(sampledImage.name);
                sampledImageResources.push_back(resourceId);
            }

            SMaterialDescriptor materialDescriptor {};
            materialDescriptor.name                     = aResourceId;
            materialDescriptor.pipelineDescriptor       = pipelineDescription;
            materialDescriptor.shaderModuleDescriptor   = shaderModuleDescription;
            materialDescriptor.uniformBufferDescriptors = bufferDescriptions;
            materialDescriptor.sampledImages            = sampledImageResources;

            std::vector<std::string> pipelineDependencies {};

            CEngineResult<Shared<ILogicalResourceObject>> shaderModuleObject = aResourceManager->useDynamicResource<SShaderModule>(materialDescriptor.shaderModuleDescriptor.name, materialDescriptor.shaderModuleDescriptor);
            EngineStatusPrintOnError(shaderModuleObject.result(),  "Material::AssetLoader", "Failed to create shader module.");

            Shared<SShaderModule> shaderModule = std::static_pointer_cast<SShaderModule>(shaderModuleObject.data());
            pipelineDependencies.push_back(materialDescriptor.shaderModuleDescriptor.name);

            std::vector<Shared<SBuffer>> buffers(materialDescriptor.uniformBufferDescriptors.size());
            for(std::size_t k=0; k<materialDescriptor.uniformBufferDescriptors.size(); ++k)
            {
                auto const &bufferDescriptor = materialDescriptor.uniformBufferDescriptors.at(k);

                CEngineResult<Shared<ILogicalResourceObject>> bufferResourceObject = aResourceManager->useDynamicResource<SBuffer>(bufferDescriptor.name, bufferDescriptor);
                EngineStatusPrintOnError(bufferResourceObject.result(),  "Material::AssetLoader", "Failed to create buffer.");
                pipelineDependencies.push_back(bufferDescriptor.name);

                buffers[k] = std::static_pointer_cast<SBuffer>(bufferResourceObject.data());
            }

            // pipelineDependencies.push_back(renderPassHandle);
            // for(auto const &textureViewHandle : textureViewHandles)
            // {
            //     pipelineDependencies.push_back(textureViewHandle);
            // }

            if(not includeSystemBuffers)
            {
                pipelineDependencies.emplace_back("Core_pipeline");
            }

            CEngineResult<Shared<ILogicalResourceObject>> pipelineObject = aResourceManager->useDynamicResource<SPipeline>(materialDescriptor.pipelineDescriptor.name, materialDescriptor.pipelineDescriptor);
            EngineStatusPrintOnError(pipelineObject.result(),  "Material::AssetLoader", "Failed to create pipeline.");

            Shared<SPipeline> pipeline = std::static_pointer_cast<SPipeline>(pipelineObject.data());

            CEngineResult<Shared<ILogicalResourceObject>> materialObject = aResourceManager->useDynamicResource<SMaterial>(materialDescriptor.name, materialDescriptor);
            Shared<SMaterial> material = std::static_pointer_cast<SMaterial>(materialObject.data());
            material->pipelineResource     = pipeline;
            material->shaderModuleResource = shaderModule;
            material->bufferResources      = std::move(buffers);

            return material;
        };

        return makeShared<CResourceFromAssetResourceObjectCreator<SMaterial>>(loader);
    }
}

#endif //__SHIRABEDEVELOPMENT_ASSETLOADER_H__
