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
      R8_TYPELESS,
      R8_SINT,
      R8_UINT,
      R8_SNORM,
      R8_UNORM,
      R8G8B8A8_TYPELESS,
      R8G8B8A8_SINT,
      R8G8B8A8_UINT,
      R8G8B8A8_SNORM,
      R8G8B8A8_UNORM,
      R8G8B8A8_UNORM_SRGB,
      R8G8B8A8_FLOAT,
      R16_TYPELESS,
      R16_SINT,
      R16_UINT,
      R16_SNORM,
      R16_UNORM,
      R16_FLOAT,
      R16G16B16A16_TYPELESS,
      R16G16B16A16_SINT,
      R16G16B16A16_UINT,
      R16G16B16A16_SNORM,
      R16G16B16A16_UNORM,
      R16G16B16A16_FLOAT,
      R24_UNORM_X8_TYPELESS,
      R32_TYPELESS,
      R32_SINT,
      R32_UINT,
      R32_SNORM,
      R32_UNORM,
      R32_FLOAT,
      R32_FLOAT_S8X24_TYPELESS,
      R32G32B32A32_TYPELESS,
      R32G32B32A32_SINT,
      R32G32B32A32_UINT,
      D24_UNORM_S8_UINT,
      D32_FLOAT,
      D32_FLOAT_S8X24_UINT,
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


    struct FrameGraphResourceIndex {
      Range
        arraySliceRange,
        mipSliceRange;

      inline
        FrameGraphResourceIndex(
          Range const&inArraySliceRange,
          Range const&inMipSliceRange)
        : arraySliceRange(inArraySliceRange)
        , mipSliceRange(inMipSliceRange)
      {}

    };

    inline static
      bool
      operator<(
        FrameGraphResourceIndex const&l,
        FrameGraphResourceIndex const&r)
    {
      if(l.arraySliceRange.offset == r.arraySliceRange.offset)
        if(l.arraySliceRange.length == r.arraySliceRange.length)
          if(l.mipSliceRange.offset == r.mipSliceRange.offset)
            if(l.mipSliceRange.length == r.mipSliceRange.length)
              return true;
            else
              return (l.mipSliceRange.length < r.mipSliceRange.length);
          else
            return (l.mipSliceRange.offset < r.mipSliceRange.offset);
        else
          return (l.arraySliceRange.length < r.arraySliceRange.length);
      else
        return (l.arraySliceRange.offset < r.arraySliceRange.offset);
    }

    inline static
      bool operator!=(
        FrameGraphResourceIndex const&l,
        FrameGraphResourceIndex const&r)
    {
      return !(
        l.arraySliceRange.offset == r.arraySliceRange.offset &&
        l.arraySliceRange.length == r.arraySliceRange.length &&
        l.mipSliceRange.offset   == r.mipSliceRange.offset   &&
        l.mipSliceRange.length   == r.mipSliceRange.length);
    }

    struct FrameGraphUsageInfo {
      enum class Type {
        None   = 0,
        Create = 1,
        Read   = 2,
        Write  = 4
      };

      // Store all created resources in a plain list to allow their descriptors be accessed where necessary!
      FrameGraphResourceId_t
        resourceId;
      // UsageInfo-global flat to determine different operations performed on this resource for a given pass.
      BitField<Type>
        usageTypes; 
      // UsageInfo-per-subresource information on operations.
      std::map<FrameGraphResourceIndex, Type>
        resourceAccessors;
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

      inline
        FrameGraphTexture()
        : width(0)
        , height(1)
        , depth(1)
        , format(FrameGraphFormat::Undefined)
        , mipLevels(1)
        , arraySize(1)
        , initialState(FrameGraphResourceInitState::Undefined)
      {}

      virtual inline
        bool
        validate() const
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