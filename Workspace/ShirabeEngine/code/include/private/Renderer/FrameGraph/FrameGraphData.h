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

#include "Renderer/IRenderer.h"

namespace Engine {
  namespace FrameGraph {
    using Core::BitField;
    using Engine::Range;

    using FrameGraphResourceId_t = uint64_t;
    using PassUID_t              = uint64_t;

    DeclareListType(FrameGraphResourceId_t, FrameGraphResourceId);

    /**********************************************************************************************//**
     * \enum  FrameGraphResourceType
     *
     * \brief Values that represent top-level frame graph resource types
     **************************************************************************************************/

    enum class FrameGraphResourceType
      : uint8_t
    {
      Undefined = 0,
      Imported  = 1,
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
      SpecialFormatRangeFlag      = 1,
      Automatic,
      Structured,
      // 8-bit formats
      Format8BitFormatRangeFlag   = 8,
      R8_TYPELESS,
      R8_SINT,
      R8_UINT,
      R8_SNORM,
      R8_UNORM,
      // 16-bit formats
      Format16BitFormatRangeFlag  = 16,
      R16_TYPELESS,
      R16_SINT,
      R16_UINT,
      R16_SNORM,
      R16_UNORM,
      R16_FLOAT,
      // 32-bit formats
      Format32BitFormatRangeFlag  = 32,
      R8G8B8A8_TYPELESS,
      R8G8B8A8_SINT,
      R8G8B8A8_UINT,
      R8G8B8A8_SNORM,
      R8G8B8A8_UNORM,
      R8G8B8A8_UNORM_SRGB,
      R8G8B8A8_FLOAT,
      R24_UNORM_X8_TYPELESS,
      R32_TYPELESS,
      R32_SINT,
      R32_UINT,
      R32_SNORM,
      R32_UNORM,
      R32_FLOAT,
      D24_UNORM_S8_UINT,
      D32_FLOAT,
      // 64-bit formats
      Format64BitFormatRangeFlag  = 64,
      R16G16B16A16_TYPELESS = 64,
      R16G16B16A16_SINT,
      R16G16B16A16_UINT,
      R16G16B16A16_SNORM,
      R16G16B16A16_UNORM,
      R16G16B16A16_FLOAT,
      R32_FLOAT_S8X24_TYPELESS,
      D32_FLOAT_S8X24_UINT,
      // 128-bit formats
      Format128BitFormatRangeFlag = 128,
      R32G32B32A32_TYPELESS,
      R32G32B32A32_SINT,
      R32G32B32A32_UINT,
      Format256BitFormatRangeFlag = 256,
      // TODO: DXT/BC Compression and Video formats
    };

    SHIRABE_TEST_EXPORT bool validateFormatCompatibility(FrameGraphFormat const&base, FrameGraphFormat const&derived);

    enum class FrameGraphResourceUsage
      : uint8_t
    {
      Undefined      =  0,
      ImageResource  =  1,
      BufferResource =  2,
      RenderTarget   =  4,
      DepthTarget    =  8,
      BufferTarget   = 16
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
      uint32_t
        elementSize,
        elementCount;

      FrameGraphBuffer();
    };

    struct SHIRABE_TEST_EXPORT FrameGraphBufferView {
      Range
        subrange;
      FrameGraphFormat
        format;
      BitField<FrameGraphViewAccessMode>
        mode;

      FrameGraphBufferView();
    };

    struct SHIRABE_TEST_EXPORT FrameGraphTexture {
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
      BitField<FrameGraphResourceUsage>
        permittedUsage,
        requestedUsage;

      FrameGraphTexture();

      virtual bool validate() const;
    };

    struct SHIRABE_TEST_EXPORT FrameGraphTextureView {
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

    using FrameGraphResourceData = std::variant<FrameGraphTexture, FrameGraphTextureView, FrameGraphBuffer, FrameGraphBufferView>;

    struct SHIRABE_TEST_EXPORT FrameGraphResource {
      PassUID_t
        assignedPassUID;
      FrameGraphResourceId_t
        resourceId;
      FrameGraphResourceId_t
        parentResource;
      FrameGraphResourceId_t
        subjacentResource;
      std::string
        readableName;
      FrameGraphResourceType
        type;
      FrameGraphResourceData
        data;
      bool
        isExternalResource;

      FrameGraphResource();

      inline operator FrameGraphResourceId_t() { return resourceId; }
    };

    SHIRABE_TEST_EXPORT bool operator<(FrameGraphResource const&l, FrameGraphResource const&r);
    SHIRABE_TEST_EXPORT bool operator!=(FrameGraphResource const&l, FrameGraphResource const&r);

    DeclareMapType(FrameGraphResourceId_t, FrameGraphResource, FrameGraphResource);

    static bool isResourceRegistered(
      FrameGraphResourceMap const&registry,
      FrameGraphResource    const&subjacentTargetResource)
    {
      return (registry.find(subjacentTargetResource.resourceId) != registry.end());
    }

    static bool isResourceTexture(
      FrameGraphResourceMap const&registry,
      FrameGraphResource        const&resourceId)
    {
      return (isResourceRegistered(registry, resourceId) && registry.at(resourceId.resourceId).type == FrameGraphResourceType::Texture);
    }

    static bool isResourceTextureView(
      FrameGraphResourceMap const&registry,
      FrameGraphResource    const&resourceId)
    {
      return (isResourceRegistered(registry, resourceId) && registry.at(resourceId.resourceId).type == FrameGraphResourceType::TextureView);
    }

    template <typename TUnderlyingIDFrom, typename TUnderlyingIDTo = TUnderlyingIDFrom>
    using AdjacencyListMap = std::map<TUnderlyingIDFrom, std::vector<TUnderlyingIDTo>>;

    class FrameGraphResources {
    public:
      FrameGraphTexture       const&getTexture(FrameGraphResource const&)     const;
      FrameGraphTextureView   const&getTextureView(FrameGraphResource const&) const;
      FrameGraphBuffer        const&getBuffer(FrameGraphResource const&)      const;
      FrameGraphBuffer        const&getBufferView(FrameGraphResource const&)  const;
      std::vector<Renderable> const&getRenderables(FrameGraphResource const&) const;

    private:
      FrameGraphResourceMap m_resources;
    };
  }

  template <>
  std::string to_string<FrameGraph::FrameGraphResourceType>(FrameGraph::FrameGraphResourceType const&type);
  template <>
  std::string to_string<FrameGraph::FrameGraphFormat>(FrameGraph::FrameGraphFormat const&format);
  template <>
  std::string to_string<FrameGraph::FrameGraphResourceUsage>(FrameGraph::FrameGraphResourceUsage const&usage);
  template <>
  std::string to_string<FrameGraph::FrameGraphWriteTarget>(FrameGraph::FrameGraphWriteTarget const&target);
  template <>
  std::string to_string<FrameGraph::FrameGraphResourceAccessibility>(FrameGraph::FrameGraphResourceAccessibility const&accessibility);
  template <>
  std::string to_string<FrameGraph::FrameGraphResourceInitState>(FrameGraph::FrameGraphResourceInitState const&state);
  template <>
  std::string to_string<FrameGraph::FrameGraphViewAccessMode>(FrameGraph::FrameGraphViewAccessMode const&accessMode);

}

#endif