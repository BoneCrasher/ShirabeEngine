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

#include "renderer/resource_management/resourcedescriptions.h"
#include "renderer/resource_management/cresourceobject.h"
#include "renderer/resource_management/resourcedatasource.h"

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
            SHIRABE_LIBRARY_EXPORT SMemory
            : public CResourceObject<SMemoryDescription>
        {
            using CResourceObject<SMemoryDescription>::CResourceObject;
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SBuffer
            : public CResourceObject<SBufferDescription>
        {
            using CResourceObject<SBufferDescription>::CResourceObject;
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SBufferView
            : public CResourceObject<SBufferViewDescription>
        {
            using CResourceObject<SBufferViewDescription>::CResourceObject;
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT STexture
            : public CResourceObject<STextureDescription>
        {
            using CResourceObject<STextureDescription>::CResourceObject;
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT STextureView
            : public CResourceObject<STextureViewDescription>
        {
            using CResourceObject<STextureViewDescription>::CResourceObject;
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SDepthStencilState
            : public CResourceObject<SDepthStencilStateDescription>
        {
            using CResourceObject<SDepthStencilStateDescription>::CResourceObject;
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SRasterizerState
            : public CResourceObject<SRasterizerStateDescription>
        {
            using CResourceObject<SRasterizerStateDescription>::CResourceObject;
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SSubpass
            : public CResourceObject<SSubpassDescription>
        {
            using CResourceObject<SSubpassDescription>::CResourceObject;
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SRenderPass
            : public CResourceObject<SRenderPassDescription>
        {
            using CResourceObject<SRenderPassDescription>::CResourceObject;
        };

        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SFrameBuffer
            : public CResourceObject<SFrameBufferDescription>
        {
            using CResourceObject<SFrameBufferDescription>::CResourceObject;

        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SSwapChainBuffer
            : public CResourceObject<SSwapChainBufferDescription>
        {
            using CResourceObject<SSwapChainBufferDescription>::CResourceObject;
        };

        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SSwapChain
            : public CResourceObject<SSwapChainDescription>
        {
            using CResourceObject<SSwapChainDescription>::CResourceObject;
        };

        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SBasePipeline
            : public CResourceObject<SMaterialBasePipelineDescriptor>
        {
            using CResourceObject<SMaterialBasePipelineDescriptor>::CResourceObject;
        };

        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SPipeline
            : public CResourceObject<SMaterialPipelineDescriptor>
        {
            using CResourceObject<SMaterialPipelineDescriptor>::CResourceObject;
        };

        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SShaderModule
            : public CResourceObject<SShaderModuleDescriptor>
        {
            using CResourceObject<SShaderModuleDescriptor>::CResourceObject;
        };

        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SMaterial
            : public CResourceObject<SMaterialDescription>
        {
            using CResourceObject<SMaterialDescription>::CResourceObject;

            Shared<SPipeline>       pipelineResource;
            Shared<SShaderModule>   shaderModuleResource;
            Vector<Shared<SBuffer>> bufferResources;
        };

        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SMesh
            : public CResourceObject<SMeshDescriptor>
        {
            using CResourceObject<SMeshDescriptor>::CResourceObject;

            Shared<SBuffer> vertexDataBufferResource;
            Shared<SBuffer> indexBufferResource;
        };
    }
}

#endif //SHIRABEDEVELOPMENT_RESOURCETYPES_H
