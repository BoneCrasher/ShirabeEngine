/*!
 * @file      all.h
 * @author    Marc-Anton Boehm-von Thenen
 * @date      10/08/2018
 * @copyright 2017-2018 >Craft|Deploy_
 */

#include "textureview.h"
#include "depthstencilstate.h"
#include "texture.h"
#include "rasterizerstate.h"
#include "constantbuffer.h"
#include "instancebuffer.h"
#include "objectbuffer.h"
#include "structuredbuffer.h"
#include "shader.h"
#include "swapchain.h"
#include "renderpass.h"
#include "framebuffer.h"
#include "pipeline.h"

namespace engine
{
    namespace gfxapi
    {

        #define SHIRABE_ENGINE_RESOURCE_TYPES     \
            Texture,                              \
            DepthStencilView,                     \
            RenderTargetView,                     \
            ShaderResourceView,                   \
            DepthStencilState,                    \
            RasterizerState,                      \
            SwapChain,                            \
            SwapChainBuffer,                      \
            FrameBuffer,                          \
            RenderPass
                // ConstantBuffer,     \
                // ObjectBuffer,       \
                // StructuredBuffer,   \
                // InstanceBuffer,     \
                // Shader,             \

    }
}
