//
// Created by dotti on 24.11.19.
//

#ifndef __SHIRABEDEVELOPMENT_MATERIAL_ADAPTER_H__
#define __SHIRABEDEVELOPMENT_MATERIAL_ADAPTER_H__

#include <asset/assetstorage.h>
#include <asset/material/serialization.h>

#include <renderer/resource_management/cresourcemanager.h>
#include <renderer/resource_management/resourcedescriptions.h>
#include "materialsystem/declaration.h"

namespace engine
{
    namespace resources
    {
        using namespace asset;
        using namespace material;

        template<>
        class CResourceCreator<CMaterialInstance, SMaterialDescription>
        {
        public:
            static OptionalRef_t<vulkan::MaterialResourceState_t>
                    create(Shared<CAssetStorage>       aAssetStorage
                           , Shared<CResourceManager>  aResourceManager
                           , Shared<CMaterialInstance> aInstance)
            {
                using namespace vulkan;

                static constexpr char const *SHIRABE_MATERIALSYSTEM_CORE_MATERIAL_RESOURCEID = "Core";

                Shared<CSharedMaterial> const &sharedMaterial = aInstance->sharedMaterial();

                bool const isCoreMaterial = (SHIRABE_MATERIALSYSTEM_CORE_MATERIAL_RESOURCEID == sharedMaterial->name());

                SMaterialBasePipelineDescriptor basePipelineDescriptor {};
                SShaderModuleDescriptor         shaderModuleDescriptor {};

                basePipelineDescriptor.name = fmt::format("{}_{}", sharedMaterial->name(), "basepipeline");
                shaderModuleDescriptor.name = fmt::format("{}_{}", sharedMaterial->name(), "shadermodule");

                for(auto const &[stageKey, stage] : sharedMaterial->stages())
                {
                    //
                    // Derive data accessors for shader module creation from sharedMaterial material
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

                // uint32_t const setSubtractionValue = includeSystemBuffers ? 0 : 2;
                uint32_t const setCount = sharedMaterial->layoutInfo().setCount; // - setSubtractionValue;

                std::vector<VkDescriptorSetLayoutCreateInfo> descriptorSets {};
                descriptorSets.resize(setCount);
                basePipelineDescriptor.descriptorSetLayoutBindings.resize(setCount);

                for(std::size_t k=0; k<descriptorSets.size(); ++k)
                {
                    VkDescriptorSetLayoutCreateInfo &info = descriptorSets[k];

                    info.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
                    info.pNext        = nullptr;
                    info.flags        = 0;
                    info.bindingCount = sharedMaterial->layoutInfo().setBindingCount[k /* + setSubtractionValue*/];

                    basePipelineDescriptor.descriptorSetLayoutBindings[k].resize(info.bindingCount);
                }

                basePipelineDescriptor.descriptorSetLayoutCreateInfos = descriptorSets;

                for(SSubpassInput const &input : sharedMaterial->subpassInputs())
                {
                    if(not isCoreMaterial && 2 > input.set)
                        continue;

                    VkDescriptorSetLayoutBinding layoutBinding {};
                    layoutBinding.binding            = input.binding;
                    layoutBinding.descriptorType     = VkDescriptorType::VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
                    layoutBinding.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT; // Subpass inputs are only accessibly in fragment shaders.
                    layoutBinding.descriptorCount    = 1;
                    layoutBinding.pImmutableSamplers = nullptr;
                    basePipelineDescriptor.descriptorSetLayoutBindings[input.set /* - setSubtractionValue */][input.binding] = layoutBinding;
                }

                for(SUniformBuffer const &uniformBuffer : sharedMaterial->uniformBuffers())
                {
                    if(not isCoreMaterial && 2 > uniformBuffer.set)
                        continue;

                    VkDescriptorSetLayoutBinding layoutBinding {};
                    layoutBinding.binding            = uniformBuffer.binding;
                    layoutBinding.descriptorType     = VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                    layoutBinding.stageFlags         = VkShaderStageFlagBits::VK_SHADER_STAGE_ALL; // serialization::shaderStageFromPipelineStage(uniformBuffer.stageBinding.value());
                    layoutBinding.descriptorCount    = uniformBuffer.array.layers;
                    layoutBinding.pImmutableSamplers = nullptr;
                    basePipelineDescriptor.descriptorSetLayoutBindings[uniformBuffer.set /* - setSubtractionValue */][uniformBuffer.binding] = layoutBinding;
                }

                for(SUniformBuffer const &storageBuffer : sharedMaterial->storageBuffers())
                {
                    if(not isCoreMaterial && 2 > storageBuffer.set)
                        continue;

                    VkDescriptorSetLayoutBinding layoutBinding {};
                    layoutBinding.binding            = storageBuffer.binding;
                    layoutBinding.descriptorType     = VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                    layoutBinding.stageFlags         = VkShaderStageFlagBits::VK_SHADER_STAGE_ALL;
                    layoutBinding.descriptorCount    = storageBuffer.array.layers;
                    layoutBinding.pImmutableSamplers = nullptr;
                    basePipelineDescriptor.descriptorSetLayoutBindings[storageBuffer.set /* - setSubtractionValue */][storageBuffer.binding] = layoutBinding;
                }

                for(SSampledImage const &sampledImage : sharedMaterial->sampledImages())
                {
                    if(not isCoreMaterial && 2 > sampledImage.set)
                        continue;

                    VkDescriptorSetLayoutBinding layoutBinding {};
                    layoutBinding.binding            = sampledImage.binding;
                    layoutBinding.descriptorType     = VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    layoutBinding.stageFlags         = serialization::shaderStageFromPipelineStage(sampledImage.stageBinding.value());
                    layoutBinding.descriptorCount    = 1;
                    layoutBinding.pImmutableSamplers = nullptr;
                    basePipelineDescriptor.descriptorSetLayoutBindings[sampledImage.set /* - setSubtractionValue */][sampledImage.binding] = layoutBinding;
                }

                Vector <SSampledImage> sampledImages = sharedMaterial->sampledImages();
                auto const pred = [](SSampledImage const   &aLHS
                                     , SSampledImage const &aRHS) -> bool
                    {
                        return (aLHS.binding < aRHS.binding);
                    };

                std::sort(sampledImages.begin(), sampledImages.end(), pred);

                Vector <asset::AssetId_t> sampledImageResources{};
                for(auto const &sampledImage : sampledImages)
                {
                    CMaterialConfig::SampledImageMap_t const &assignment = aInstance->config().getSampledImageAssignment();
                    if( assignment.end() == assignment.find(sampledImage.name))
                    {
                        sampledImageResources.push_back(asset::AssetId_t{}); // Fill gaps...
                        continue;
                    }

                    asset::AssetId_t const &assetId = assignment.at(sampledImage.name);

                    sampledImageResources.push_back(assetId);
                }

                SMaterialDescription materialDescriptor{};
                materialDescriptor.name = aInstance->name();
                {
                    auto const &[status, resourceState] =
                        aResourceManager->useResource<BasePipelineResourceState_t>(basePipelineDescriptor.name, basePipelineDescriptor);
                    if(CheckEngineError(status))
                    {
                        // TODO: Error log
                        return {};
                    }

                    materialDescriptor.basePipelineId = basePipelineDescriptor.name;
                }

                {
                    auto const &[status, resourceState] =
                        aResourceManager->useResource<ShaderModuleResourceState_t>(shaderModuleDescriptor.name, shaderModuleDescriptor);
                    if(CheckEngineError(status))
                    {
                        // TODO: Error log
                        return {};
                    }

                    materialDescriptor.shaderModuleId = shaderModuleDescriptor.name;
                }

                {
                    auto const &[status, resourceState] =
                        aResourceManager->useResource<MaterialResourceState_t>(materialDescriptor.name, materialDescriptor);
                    if(CheckEngineError(status))
                    {
                        // TODO: Error log
                        return {};
                    }

                    return resourceState;
                }
            }
        };
    }
}

#endif //__SHIRABEDEVELOPMENT_ASSETLOADER_H__
