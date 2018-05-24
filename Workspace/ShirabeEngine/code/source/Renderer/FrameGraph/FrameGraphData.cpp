#include "Renderer/FrameGraph/FrameGraphData.h"

#include <string.h>

namespace Engine {
  using namespace FrameGraph;

  template <>
  std::string to_string<FrameGraphResourceType>(FrameGraphResourceType const&type)
  {
    switch(type) {
    default:
    case FrameGraphResourceType::Undefined:   return "Undefined";
    case FrameGraphResourceType::Texture:     return "Texture";
    case FrameGraphResourceType::Buffer:      return "Buffer";
    case FrameGraphResourceType::TextureView: return "TextureView";
    case FrameGraphResourceType::BufferView:  return "BufferView";
    }
  }

  template <>
  std::string to_string<FrameGraphFormat>(FrameGraphFormat const&format)
  {
    switch(format) {
    default:
    case FrameGraphFormat::Undefined:                return "Undefined";
    case FrameGraphFormat::Automatic:                return "Automatic";
    case FrameGraphFormat::Structured:               return "Structured";
    case FrameGraphFormat::R8_TYPELESS:              return "R8_TYPELESS";
    case FrameGraphFormat::R8_SINT:                  return "R8_SINT";
    case FrameGraphFormat::R8_UINT:                  return "R8_UINT";
    case FrameGraphFormat::R8_SNORM:                 return "R8_SNORM";
    case FrameGraphFormat::R8_UNORM:                 return "R8_UNORM";
    case FrameGraphFormat::R8G8B8A8_TYPELESS:        return "R8G8B8A8_TYPELESS";
    case FrameGraphFormat::R8G8B8A8_SINT:            return "R8G8B8A8_SINT";
    case FrameGraphFormat::R8G8B8A8_UINT:            return "R8G8B8A8_UINT";
    case FrameGraphFormat::R8G8B8A8_SNORM:           return "R8G8B8A8_SNORM";
    case FrameGraphFormat::R8G8B8A8_UNORM:           return "R8G8B8A8_UNORM";
    case FrameGraphFormat::R8G8B8A8_UNORM_SRGB:      return "R8G8B8A8_UNORM_SRGB";
    case FrameGraphFormat::R8G8B8A8_FLOAT:           return "R8G8B8A8_FLOAT";
    case FrameGraphFormat::R16_TYPELESS:             return "R16_TYPELESS";
    case FrameGraphFormat::R16_SINT:                 return "R16_SINT";
    case FrameGraphFormat::R16_UINT:                 return "R16_UINT";
    case FrameGraphFormat::R16_SNORM:                return "R16_SNORM";
    case FrameGraphFormat::R16_UNORM:                return "R16_UNORM";
    case FrameGraphFormat::R16_FLOAT:                return "R16_FLOAT";
    case FrameGraphFormat::R16G16B16A16_TYPELESS:    return "R16G16B16A16_TYPELESS";
    case FrameGraphFormat::R16G16B16A16_SINT:        return "R16G16B16A16_SINT";
    case FrameGraphFormat::R16G16B16A16_UINT:        return "R16G16B16A16_UINT";
    case FrameGraphFormat::R16G16B16A16_SNORM:       return "R16G16B16A16_SNORM";
    case FrameGraphFormat::R16G16B16A16_UNORM:       return "R16G16B16A16_UNORM";
    case FrameGraphFormat::R16G16B16A16_FLOAT:       return "R16G16B16A16_FLOAT";
    case FrameGraphFormat::R24_UNORM_X8_TYPELESS:    return "R24_UNORM_X8_TYPELESS";
    case FrameGraphFormat::R32_TYPELESS:             return "R32_TYPELESS";
    case FrameGraphFormat::R32_SINT:                 return "R32_SINT";
    case FrameGraphFormat::R32_UINT:                 return "R32_UINT";
    case FrameGraphFormat::R32_SNORM:                return "R32_SNORM";
    case FrameGraphFormat::R32_UNORM:                return "R32_UNORM";
    case FrameGraphFormat::R32_FLOAT:                return "R32_FLOAT";
    case FrameGraphFormat::R32_FLOAT_S8X24_TYPELESS: return "R32_FLOAT_S8X24_TYPELESS";
    case FrameGraphFormat::R32G32B32A32_TYPELESS:    return "R32G32B32A32_TYPELESS";
    case FrameGraphFormat::R32G32B32A32_SINT:        return "R32G32B32A32_SINT";
    case FrameGraphFormat::R32G32B32A32_UINT:        return "R32G32B32A32_UINT";
    case FrameGraphFormat::D24_UNORM_S8_UINT:        return "D24_UNORM_S8_UINT";
    case FrameGraphFormat::D32_FLOAT:                return "D32_FLOAT";
    case FrameGraphFormat::D32_FLOAT_S8X24_UINT:     return "D32_FLOAT_S8X24_UINT";
    }
  }

