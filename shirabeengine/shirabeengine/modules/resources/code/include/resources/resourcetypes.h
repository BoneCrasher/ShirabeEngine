//
// Created by dottideveloper on 20.10.19.
//

#ifndef SHIRABEDEVELOPMENT_RESOURCETYPES_H
#define SHIRABEDEVELOPMENT_RESOURCETYPES_H

#include <vector>
#include <vulkan/vulkan.h>
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
            : public CResourceObject<SBufferDescription>
        {
            using CResourceObject<SBufferDescription>::CResourceObject;
        };

        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SBufferView
            : public CResourceObject<SBufferViewDescription>
        {
            using CResourceObject<SBufferViewDescription>::CResourceObject;
        };

        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT STexture
            : public CResourceObject<STextureDescription>
        {
            using CResourceObject<STextureDescription>::CResourceObject;
        };

        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT STextureView
            : public CResourceObject<STextureViewDescription>
        {
            using CResourceObject<STextureViewDescription>::CResourceObject;
        };

        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SDepthStencilState
            : public CResourceObject<SDepthStencilStateDescription>
        {
            using CResourceObject<SDepthStencilStateDescription>::CResourceObject;
        };

        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SRasterizerState
            : public CResourceObject<SRasterizerStateDescription>
        {
            using CResourceObject<SRasterizerStateDescription>::CResourceObject;
        };

        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SSubpass
            : public CResourceObject<SSubpassDescription>
        {
            using CResourceObject<SSubpassDescription>::CResourceObject;
        };

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
            SHIRABE_LIBRARY_EXPORT SPipeline
            : public CResourceObject<SPipelineDescription>
        {
            using CResourceObject<SPipelineDescription>::CResourceObject;
        };
    }
}

#endif //SHIRABEDEVELOPMENT_RESOURCETYPES_H
