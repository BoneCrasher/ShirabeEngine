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

#include "rhi/resource_management/resourcedescriptions.h"
#include "rhi/resource_management/cresourceobject.h"
#include "rhi/resource_management/resourcedatasource.h"

namespace engine
{
    namespace rhi
    {
        using namespace graphicsapi;

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SRHIMemory
            : public CRHIResourceObject<SRHIMemoryDescription>
        {
            using CRHIResourceObject<SRHIMemoryDescription>::CRHIResourceObject;
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SRHIBuffer
            : public CRHIResourceObject<SRHIBufferDescription>
        {
            using CRHIResourceObject<SRHIBufferDescription>::CRHIResourceObject;
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SRHIBufferView
            : public CRHIResourceObject<SRHIBufferViewDescription>
        {
            using CRHIResourceObject<SRHIBufferViewDescription>::CRHIResourceObject;
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SRHIImage
            : public CRHIResourceObject<SRHIImageDescription>
        {
            using CRHIResourceObject<SRHIImageDescription>::CRHIResourceObject;
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SRHIImageView
            : public CRHIResourceObject<SRHIImageViewDescription>
        {
            using CRHIResourceObject<SRHIImageViewDescription>::CRHIResourceObject;
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SDepthStencilState
            : public CRHIResourceObject<SRHIDepthStencilStateDescription>
        {
            using CRHIResourceObject<SRHIDepthStencilStateDescription>::CRHIResourceObject;
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SRasterizerState
            : public CRHIResourceObject<SRHIRasterizerStateDescription>
        {
            using CRHIResourceObject<SRHIRasterizerStateDescription>::CRHIResourceObject;
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SRHISubpass
            : public CRHIResourceObject<SRHISubpassDescription>
        {
            using CRHIResourceObject<SRHISubpassDescription>::CRHIResourceObject;
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SRHIRenderPass
            : public CRHIResourceObject<SRHIRenderPassDescription>
        {
            using CRHIResourceObject<SRHIRenderPassDescription>::CRHIResourceObject;
        };

        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SRHIFrameBuffer
            : public CRHIResourceObject<SRHIFrameBufferDescription>
        {
            using CRHIResourceObject<SRHIFrameBufferDescription>::CRHIResourceObject;
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SRHISwapChainBuffer
            : public CRHIResourceObject<SRHISwapChainBufferDescription>
        {
            using CRHIResourceObject<SRHISwapChainBufferDescription>::CRHIResourceObject;
        };

        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SSwapChain
            : public CRHIResourceObject<SRHISwapChainDescription>
        {
            using CRHIResourceObject<SRHISwapChainDescription>::CRHIResourceObject;
        };

        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SRHIDescriptorPool
            : public CRHIResourceObject<SRHIDescriptorPoolDescription>
        {
            using CRHIResourceObject<SRHIDescriptorPoolDescription>::CRHIResourceObject;
        };

        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SRHIPipelineLayout
            : public CRHIResourceObject<SRHIPipelineLayoutDescriptor>
        {
            using CRHIResourceObject<SRHIPipelineLayoutDescriptor>::CRHIResourceObject;
        };

        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SRHIPipeline
            : public CRHIResourceObject<SRHIPipelineDescriptor>
        {
            using CRHIResourceObject<SRHIPipelineDescriptor>::CRHIResourceObject;
        };

        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SRHIShaderModule
            : public CRHIResourceObject<SRHIShaderModuleDescriptor>
        {
            using CRHIResourceObject<SRHIShaderModuleDescriptor>::CRHIResourceObject;
        };

        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SMaterial
            : public CRHIResourceObject<SRHIDescriptorPoolDescription>
        {
            using CRHIResourceObject<SRHIDescriptorPoolDescription>::CRHIResourceObject;

            Shared<SRHIPipeline>       pipelineResource;
            Shared<SRHIShaderModule>   shaderModuleResource;
            Vector<Shared<SRHIBuffer>> bufferResources;
        };

        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SMesh
            : public CRHIResourceObject<SRHIMeshDescriptor>
        {
            using CRHIResourceObject<SRHIMeshDescriptor>::CRHIResourceObject;

            Shared<SRHIBuffer> vertexDataBufferResource;
            Shared<SRHIBuffer> indexBufferResource;
        };
    }
}

#endif //SHIRABEDEVELOPMENT_RESOURCETYPES_H
