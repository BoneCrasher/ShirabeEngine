//
// Created by dottideveloper on 20.10.19.
//

#ifndef SHIRABEDEVELOPMENT_RESOURCETYPES_H
#define SHIRABEDEVELOPMENT_RESOURCETYPES_H

#include <vector>
#include <platform/platform.h>
#include <core/enginetypehelper.h>
#include <core/databuffer.h>
#include <core/bitfield.h>
#include <graphicsapi/definitions.h>
#include "resources/resourcedescriptions.h"
#include "resources/cresourceobject.h"
#include "resources/resourcedatasource.h"

namespace engine
{
    namespace resources
    {
        using namespace graphicsapi;

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SBuffer
            : public CResourceObject<SBufferDescription, SNoDependencies>
        {
            using CResourceObject<SBufferDescription, SNoDependencies>::CResourceObject;

            SHIRABE_INLINE
            EEngineStatus load(SNoDependencies const &aDependencies) override
            {
                SHIRABE_UNUSED(aDependencies);
                return loadGpuApiResource(aDependencies, {});
            }

            SHIRABE_INLINE
            EEngineStatus unload() override
            {
                return unloadGpuApiResource(*getCurrentDependencies(), {});
            }
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SBufferView
            : public CResourceObject<SBufferViewDescription, SBufferViewDependencies>
        {
            using CResourceObject<SBufferViewDescription, SBufferViewDependencies>::CResourceObject;

            SHIRABE_INLINE
            EEngineStatus load(SBufferViewDependencies const &aDependencies) override
            {
                return loadGpuApiResource(aDependencies, { aDependencies.bufferId });
            }

            SHIRABE_INLINE
            EEngineStatus unload() override
            {
                std::optional<SBufferViewDependencies> const &dependencies = getCurrentDependencies();
                if(false == dependencies.has_value())
                {
                    return EEngineStatus::Error;
                }

                return unloadGpuApiResource(*dependencies, { dependencies->bufferId });
            }
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT STexture
            : public CResourceObject<STextureDescription, SNoDependencies>
        {
            using CResourceObject<STextureDescription, SNoDependencies>::CResourceObject;

            SHIRABE_INLINE
            EEngineStatus load(SNoDependencies const &aDependencies) override
            {
                SHIRABE_UNUSED(aDependencies);
                return loadGpuApiResource(aDependencies, {});
            }

            SHIRABE_INLINE
            EEngineStatus unload() override
            {
                return unloadGpuApiResource(*getCurrentDependencies(), {});
            }
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT STextureView
            : public CResourceObject<STextureViewDescription, STextureViewDependencies>
        {
            using CResourceObject<STextureViewDescription, STextureViewDependencies>::CResourceObject;

            SHIRABE_INLINE
            EEngineStatus load(STextureViewDependencies const &aDependencies) override
            {
                return loadGpuApiResource(aDependencies, { aDependencies.subjacentTextureId });
            }

            SHIRABE_INLINE
            EEngineStatus unload() override
            {
                std::optional<STextureViewDependencies> const &dependencies = getCurrentDependencies();
                if(false == dependencies.has_value())
                {
                    return EEngineStatus::Error;
                }

                return unloadGpuApiResource(*dependencies, { dependencies->subjacentTextureId });
            }

        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SDepthStencilState
            : public CResourceObject<SDepthStencilStateDescription, SNoDependencies>
        {
            using CResourceObject<SDepthStencilStateDescription, SNoDependencies>::CResourceObject;

            SHIRABE_INLINE
            EEngineStatus load(SNoDependencies const &aDependencies) override
            {
                SHIRABE_UNUSED(aDependencies);
                return loadGpuApiResource(aDependencies, {});
            }

            SHIRABE_INLINE
            EEngineStatus unload() override
            {
                return unloadGpuApiResource(*getCurrentDependencies(), {});
            }
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SRasterizerState
            : public CResourceObject<SRasterizerStateDescription, SNoDependencies>
        {
            using CResourceObject<SRasterizerStateDescription, SNoDependencies>::CResourceObject;

            SHIRABE_INLINE
            EEngineStatus load(SNoDependencies const &aDependencies) override
            {
                SHIRABE_UNUSED(aDependencies);
                return loadGpuApiResource(aDependencies, {});
            }

            SHIRABE_INLINE
            EEngineStatus unload() override
            {
                return unloadGpuApiResource(*getCurrentDependencies(), {});
            }
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SSubpass
            : public CResourceObject<SSubpassDescription, SNoDependencies>
        {
            using CResourceObject<SSubpassDescription, SNoDependencies>::CResourceObject;

            SHIRABE_INLINE
            EEngineStatus load(SNoDependencies const &aDependencies) override
            {
                SHIRABE_UNUSED(aDependencies);
                return loadGpuApiResource(aDependencies, {});
            }

            SHIRABE_INLINE
            EEngineStatus unload() override
            {
                return unloadGpuApiResource(*getCurrentDependencies(), {});
            }
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SRenderPass
            : public CResourceObject<SRenderPassDescription, SRenderPassDependencies>
        {
            using CResourceObject<SRenderPassDescription, SRenderPassDependencies>::CResourceObject;

            SHIRABE_INLINE
            EEngineStatus load(SRenderPassDependencies const &aDependencies) override
            {
                return loadGpuApiResource(aDependencies, std::vector<ResourceId_t>(aDependencies.attachmentTextureViews));
            }

            SHIRABE_INLINE
            EEngineStatus unload() override
            {
                std::optional<SRenderPassDependencies> const &dependencies = getCurrentDependencies();
                if(false == dependencies.has_value())
                {
                    return EEngineStatus::Error;
                }

                return unloadGpuApiResource(*dependencies, std::vector<ResourceId_t>(dependencies->attachmentTextureViews));
            }
        };

        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SFrameBuffer
            : public CResourceObject<SFrameBufferDescription, SFrameBufferDependencies>
        {
            using CResourceObject<SFrameBufferDescription, SFrameBufferDependencies>::CResourceObject;

            SHIRABE_INLINE
            EEngineStatus load(SFrameBufferDependencies const &aDependencies) override
            {
                std::vector<ResourceId_t> dependencies {};
                dependencies.push_back(aDependencies.referenceRenderPassId);
                for(auto const &view : aDependencies.attachmentTextureViews)
                {
                    dependencies.push_back(view);
                }

                return loadGpuApiResource(aDependencies, std::move(dependencies));
            }

            SHIRABE_INLINE
            EEngineStatus unload() override
            {
                std::optional<SFrameBufferDependencies> const &dependencies = getCurrentDependencies();
                if(false == dependencies.has_value())
                {
                    return EEngineStatus::Error;
                }

                return unloadGpuApiResource(*dependencies, { dependencies->referenceRenderPassId });
            }
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SSwapChainBuffer
            : public CResourceObject<SSwapChainBufferDescription, SNoDependencies>
        {
            using CResourceObject<SSwapChainBufferDescription, SNoDependencies>::CResourceObject;

            SHIRABE_INLINE
            EEngineStatus load(SNoDependencies const &aDependencies) override
            {
                SHIRABE_UNUSED(aDependencies);
                return loadGpuApiResource(aDependencies, {});
            }

            SHIRABE_INLINE
            EEngineStatus unload() override
            {
                return unloadGpuApiResource(*getCurrentDependencies(), {});
            }
        };

        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SSwapChain
            : public CResourceObject<SSwapChainDescription, SNoDependencies>
        {
            using CResourceObject<SSwapChainDescription, SNoDependencies>::CResourceObject;

            SHIRABE_INLINE
            EEngineStatus load(SNoDependencies const &aDependencies) override
            {
                SHIRABE_UNUSED(aDependencies);
                return loadGpuApiResource(aDependencies, {});
            }

            SHIRABE_INLINE
            EEngineStatus unload() override
            {
                return unloadGpuApiResource(*getCurrentDependencies(), {});
            }
        };

        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SPipeline
            : public CResourceObject<SMaterialPipelineDescriptor, SMaterialPipelineDependencies>
        {
            using CResourceObject<SMaterialPipelineDescriptor, SMaterialPipelineDependencies>::CResourceObject;

            SHIRABE_INLINE
            EEngineStatus load(SMaterialPipelineDependencies const &aDependencies) override
            {
                std::vector<ResourceId_t> dependencies {};
                dependencies.push_back(aDependencies.referenceRenderPassId);
                for(auto const &buffer : aDependencies.bufferViewIds)
                {
                    dependencies.push_back(buffer);
                }

                return loadGpuApiResource(aDependencies, std::move(dependencies));
            }

            SHIRABE_INLINE
            EEngineStatus unload() override
            {
                std::optional<SMaterialPipelineDependencies> const &currentDependencies = getCurrentDependencies();
                if(false == currentDependencies.has_value())
                {
                    return EEngineStatus::Error;
                }

                std::vector<ResourceId_t> dependencies {};
                dependencies.push_back(currentDependencies->referenceRenderPassId);
                for(auto const &buffer : currentDependencies->bufferViewIds)
                {
                    dependencies.push_back(buffer);
                }

                return unloadGpuApiResource(*currentDependencies, std::move(dependencies));
            }
        };

        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SShaderModule
            : public CResourceObject<SShaderModuleDescriptor, SNoDependencies>
        {
            using CResourceObject<SShaderModuleDescriptor, SNoDependencies>::CResourceObject;

            SHIRABE_INLINE
            EEngineStatus load(SNoDependencies const &aDependencies) override
            {
                SHIRABE_UNUSED(aDependencies);
                return loadGpuApiResource(aDependencies, {});
            }

            SHIRABE_INLINE
            EEngineStatus unload() override
            {
                return unloadGpuApiResource(*getCurrentDependencies(), {});
            }
        };

        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SMaterial
            : public CResourceObject<SMaterialDescriptor, SMaterialDependencies>
        {
            using CResourceObject<SMaterialDescriptor, SMaterialDependencies>::CResourceObject;

            Shared<SPipeline>       pipelineResource;
            Shared<SShaderModule>   shaderModuleResource;
            Vector<Shared<SBuffer>> bufferResources;

            SHIRABE_INLINE
            EEngineStatus load(SMaterialDependencies const &aDependencies) override
            {
                for(auto const &buffer : bufferResources)
                {
                    EEngineStatus const bufferStatus = buffer->load({});
                    EngineStatusPrintOnError(bufferStatus, "SMaterial::load", "Failed to load buffer.");
                }

                EEngineStatus const shaderModuleStatus = shaderModuleResource->load({});
                EngineStatusPrintOnError(shaderModuleStatus, "SMaterial::load", "Failed to load shader module.");

                EEngineStatus const pipelineStatus = pipelineResource->load(aDependencies.pipelineDependencies);
                EngineStatusPrintOnError(pipelineStatus, "SMaterial::load", "Failed to load pipeline.");

                return EEngineStatus::Ok;
            }

            SHIRABE_INLINE
            EEngineStatus unload() override
            {
                std::optional<SMaterialDependencies> const &dependencies = getCurrentDependencies();
                if(false == dependencies.has_value())
                {
                    return EEngineStatus::Error;
                }

                EEngineStatus const pipelineStatus = pipelineResource->load(dependencies->pipelineDependencies);
                EngineStatusPrintOnError(pipelineStatus, "SMaterial::load", "Failed to load pipeline.");

                EEngineStatus const shaderModuleStatus = shaderModuleResource->load({});
                EngineStatusPrintOnError(shaderModuleStatus, "SMaterial::load", "Failed to load shader module.");

                for(auto const &buffer : bufferResources)
                {
                    EEngineStatus const bufferStatus = buffer->load({});
                    EngineStatusPrintOnError(bufferStatus, "SMaterial::load", "Failed to load buffer.");
                }

                resetCurrentDependencies();
                return EEngineStatus::Ok;
            }
        };

    }
}

#endif //SHIRABEDEVELOPMENT_RESOURCETYPES_H
