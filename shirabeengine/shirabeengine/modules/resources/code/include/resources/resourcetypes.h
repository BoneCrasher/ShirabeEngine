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

        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SBuffer
            : public CResourceObject<SBufferDescription, SNoDependencies>
        {
            using CResourceObject<SBufferDescription, SNoDependencies>::CResourceObject;
        };

        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SBufferView
            : public CResourceObject<SBufferViewDescription, SBufferViewDependencies>
        {
            using CResourceObject<SBufferViewDescription, SBufferViewDependencies>::CResourceObject;
        };

        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT STexture
            : public CResourceObject<STextureDescription, SNoDependencies>
        {
            using CResourceObject<STextureDescription, SNoDependencies>::CResourceObject;
        };

        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT STextureView
            : public CResourceObject<STextureViewDescription, STextureViewDependencies>
        {
            using CResourceObject<STextureViewDescription, STextureViewDependencies>::CResourceObject;
        };

        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SDepthStencilState
            : public CResourceObject<SDepthStencilStateDescription, SNoDependencies>
        {
            using CResourceObject<SDepthStencilStateDescription, SNoDependencies>::CResourceObject;
        };

        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SRasterizerState
            : public CResourceObject<SRasterizerStateDescription, SNoDependencies>
        {
            using CResourceObject<SRasterizerStateDescription, SNoDependencies>::CResourceObject;
        };

        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SSubpass
            : public CResourceObject<SSubpassDescription, SNoDependencies>
        {
            using CResourceObject<SSubpassDescription, SNoDependencies>::CResourceObject;
        };

        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SRenderPass
            : public CResourceObject<SRenderPassDescription, SRenderPassDependencies>
        {
            using CResourceObject<SRenderPassDescription, SRenderPassDependencies>::CResourceObject;
        };

        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SFrameBuffer
            : public CResourceObject<SFrameBufferDescription, SFrameBufferDependencies>
        {
            using CResourceObject<SFrameBufferDescription, SFrameBufferDependencies>::CResourceObject;
        };

        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SSwapChainBuffer
            : public CResourceObject<SSwapChainBufferDescription, SNoDependencies>
        {
            using CResourceObject<SSwapChainBufferDescription, SNoDependencies>::CResourceObject;
        };

        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SSwapChain
            : public CResourceObject<SSwapChainDescription, SNoDependencies>
        {
            using CResourceObject<SSwapChainDescription, SNoDependencies>::CResourceObject;
        };

        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SPipeline
            : public CResourceObject<SMaterialPipelineDescriptor, SMaterialPipelineDependencies>
        {
            using CResourceObject<SMaterialPipelineDescriptor, SMaterialPipelineDependencies>::CResourceObject;
        };

        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SShaderModule
            : public CResourceObject<SShaderModuleDescriptor, SNoDependencies>
        {
            using CResourceObject<SShaderModuleDescriptor, SNoDependencies>::CResourceObject;

        };

        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SMaterial
            : public CResourceObject<SMaterialDescriptor, SNoDependencies>
        {
            using CResourceObject<SMaterialDescriptor, SNoDependencies>::CResourceObject;

            Shared<SPipeline>       pipelineResource;
            Shared<SShaderModule>   shaderModuleResource;
            Vector<Shared<SBuffer>> bufferResources;
        };

    }
}

#endif //SHIRABEDEVELOPMENT_RESOURCETYPES_H
