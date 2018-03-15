#ifndef __SHIRABE_FRAMEGRAPH_FRAMEGRAPHDATA_H__
#define __SHIRABE_FRAMEGRAPH_FRAMEGRAPHDATA_H__

#include <stdint.h>
#include <type_traits>

#include "Core/BitField.h"

namespace Engine {
  namespace FrameGraph {
    using Core::BitField;

    using FrameGraphResourceId_t = uint64_t;

    enum class FrameGraphResourceType
      : uint8_t
    {
      Undefined        =   0,
      Texture1D        =   1,
      Texture2D        =   2,
      Texture3D        =   4,
      Texture1DArray   =   8,
      Texture2DArray   =  16,
      Texture3DArray   =  32,
      ConstantBuffer   =  64,
      TexureBuffer     = 128,
      StructuredBuffer = 256
    };

    enum class FrameGraphFormat {
      Undefined      = 0,
      RGBA8_TYPELESS = 1,
      RGBA8_SINT,
      RGBA8_UINT,
      RGBA8_SNORM,
      RGBA8_UNORM,
      RGBA8_UNORM_SRGB,
      RGBA8_FLOAT,
      RGBA16_TYPELESS,
      RGBA16_SINT,
      RGBA16_UINT,
      RGBA16_SNORM,
      RGBA16_UNORM,
      RGBA16_FLOAT,
      RGBA32_TYPELESS,
      RGBA32_SINT,
      RGBA32_UINT,
      D24_UNORM_S8_UINT,
      D32_FLOAT,
      D32_FLOAT_S8X24_UINT,
      R24_UNORM_X8_TYPELESS,
      R32_FLOAT,
      R32_FLOAT_S8X24_TYPELESS
      // TODO: DXT Compression and Video formats
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
      None = 0,
      Immutable = 1,
      Mutable = 2
    };

    enum class FrameGraphResourceInitState {
      Undefined = 0,
      Clear     = 1  // Resource will be cleared depending on the resource type.
    };

    struct FrameGraphResource {
      FrameGraphResourceId_t            const resourceId;
      FrameGraphResourceType            const type;
      BitField<FrameGraphResourceUsage> const usage;
      FrameGraphResourceAccessibility   const cpuAccessibility;
      FrameGraphResourceAccessibility   const gpuAccessibility;

      inline
        FrameGraphResource()
        : resourceId(FrameGraphResourceId_t())
        , type(FrameGraphResourceType::Undefined)
        , usage(FrameGraphResourceUsage::Undefined)
        , cpuAccessibility(FrameGraphResourceAccessibility::None)
        , gpuAccessibility(FrameGraphResourceAccessibility::None)
      {}

      inline
        FrameGraphResource(
          FrameGraphResourceId_t          const inResourceId,
          FrameGraphResourceType          const inType,
          FrameGraphResourceUsage         const inUsage,
          FrameGraphResourceAccessibility const inCPUAccessibility,
          FrameGraphResourceAccessibility const inGPUAccessibility)
        : resourceId(inResourceId)
        , type(inType)
        , usage(inUsage)
        , cpuAccessibility(inCPUAccessibility)
        , gpuAccessibility(inGPUAccessibility)
      {}

      inline
        FrameGraphResource(
          FrameGraphResource const&other)
        : resourceId(other.resourceId)
        , type(other.type)
        , usage(other.usage)
        , cpuAccessibility(other.cpuAccessibility)
        , gpuAccessibility(other.gpuAccessibility)
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

    struct FrameGraphUsage {
      enum class Type {
        Create = 1,
        Read   = 2,
        Write  = 3
      };


      asdfj ks; lf asklf j;l:==> Store all created resources in a plain list to allow their descriptors be accessed where necessary!
      FrameGraphResourceId_t 
        resourceId;
      BitField<Type>
        types;
    };

    struct FrameGraphTexture {
      uint32_t
        width,  // 0 - Undefined
        height, // At least 1
        depth;  // At least 1
      FrameGraphFormat
        format;
      uint16_t
        mipLevels; // At least 1 (most detailed MIP)
      uint16_t
        arraySize; // At least 1 (basically everything is a vector...)
      FrameGraphResourceInitState
        initialState;
      bool
        enableCPURead,  // Can the CPU read the data currently stored on GPU? (Staging-Buffer)
        enableCPUWrite, // Can the CPU write to the buffer dynamically? (Staging- or Dynamic-Buffer)
        enableGPURead,  // Can the GPU read from the resource? If not, CPU only resource not synced with the GPU.
        enableGPUWrite; // Can the GPU write (Default- or Staging-Buffer)

      inline
        FrameGraphTexture()
        : width(0)    
        , height(1)   
        , depth(1)    
        , format(FrameGraphFormat::Undefined)
        , mipLevels(1)
        , arraySize(1)
        , initialState(FrameGraphResourceInitState::Undefined)
        , enableCPURead(false)
        , enableCPUWrite(false)
        , enableGPURead(true)
        , enableGPUWrite(false)
      {}

      virtual inline
      bool 
        validate()
      {
        bool dimensionsValid = (width == 0 || !(width == 0 || height == 0 || depth == 0));
        bool mipLevelsValid  = (mipLevels >= 1);
        bool arraySizeValid  = (arraySize >= 1);

        return (dimensionsValid && mipLevelsValid && arraySizeValid);
      }
    };
  }
}

#endif