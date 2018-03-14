#ifndef __SHIRABE_FRAMEGRAPH_FRAMEGRAPHDATA_H__
#define __SHIRABE_FRAMEGRAPH_FRAMEGRAPHDATA_H__

#include <stdint.h>

namespace Engine {
  namespace FrameGraph {

    using FrameGraphResourceId_t = uint64_t;

    struct FrameGraphResource {
      FrameGraphResourceId_t resourceId;
    };

    struct FrameGraphShaderResource
      : public FrameGraphResource
    {};

    struct FrameGraphRenderTarget
      : public FrameGraphResource
    {};

    struct FrameGraphTexture
      : public FrameGraphResource
    {};

    template <uint64_t N>
    struct FrameGraphTextureArray
      : public FrameGraphResource
    {
      FrameGraphTexture textures[N];
    };

  }
}

#endif