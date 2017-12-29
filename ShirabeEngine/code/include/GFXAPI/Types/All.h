#include "ShaderResourceView.h"
#include "RenderTargetView.h"
#include "DepthStencilView.h"
#include "TextureND.h"
#include "DepthStencilState.h"
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
  SwapChain,          \
  SwapChainBuffer
  // ConstantBuffer,     \
  // ObjectBuffer,       \
  // StructuredBuffer,   \
  // InstanceBuffer,     \
  // Shader,             \

  }
}