  template <>
  std::string to_string<FrameGraph::FrameGraphResourceUsage>(FrameGraph::FrameGraphResourceUsage const&usage) {
    switch(usage) {
    default:
    case FrameGraphResourceUsage::Undefined:      return "Undefined";
    case FrameGraphResourceUsage::BufferResource: return "BufferResource";
    case FrameGraphResourceUsage::BufferTarget:   return "BufferTarget";
    case FrameGraphResourceUsage::ImageResource:  return "ImageResource";
    case FrameGraphResourceUsage::RenderTarget:   return "RenderTarget";
    case FrameGraphResourceUsage::DepthTarget:    return "DepthTarget";
    }
  }

  template <>
  std::string to_string<FrameGraph::FrameGraphWriteTarget>(FrameGraph::FrameGraphWriteTarget const&target)
  {
    return "";
  }

  template <>
  std::string to_string<FrameGraph::FrameGraphResourceAccessibility>(FrameGraph::FrameGraphResourceAccessibility const&accessibility) { return ""; }

  template <>
  std::string to_string<FrameGraph::FrameGraphResourceInitState>(FrameGraph::FrameGraphResourceInitState const&state)
  {
    switch(state) {
    default:
    case FrameGraphResourceInitState::Undefined: return "Undefined";
    case FrameGraphResourceInitState::Clear:     return "Clear";
    }
  }

  template <>
  std::string to_string<FrameGraph::FrameGraphViewAccessMode>(FrameGraph::FrameGraphViewAccessMode const&accessMode) { return ""; }

  namespace FrameGraph {
    std::ostream& operator<<(std::ostream &strm, FrameGraphResourceType const&e) {
      strm << to_string(e).c_str();
      return strm;
    }

    std::ostream& operator<<(std::ostream &strm, FrameGraphFormat const&e) {
      strm << to_string(e).c_str();
      return strm;
    }

    bool validateFormatCompatibility(
      FrameGraphFormat const&base,
      FrameGraphFormat const&derived)
    {
      return true;
    }

    FrameGraphResource::FrameGraphResource()
      : assignedPassUID(0)
      , resourceId(FrameGraphResourceId_t{ })
      , parentResource(0)
      , subjacentResource(0)
      , readableName("")
      , type(FrameGraphResourceType::Undefined)
      , isExternalResource(false)
    {}


    FrameGraphBuffer::FrameGraphBuffer()
      : FrameGraphResource()
      , elementSize(0)
      , elementCount(0)
    {}

    FrameGraphBufferView::FrameGraphBufferView()
      : FrameGraphResource()
      , subrange(Range(0, 0))
      , format(FrameGraphFormat::Undefined)
      , mode(FrameGraphViewAccessMode::Undefined)
    {}

    FrameGraphTexture::FrameGraphTexture()
      : FrameGraphResource()
      , TextureInfo()
      , initialState(FrameGraphResourceInitState::Undefined)
      , permittedUsage(FrameGraphResourceUsage::Undefined)
      , requestedUsage(FrameGraphResourceUsage::Undefined)
    {}


    void
      FrameGraphTexture::assignTextureParameters(FrameGraphTexture const&other)
    {
      this->assignTextureInfoParameters(other);

      initialState   = other.initialState;
      permittedUsage = other.permittedUsage;
      requestedUsage = other.requestedUsage;
    }

    bool
      FrameGraphTexture::validate() const
    {
      bool dimensionsValid = (width == 0 || !(width == 0 || height == 0 || depth == 0));
      bool mipLevelsValid  = (mipLevels >= 1);
      bool arraySizeValid  = (arraySize >= 1);

      return (dimensionsValid && mipLevelsValid && arraySizeValid);
    }

