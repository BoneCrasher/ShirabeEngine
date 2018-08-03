#include "TextureView.h"
#include "DepthStencilState.h"
#include "Texture.h"
#include "RasterizerState.h"
#include "ConstantBuffer.h"
#include "InstanceBuffer.h"
#include "ObjectBuffer.h"
#include "StructuredBuffer.h"
#include "Shader.h"
#include "SwapChain.h"

namespace engine {
  namespace GFXAPI {

    #define EngineTypes   \
  Texture,            \
  DepthStencilView,   \
  RenderTargetView,   \
  ShaderResourceView, \
  DepthStencilState,  \
  RasterizerState,    \
  SwapChain,          \
  SwapChainBuffer
    // ConstantBuffer,     \
    // ObjectBuffer,       \
    // StructuredBuffer,   \
    // InstanceBuffer,     \
    // Shader,             \

  }
}