#ifndef __SHIRABE_FRAMEGRAPH_FRAMEGRAPHDATA_H__
#define __SHIRABE_FRAMEGRAPH_FRAMEGRAPHDATA_H__

#include <stdint.h>

namespace Engine {
  namespace FrameGraph {

    using FrameGraphResourceId_t = uint64_t;

    enum class FrameGraphResourceType 
      : uint8_t
    {
      Undefined = 0,
      Texture1D = 1,
      Texture2D = 2,
      Texture3D = 4,
      ConstantBuffer = 8,
      TexureBuffer = 16,
      StructuredBuffer = 32
    };

    enum class FrameGraphResourceUsage
      : uint8_t 
    {
      Undefined = 0,
      ShaderResource = 1,
      RenderTarget = 2,
      DepthTarget = 4
    };

    enum class FrameGraphResourceAccessibility
      : uint8_t
    {
      Immutable = 1,
      Mutable = 2
    };

    struct FrameGraphResource {
      FrameGraphResourceId_t          const resourceId;
      FrameGraphResourceType          const type;
      FrameGraphResourceUsage         const usage;
      FrameGraphResourceAccessibility const accessibility;

      inline
        FrameGraphResource()
        : resourceId(FrameGraphResourceId_t())
        , type(FrameGraphResourceType::Undefined)
        , usage(FrameGraphResourceUsage::Undefined)
        , accessibility(FrameGraphResourceAccessibility::Immutable)
      {}

      inline
        FrameGraphResource(
          FrameGraphResourceId_t          const inResourceId,
          FrameGraphResourceType          const inType,
          FrameGraphResourceUsage         const inUsage,
          FrameGraphResourceAccessibility const inAccessibility)
        : resourceId(inResourceId)
        , type(inType)
        , usage(inUsage)
        , accessibility(inAccessibility)
      {}

      inline
        FrameGraphResource(
          FrameGraphResource const&other)
        : resourceId(other.resourceId)
        , type(other.type)
        , usage(other.usage)
        , accessibility(other.accessibility)
      {}

      inline 
        FrameGraphResource  
        operator=(FrameGraphResource const&other)
      {
        return FrameGraphResource(other);
      }

    };

    static
    bool
      operator<(FrameGraphResource const&l, FrameGraphResource const&r)
    {
      return (l.resourceId < r.resourceId);
    }

    static
    bool
      operator!=(FrameGraphResource const&l, FrameGraphResource const&r)
    {
      return (l.resourceId == r.resourceId);
    }
  }
}

#endif