    FrameGraphTextureView::FrameGraphTextureView()
      : FrameGraphResource()
      , arraySliceRange(Range(0, 0))
      , mipSliceRange(Range(0, 0))
      , format(FrameGraphFormat::Undefined)
      , mode(FrameGraphViewAccessMode::Undefined)
    {}

    FrameGraphResourceFlags::FrameGraphResourceFlags()
      : requiredFormat(FrameGraphFormat::Undefined)
    {}

    bool
      operator<(FrameGraphResource const&l, FrameGraphResource const&r)
    {
      return (l.resourceId < r.resourceId);
    }

    bool
      operator!=(FrameGraphResource const&l, FrameGraphResource const&r)
    {
      return (l.resourceId == r.resourceId);
    }

    template <typename T>
    void appendToVector(std::vector<T> &inOutTarget, std::vector<T> const&inSource) {
      inOutTarget.resize(inOutTarget.size() + inSource.size());
      for(T const&s : inSource)
        inOutTarget.push_back(s);
    }
        
    bool FrameGraphMutableResources::mergeIn(FrameGraphResources const&other)
    {
      #if defined SHIRABE_DEBUG || defined SHIRABE_TEST 
      try {
        #endif

        appendToVector(m_resources, other.resources());

        for(RefIndex::value_type const&id : other.textures())
          FrameGraphResourcesRef<FrameGraphTexture>::insert(id);
        for(RefIndex::value_type const&id : other.textureViews())
          FrameGraphResourcesRef<FrameGraphTextureView>::insert(id);
        for(RefIndex::value_type const&id : other.buffers())
          FrameGraphResourcesRef<FrameGraphBuffer>::insert(id);
        for(RefIndex::value_type const&id : other.bufferViews())
          FrameGraphResourcesRef<FrameGraphBufferView>::insert(id);
        for(RefIndex::value_type const&id : other.renderablesLists())
          FrameGraphResourcesRef<FrameGraphRenderableList>::insert(id);
        for(RefIndex::value_type const&id : other.renderableListViews())
          FrameGraphResourcesRef<FrameGraphRenderableListView>::insert(id);

        return true;

        #if defined SHIRABE_DEBUG || defined SHIRABE_TEST
      }
      catch(std::runtime_error const&rte) {
        return false;
      }
      catch(...) {
        return false;
      }
      #endif
    }

  }

  template <>
  std::string to_string<FrameGraph::FrameGraphTexture>(FrameGraph::FrameGraphTexture const&texture)
  {
    std::string s =
      String::format(
        "  %0 (RID: %1)"
        "\n    Name:          %2"
        "\n    Sizes:         %3 x %4 x %5"
        "\n    Format:        %6"
        "\n    Array-Levels:  %7"
        "\n    Mip-Levels:    %8"
        "\n    Initial-State: %9",
        "Texture",
        texture.resourceId,
        texture.readableName,
        texture.width, texture.height, texture.depth,
        to_string(texture.format),
        texture.arraySize,
        texture.mipLevels,
        to_string(texture.initialState));
    return s;
  }

  template <>
  std::string to_string<FrameGraph::FrameGraphTextureView>(FrameGraph::FrameGraphTextureView const&view)
  {
    bool viewIsReadMode  = view.mode.check(FrameGraphViewAccessMode::Read);

    std::string s =
      String::format(
        "  %0 (RID: %1)"
        "\n    SubjacentResourceId: %2"
        "\n    Mode:                %3"
        "\n    Format:              %4"
        "\n    ArrayRange:          %5"
        "\n    MipRange:            %6",
        "TextureView",
        view.resourceId,
        view.subjacentResource,
        (viewIsReadMode ? "Read" : "Write"),
        to_string(view.format),
        to_string(view.arraySliceRange),
        to_string(view.mipSliceRange));
    return s;
  }

  template <>
  std::string to_string<FrameGraph::FrameGraphBuffer>(FrameGraph::FrameGraphBuffer const&buffer)
  {
    std::string s =
      String::format(
        "  %0 (RID: %1)",
        "Buffer",
        buffer.resourceId);
    return s;
  }

  template <>
  std::string to_string<FrameGraph::FrameGraphBufferView>(FrameGraph::FrameGraphBufferView const&view)
  {
    std::string s =
      String::format(
        "  %0 (RID: %1)",
        "BufferView",
        view.resourceId);
    return s;
  }

}