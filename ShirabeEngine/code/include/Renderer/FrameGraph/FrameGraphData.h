#ifndef __SHIRABE_FRAMEGRAPH_FRAMEGRAPHDATA_H__
#define __SHIRABE_FRAMEGRAPH_FRAMEGRAPHDATA_H__

#include <stdint.h>
#include <type_traits>
#include <map>
#include <vector>
#include <variant>

#include "Core/BitField.h"

namespace Engine {
  namespace FrameGraph {
    using Core::BitField;

    using FrameGraphResourceId_t = uint64_t;

    /**********************************************************************************************//**
     * \enum  FrameGraphResourceType
     *
     * \brief Values that represent top-level frame graph resource types
     **************************************************************************************************/
    enum class FrameGraphResourceType
      : uint8_t
    {
      Undefined = 0,
      Texture,
      Buffer,
      TextureView,
      BufferView
    };

    /**********************************************************************************************//**
     * \enum  FrameGraphFormat
     *
     * \brief Values that represent texture and buffer formats
     **************************************************************************************************/
    enum class FrameGraphFormat
      : uint8_t
    {
      Undefined = 0,
      Automatic,
      Structured,
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
      D32_FLOAT_S8X24_UINT
      // TODO: DXT/BC Compression and Video formats
    };

    enum class FrameGraphResourceUsage
      : uint8_t
    {
      Undefined      =  1,
      ImageResource  =  2,
      BufferResource =  4,
      RenderTarget   =  8,
      DepthTarget    = 16,
      BufferTarget   = 32
    };

    enum class FrameGraphWriteTarget 
      : uint8_t 
    {
      Undefined = 0,
      Color,
      Depth
    };

    enum class FrameGraphResourceAccessibility
      : uint8_t
    {
      None      = 0,
      Immutable = 1,
      Mutable   = 2
    };

    enum class FrameGraphResourceInitState {
      Undefined = 0,
      Clear     = 1  // Resource will be cleared depending on the resource type.
    };

    struct FrameGraphBuffer
    {
      uint32_t
        elementSize,
        elementCount;
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

      FrameGraphTexture();

      virtual bool validate() const;
    };

    struct FrameGraphResourceFlags {
      FrameGraphResourceFlags();

      FrameGraphFormat
        requiredFormat;
    };

    struct FrameGraphReadTextureFlags
      : public FrameGraphResourceFlags
    {
    };

    struct FrameGraphWriteTextureFlags
      : public FrameGraphResourceFlags
    {
      FrameGraphWriteTarget
        writeTarget;
    };

    struct FrameGraphResourcePrivateData {
      FrameGraphResourcePrivateData();

      BitField<FrameGraphResourceUsage>
        usage;
      std::vector<FrameGraphResourceId_t>
        resourceViews;
    };

    struct FrameGraphResource {
      FrameGraphResource();

      FrameGraphResourceId_t
        resourceId,
        parentResourceId;
      FrameGraphResourceType
        type;
      FrameGraphResourceFlags
        flags;
      std::variant<FrameGraphTexture, FrameGraphBuffer>
        descriptor; 

    public:
      FrameGraphResource();
    };

    static bool operator<(FrameGraphResource const&l, FrameGraphResource const&r);
    static bool operator!=(FrameGraphResource const&l, FrameGraphResource const&r);
  }
}

#endif