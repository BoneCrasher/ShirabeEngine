#include "ShaderResourceView.h"
#include "RenderTargetView.h"
#include "DepthStencilView.h"
#include "DepthStencilState.h"
#include "TextureND.h"
#include "RasterizerState.h"
#include "ConstantBuffer.h"
#include "InstanceBuffer.h"
#include "ObjectBuffer.h"
#include "StructuredBuffer.h"
#include "Shader.h"
#include "SwapChain.h"

namespace Engine {
  namespace GFXAPI {

#define EngineTypes   \
  Texture1D,          \
  Texture2D,          \
  Texture3D,          \
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