
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto const configureInputAssembly =
                           [] (SRHIPipelineDescriptor &aPipelineDescriptor) -> void
                               {
                                   aPipelineDescriptor.inputAssemblyState.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
                                   aPipelineDescriptor.inputAssemblyState.pNext                  = nullptr;
                                   aPipelineDescriptor.inputAssemblyState.flags                  = 0;
                                   aPipelineDescriptor.inputAssemblyState.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
                                   aPipelineDescriptor.inputAssemblyState.primitiveRestartEnable = false;
                               };

            auto const configureRasterizer =
                           [] (SRHIPipelineDescriptor &aPipelineDescriptor) -> void
                               {
                                   aPipelineDescriptor.rasterizerState.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
                                   aPipelineDescriptor.rasterizerState.pNext                   = nullptr;
                                   aPipelineDescriptor.rasterizerState.flags                   = 0;
                                   aPipelineDescriptor.rasterizerState.cullMode                = VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT;
                                   aPipelineDescriptor.rasterizerState.frontFace               = VkFrontFace::VK_FRONT_FACE_COUNTER_CLOCKWISE;
                                   aPipelineDescriptor.rasterizerState.polygonMode             = VkPolygonMode::VK_POLYGON_MODE_FILL;
                                   aPipelineDescriptor.rasterizerState.lineWidth               = 1.0f;
                                   aPipelineDescriptor.rasterizerState.rasterizerDiscardEnable = VK_FALSE; // isCoreMaterial ? VK_TRUE : VK_FALSE;
                                   aPipelineDescriptor.rasterizerState.depthClampEnable        = VK_FALSE;
                                   aPipelineDescriptor.rasterizerState.depthBiasEnable         = VK_FALSE;
                                   aPipelineDescriptor.rasterizerState.depthBiasSlopeFactor    = 0.0f;
                                   aPipelineDescriptor.rasterizerState.depthBiasConstantFactor = 0.0f;
                                   aPipelineDescriptor.rasterizerState.depthBiasClamp          = 0.0f;
                               };

            auto const configureMultisampler =
                           [] (SRHIPipelineDescriptor &aPipelineDescriptor) -> void
                               {
                                   aPipelineDescriptor.multiSampler.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
                                   aPipelineDescriptor.multiSampler.pNext                 = nullptr;
                                   aPipelineDescriptor.multiSampler.flags                 = 0;
                                   aPipelineDescriptor.multiSampler.sampleShadingEnable   = VK_FALSE;
                                   aPipelineDescriptor.multiSampler.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
                                   aPipelineDescriptor.multiSampler.minSampleShading      = 1.0f;
                                   aPipelineDescriptor.multiSampler.pSampleMask           = nullptr;
                                   aPipelineDescriptor.multiSampler.alphaToCoverageEnable = VK_FALSE;
                                   aPipelineDescriptor.multiSampler.alphaToOneEnable      = VK_FALSE;
                               };

            auto const configureDepthStencil =
                           [] (SRHIPipelineDescriptor &aPipelineDescriptor) -> void
                               {
                                   aPipelineDescriptor.depthStencilState.sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
                                   aPipelineDescriptor.depthStencilState.pNext                 = nullptr;
                                   aPipelineDescriptor.depthStencilState.flags                 = 0;
                                   aPipelineDescriptor.depthStencilState.depthTestEnable       = VK_TRUE;
                                   aPipelineDescriptor.depthStencilState.depthWriteEnable      = VK_TRUE;
                                   aPipelineDescriptor.depthStencilState.depthCompareOp        = VkCompareOp::VK_COMPARE_OP_LESS;
                                   aPipelineDescriptor.depthStencilState.stencilTestEnable     = VK_FALSE;
                                   aPipelineDescriptor.depthStencilState.front.passOp          = VkStencilOp::VK_STENCIL_OP_KEEP;
                                   aPipelineDescriptor.depthStencilState.front.failOp          = VkStencilOp::VK_STENCIL_OP_KEEP;
                                   aPipelineDescriptor.depthStencilState.front.depthFailOp     = VkStencilOp::VK_STENCIL_OP_KEEP;
                                   aPipelineDescriptor.depthStencilState.front.compareOp       = VkCompareOp::VK_COMPARE_OP_ALWAYS;
                                   aPipelineDescriptor.depthStencilState.front.compareMask     = 0;
                                   aPipelineDescriptor.depthStencilState.front.writeMask       = 0;
                                   aPipelineDescriptor.depthStencilState.front.reference       = 0;
                                   aPipelineDescriptor.depthStencilState.back                  = aPipelineDescriptor.depthStencilState.front;
                                   aPipelineDescriptor.depthStencilState.depthBoundsTestEnable = VK_FALSE;
                                   aPipelineDescriptor.depthStencilState.minDepthBounds        = 0.0f;
                                   aPipelineDescriptor.depthStencilState.maxDepthBounds        = 1.0f;
                               };

            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto initializeMaterial(Shared<CVulkanEnvironment>     aVulkanEnvironment
                                    , Shared<CRHIResourceManager>     aResourceManager
                                    , Shared<asset::CAssetStorage> aAssetStorage
                                    , SRenderGraphMaterial   const &aMaterial
                                    , ResourceId_t          const &aRenderPassId) -> EEngineStatus
            {
                OptRef_t<MaterialResourceState_t> materialOpt {};
                {
                    auto [success, resource] = fetchResource<MaterialResourceState_t>(aResourceManager, aMaterial.readableName);
                    if(not success)
                    {
                        return EEngineStatus::Ok;
                    }
                    materialOpt = resource;
                }
                MaterialResourceState_t &material = *materialOpt;

                SMaterialPipelineDependencies pipelineDependencies {};
                pipelineDependencies.systemUBOPipelineId   = "Core_pipeline";
                pipelineDependencies.referenceRenderPassId = aRenderPassId;
                pipelineDependencies.subpass               = aState.currentSubpassIndex;
                pipelineDependencies.shaderModuleId        = material.description.shaderModuleDescriptor.name;

                for(auto const &buffer : material.description.uniformBufferDescriptors)
                {
                    SNoDependencies dependencies {};

                    CEngineResult<> const bufferInitialization = aResourceManager->initializeResource<RHIBufferResourceState_t>(buffer.name, aVulkanEnvironment);
                    EngineStatusPrintOnError(bufferInitialization.result(), logTag(), "Failed to initialize buffer.");
                    SHIRABE_RETURN_RESULT_ON_ERROR(bufferInitialization.result());
                }

                for(auto const &sampledImageAssetId : material.description.sampledImages)
                {
                    std::string const sampledImageResourceId = fmt::format("{}", sampledImageAssetId);

                    OptRef_t <RHIResourceState_t> sampledImageOpt{};
                    {
                        auto[success, resource] = fetchResource<RHIResourceState_t>(aResourceManager, sampledImageResourceId);
                        if( not success )
                        {
                            return EEngineStatus::Ok;
                        }
                        sampledImageOpt = resource;
                    }
                    RHIResourceState_t &sampledImage = *sampledImageOpt;

                    std::string const sampledImageViewResourceId = fmt::format("{}_{}_view", material.description.name, sampledImage.description.name);
                    OptRef_t<RHIImageViewResourceState_t> sampledImageViewOpt {};
                    {
                        auto [success, resource] = fetchResource<RHIImageViewResourceState_t>(aResourceManager, sampledImageViewResourceId);
                        if(not success)
                        {
                            return EEngineStatus::Ok;
                        }
                        sampledImageViewOpt = resource;
                    }
                    RHIImageViewResourceState_t &sampledImageView = *sampledImageViewOpt;

                    CEngineResult<> const textureInitialization = aResourceManager->initializeResource<RHIResourceState_t>(sampledImageResourceId, aVulkanEnvironment);
                    EngineStatusPrintOnError(textureInitialization.result(), logTag(), "Failed to initialize texture.");
                    SHIRABE_RETURN_RESULT_ON_ERROR(textureInitialization.result());

                    STextureViewDependencies textureViewInitDependencies {};
                    textureViewInitDependencies.subjacentImageId = sampledImageResourceId;

                    CEngineResult<> const textureViewInitialization = aResourceManager->initializeResource<RHIImageViewResourceState_t>(sampledImageViewResourceId, aVulkanEnvironment);
                    EngineStatusPrintOnError(textureViewInitialization.result(), logTag(), "Failed to initialize texture view.");
                    SHIRABE_RETURN_RESULT_ON_ERROR(textureViewInitialization.result());
                }

                CEngineResult<> const shaderModuleInitialization = aResourceManager->initializeResource<RHIShaderModuleResourceState_t>(material.description.shaderModuleDescriptor.name, aVulkanEnvironment);
                EngineStatusPrintOnError(shaderModuleInitialization.result(), logTag(), "Failed to initialize shader module.");
                SHIRABE_RETURN_RESULT_ON_ERROR(shaderModuleInitialization.result());

                CEngineResult<> const pipelineInitialization = aResourceManager->initializeResource<RHIPipelineResourceState_t>(material.description.basePipelineDescriptor.name, aVulkanEnvironment);
                EngineStatusPrintOnError(pipelineInitialization.result(), logTag(), "Failed to initialize pipeline.");
                SHIRABE_RETURN_RESULT_ON_ERROR(pipelineInitialization.result());
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto transferMaterial(Shared<CVulkanEnvironment>     aVulkanEnvironment
                                  , Shared<CRHIResourceManager>     aResourceManager
                                  , Shared<asset::CAssetStorage> aAssetStorage
                                  , SRenderGraphMaterial   const &aMaterial) -> EEngineStatus
            {
                VkDevice device = aVulkanEnvironment->getLogicalDevice();

                OptRef_t<MaterialResourceState_t> materialOpt {};
                {
                    auto [success, resource] = fetchResource<MaterialResourceState_t>(aResourceManager, aMaterial.readableName);
                    if(not success)
                    {
                        return EEngineStatus::Ok;
                    }
                    materialOpt = resource;
                }
                MaterialResourceState_t &material = *materialOpt;

                for(auto const &bufferDesc : material.description.uniformBufferDescriptors)
                {
                    OptRef_t<RHIBufferResourceState_t> bufferOpt {};
                    {
                        auto [success, resource] = fetchResource<RHIBufferResourceState_t>(aResourceManager, bufferDesc.name);
                        if(not success)
                        {
                            return EEngineStatus::Ok;
                        }
                        bufferOpt = resource;
                    }
                    RHIBufferResourceState_t &buffer = *bufferOpt;

                    transferBufferData(device, bufferDesc.dataSource(), buffer.rhiHandles.attachedMemory);
                }

                for(auto const &sampledImageAssetId : material.description.sampledImages)
                {
                    std::string const sampledImageResourceId = fmt::format("{}", sampledImageAssetId);

                    OptRef_t <RHIResourceState_t> sampledImageOpt{};
                    {
                        auto[success, resource] = fetchResource<RHIResourceState_t>(aResourceManager, sampledImageResourceId);
                        if( not success )
                        {
                            return EEngineStatus::Ok;
                        }
                        sampledImageOpt = resource;
                    }
                    RHIResourceState_t &sampledImage = *sampledImageOpt;

                    // Make sure, that the texture is in the correct pre-transfer layout!
                    if(sampledImage.description.gpuBinding.check(EBufferBinding::TextureInput))
                    {
                        EEngineStatus const layoutTransfer =
                                                performImageLayoutTransferImpl(aState
                                                                               , sampledImage
                                                                               , CRange(0, 1)
                                                                               , CRange(0, 1)
                                                                               , VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT
                                                                               , VK_IMAGE_LAYOUT_UNDEFINED
                                                                               , VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

                        EngineStatusPrintOnError(layoutTransfer, logTag(), "Failed to transfer texture.");
                        SHIRABE_RETURN_RESULT_ON_ERROR(layoutTransfer);
                    }

                    CEngineResult<> const textureTransfer = aResourceManager->transferResource<RHIResourceState_t>(sampledImageResourceId, aVulkanEnvironment);
                    EngineStatusPrintOnError(textureTransfer.result(), logTag(), "Failed to transfer texture.");
                    SHIRABE_RETURN_RESULT_ON_ERROR(textureTransfer.result());

                    // Make sure, that the texture is in the correct post-transfer layout!
                    if(sampledImage.description.gpuBinding.check(EBufferBinding::TextureInput))
                    {
                        EEngineStatus const layoutTransfer =
                                                performImageLayoutTransferImpl(aState
                                                                               , sampledImage
                                                                               , CRange(0, 1)
                                                                               , CRange(0, 1)
                                                                               , VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT
                                                                               , VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
                                                                               , VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

                        EngineStatusPrintOnError(layoutTransfer, logTag(), "Failed to transfer texture.");
                        SHIRABE_RETURN_RESULT_ON_ERROR(layoutTransfer);
                    }
                }
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            struct SSampledImageBinding
            {
                OptRef_t<RHIImageViewResourceState_t> imageView;
                OptRef_t<RHIResourceState_t>     image;
            };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            auto const bindPipeline = [=] (RHIPipelineResourceState_t &aPipeline) -> EEngineStatus
                {
                SVulkanState     &vkState        = aVulkanEnvironment->getState();
                VkCommandBuffer  vkCommandBuffer = aVulkanEnvironment->getVkCurrentFrameContext()->getGraphicsCommandBuffer(); // The commandbuffers and swapchain count currently match

                vkCmdBindPipeline(vkCommandBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, aPipeline.rhiHandles.pipeline);

                vkCmdBindDescriptorSets(vkCommandBuffer
                                        , VK_PIPELINE_BIND_POINT_GRAPHICS
                                        , aPipeline.rhiHandles.pipelineLayout
                                        , 0
                                        , aPipeline.rhiHandles.descriptorSets.size()
                                        , aPipeline.rhiHandles.descriptorSets.data()
                                        , 0, nullptr);

                return EEngineStatus::Ok;
                };
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            context.bindMaterial = [=] (SRenderGraphMaterial const &aMaterial) -> EEngineStatus
                {
                VkDevice device = aVulkanEnvironment->getLogicalDevice();

                OptRef_t<MaterialResourceState_t> materialOpt {};
                {
                    auto [success, resource] = fetchResource<MaterialResourceState_t>(aResourceManager, aMaterial.readableName);
                    if(not success)
                    {
                        return EEngineStatus::Ok;
                    }
                    materialOpt = resource;
                }
                MaterialResourceState_t &material = *materialOpt;

                OptRef_t<RHIRenderPassResourceState_t> renderPassOpt {};
                {
                    auto [success, resource] = fetchResource<RHIRenderPassResourceState_t>(aResourceManager, material.dependencies.pipelineDependencies.referenceRenderPassId);
                    if(not success)
                    {
                        return EEngineStatus::Ok;
                    }
                    renderPassOpt = resource;
                }
                RHIRenderPassResourceState_t &renderPass = *renderPassOpt;

                std::vector<OptRef_t<RHIBufferResourceState_t>>      buffers           {};
                std::vector<OptRef_t<RHIImageViewResourceState_t>> inputAttachments  {};
                std::vector<SSampledImageBinding>                 textureViews      {};

                for(auto const &bufferDesc : material.description.uniformBufferDescriptors)
                {
                    OptRef_t<RHIBufferResourceState_t> bufferOpt {};
                    {
                        auto [success, resource] = fetchResource<RHIBufferResourceState_t>(aResourceManager, bufferDesc.name);
                        if(not success)
                        {
                            return EEngineStatus::Ok;
                        }
                        bufferOpt = resource;
                    }
                    buffers.push_back(bufferOpt);
                }

                SRHISubpassDescription const &subPassDesc = renderPass.description.subpassDescriptions.at(aState.currentSubpassIndex);
                for(auto const &inputAttachment : subPassDesc.inputAttachments)
                {
                    uint32_t     const &attachmentIndex           = inputAttachment.attachment;
                    ResourceId_t const &attachementResourceHandle = renderPass.dependencies.attachmentTextureViews.at(attachmentIndex);

                    OptRef_t<RHIImageViewResourceState_t> textureViewOpt {};
                    {
                        auto [success, resource] = fetchResource<RHIImageViewResourceState_t>(aResourceManager, attachementResourceHandle);
                        if(not success)
                        {
                            return EEngineStatus::Ok;
                        }
                        textureViewOpt = resource;
                    }
                    inputAttachments.push_back(textureViewOpt);
                }

                for(auto const &sampledImageAssetId : material.description.sampledImages)
                {
                    OptRef_t<RHIResourceState_t> sampledImageOpt {};
                    {
                        std::string const sampledImageResourceId = fmt::format("{}", sampledImageAssetId);
                        auto [success, resource] = fetchResource<RHIResourceState_t>(aResourceManager, sampledImageResourceId);
                        if(not success)
                        {
                            return EEngineStatus::Ok;
                        }
                        sampledImageOpt = resource;
                    }

                    if(not sampledImageOpt.has_value())
                    {
                        textureViews.push_back({}); // Fill gaps
                    }
                    else
                    {
                        RHIResourceState_t &sampledImage = *sampledImageOpt;

                        std::string const sampledImageViewResourceId = fmt::format("{}_{}_view", material.description.name, sampledImage.description.name);

                        OptRef_t<RHIImageViewResourceState_t> sampledImageViewOpt {};
                        {
                            auto [success, resource] = fetchResource<RHIImageViewResourceState_t>(aResourceManager, sampledImageViewResourceId);
                            if(not success)
                            {
                                return EEngineStatus::Ok;
                            }
                            sampledImageViewOpt = resource;
                        }

                        SSampledImageBinding binding {};
                        binding.image     = sampledImageOpt;
                        binding.imageView = sampledImageViewOpt;

                        textureViews.push_back(binding);
                    }
                }

                EEngineStatus const updateResult = updateDescriptorSets(aState
                                                                        , material
                                                                        , buffers
                                                                        , inputAttachments
                                                                        , textureViews);

                OptRef_t<RHIPipelineResourceState_t> pipelineResourceOpt {};
                {
                    auto [success, resource] = fetchResource<RHIPipelineResourceState_t>(aResourceManager, material.description.basePipelineDescriptor.name);
                    if(not success)
                    {
                        return EEngineStatus::Ok;
                    }
                    pipelineResourceOpt = resource;
                }
                RHIPipelineResourceState_t &pipelineResource = *pipelineResourceOpt;

                auto const result = bindPipeline(aState, pipelineResource);
                return result;
                };
            //<-----------------------------------------------------------------------------
        }
