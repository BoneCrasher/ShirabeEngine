#ifndef __SHIRABE_FRAMEGRAPH_FRAMEGRAPHDATA_H__
#define __SHIRABE_FRAMEGRAPH_FRAMEGRAPHDATA_H__

#include <stdint.h>
#include <type_traits>
#include <map>
#include <vector>
#include <variant>

// #include <better-enums/enum.h>

#include "Platform/Platform.h"
#include "Core/BitField.h"
#include "Core/BasicTypes.h"

namespace Engine {
  namespace FrameGraph {
    using Core::BitField;
    using Engine::Range;

    using FrameGraphResourceId_t = uint64_t;
    using PassUID_t              = uint64_t;

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

    SHIRABE_TEST_EXPORT bool validateFormatCompatibility(FrameGraphFormat const&base, FrameGraphFormat const&derived);

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

    enum class FrameGraphResourceInitState
      : uint8_t
    {
      Undefined = 0,
      Clear     = 1  // Resource will be cleared depending on the resource type.
    };

    enum class FrameGraphViewAccessMode {
      Undefined = 0,
      Read,
      Write
    };

    SHIRABE_TEST_EXPORT std::ostream& operator<<(std::ostream &strm, FrameGraphFormat const&e);
    SHIRABE_TEST_EXPORT std::ostream& operator<<(std::ostream &strm, FrameGraphResourceType const&e);

    struct SHIRABE_TEST_EXPORT FrameGraphBuffer
    {
      std::string
        readableName;
      uint32_t
        elementSize,
        elementCount;

      FrameGraphBuffer();
    };

    struct SHIRABE_TEST_EXPORT FrameGraphBufferView {
      std::string
        readableName;
      Range
        subrange;
      FrameGraphFormat
        format;
      BitField<FrameGraphViewAccessMode>
        mode;

      FrameGraphBufferView();
    };

    struct SHIRABE_TEST_EXPORT FrameGraphTexture {
      std::string
        readableName;
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

    struct SHIRABE_TEST_EXPORT FrameGraphTextureView {
      std::string
        readableName;
      Range
        arraySliceRange,
        mipSliceRange;
      FrameGraphFormat
        format;
      BitField<FrameGraphViewAccessMode>
        mode;

      FrameGraphTextureView();
    };

    struct SHIRABE_TEST_EXPORT FrameGraphResourceFlags {
      FrameGraphFormat
        requiredFormat;

      FrameGraphResourceFlags();
    };

    struct SHIRABE_TEST_EXPORT FrameGraphReadTextureFlags
      : public FrameGraphResourceFlags
    {
    };

    struct SHIRABE_TEST_EXPORT FrameGraphWriteTextureFlags
      : public FrameGraphResourceFlags
    {
      FrameGraphWriteTarget
        writeTarget;
    };

    struct SHIRABE_TEST_EXPORT FrameGraphResourcePrivateData {
      FrameGraphResourceId_t
        parentResourceId;
      FrameGraphResourceType
        type;
      FrameGraphResourceFlags
        flags;
      BitField<FrameGraphResourceUsage>
        usage;
      std::vector<FrameGraphResourceId_t>
        resourceViews;

      FrameGraphResourcePrivateData();
      ~FrameGraphResourcePrivateData() = default;
    };
    DeclareMapType(FrameGraphResourceId_t, FrameGraphResourcePrivateData, FrameGraphResourcePrivateData);


    struct SHIRABE_TEST_EXPORT FrameGraphResource {
      FrameGraphResourceId_t
        resourceId;

      FrameGraphResource();

      inline operator FrameGraphResourceId_t() { return resourceId; }
    };

    SHIRABE_TEST_EXPORT bool operator<(FrameGraphResource const&l, FrameGraphResource const&r);
    SHIRABE_TEST_EXPORT bool operator!=(FrameGraphResource const&l, FrameGraphResource const&r);

    using FrameGraphResourceData = std::variant<FrameGraphTexture, FrameGraphTextureView, FrameGraphBuffer, FrameGraphBufferView>;
    DeclareMapType(FrameGraphResourceId_t, FrameGraphResourceData, FrameGraphResourceData);

    static bool isResourceRegistered(
      FrameGraphResourcePrivateDataMap const&privateRegistry,
      FrameGraphResource               const&subjacentTargetResource)
    {
      return (privateRegistry.find(subjacentTargetResource.resourceId) != privateRegistry.end());
    }

    static bool isResourceTexture(
      FrameGraphResourcePrivateDataMap const&privateRegistry,
      FrameGraphResource               const&resourceId)
    {
      return (isResourceRegistered(privateRegistry, resourceId) && privateRegistry.at(resourceId.resourceId).type == FrameGraphResourceType::Texture);
    }

    static bool isResourceTextureView(
      FrameGraphResourcePrivateDataMap const&privateRegistry,
      FrameGraphResource               const&resourceId)
    {
      return (isResourceRegistered(privateRegistry, resourceId) && privateRegistry.at(resourceId.resourceId).type == FrameGraphResourceType::TextureView);
    }

  }
}

#endif