//
// Created by dotti on 24.11.19.
//

#ifndef __SHIRABEDEVELOPMENT_ASSETLOADER_H__
#define __SHIRABEDEVELOPMENT_ASSETLOADER_H__

#include <asset/assetstorage.h>
#include <resources/resourcedescriptions.h>
#include <resources/resourcetypes.h>
#include <resources/cresourcemanager.h>
#include "material/material_declaration.h"
#include "material/materialserialization.h"

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
    static Shared<CResourceFromAssetResourceObjectCreator<SMaterial>> getAssetLoader(Shared<resources::CResourceManager> const &aResourceManager
                                                                                   , Shared<asset::IAssetStorage>        const &aAssetStorage
                                                                                   , Shared<material::CMaterialLoader>   const &aMaterialLoader)
    {
        auto const loader = [=] (ResourceId_t const &aResourceId, AssetId_t const &aAssetId) -> Shared<ILogicalResourceObject>
        {
            auto const &[result, instance] = aMaterialLoader->loadMaterialInstance(aAssetStorage, aAssetId);
            if(CheckEngineError(result))
            {
                return nullptr;
            }

            Shared<CMaterialMaster> const &master    = instance->master();
            SMaterialSignature      const &signature = master  ->signature();
            CMaterialConfig         const &config    = instance->config();

            auto const [derivationSuccessful, pipelineDescription, shaderModuleDescription, bufferDescriptions] = deriveResourceDescriptions(aAssetStorage, master->name(), master->signature(), instance->config());
            //master->setPipelineDescription    (pipelineDescription);
            //master->setShaderModuleDescription(shaderModuleDescription);
            //master->setBufferDescriptions     (bufferDescriptions);

            SMaterialDescriptor materialDescriptor {};
            materialDescriptor.name                     = aResourceId;
            materialDescriptor.pipelineDescriptor       = pipelineDescription;
            materialDescriptor.shaderModuleDescriptor   = shaderModuleDescription;
            materialDescriptor.uniformBufferDescriptors = bufferDescriptions;

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

            CEngineResult<Shared<ILogicalResourceObject>> pipelineObject = aResourceManager->useDynamicResource<SPipeline>(materialDescriptor.pipelineDescriptor.name, materialDescriptor.pipelineDescriptor, std::move(pipelineDependencies));
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