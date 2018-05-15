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

    FrameGraphBuffer::FrameGraphBuffer()
      : elementSize(0)
      , elementCount(0)
    {}

    FrameGraphBufferView::FrameGraphBufferView()
      : subrange(Range(0, 0))
      , format(FrameGraphFormat::Undefined)
      , mode(FrameGraphViewAccessMode::Undefined)
    {}

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
    FrameGraphTexture::FrameGraphTexture()
      : width(0)
      , height(1)
      , depth(1)
      , format(FrameGraphFormat::Undefined)
      , mipLevels(1)
      , arraySize(1)
      , initialState(FrameGraphResourceInitState::Undefined)
      , permittedUsage(FrameGraphResourceUsage::Undefined)
      , requestedUsage(FrameGraphResourceUsage::Undefined)
    {}

    bool
      FrameGraphTexture::validate() const
    {
      bool dimensionsValid = (width == 0 || !(width == 0 || height == 0 || depth == 0));
      bool mipLevelsValid  = (mipLevels >= 1);
      bool arraySizeValid  = (arraySize >= 1);

      return (dimensionsValid && mipLevelsValid && arraySizeValid);
    }

    FrameGraphTextureView::FrameGraphTextureView()
      : arraySliceRange(Range(0, 0))
      , mipSliceRange(Range(0, 0))
      , format(FrameGraphFormat::Undefined)
      , mode(FrameGraphViewAccessMode::Undefined)
    {}

    FrameGraphResourceFlags::FrameGraphResourceFlags()
      : requiredFormat(FrameGraphFormat::Undefined)
    {}

    FrameGraphResource::FrameGraphResource()
      : assignedPassUID(0)
      , resourceId(FrameGraphResourceId_t{})
      , parentResource(0)
      , subjacentResource(0)
      , readableName("")
      , type(FrameGraphResourceType::Undefined)
      , isExternalResource(false)
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

    template <typename TKey, typename TValue>
    bool checkIfAdded(Map<TKey, TValue> const&map, TKey const&key) {
      return (map.find(key) != map.end());
    }

    template <typename TKey, typename TValue>
    bool addIfNotAdded(Map<TKey, TValue>&map, TKey const&key, TValue const&value) {
      #if defined SHIRABE_DEBUG || defined SHIRABE_TEST 

      if(checkIfAdded(map, key))
        return false;

      #endif

      map[key] = value;
      return true;
    }

    FrameGraphTexture const&
      FrameGraphResources::getTexture(FrameGraphResource const&resource) const
    {
      return m_textures.at(resource.resourceId);
    }

    FrameGraphTextureView const&
      FrameGraphResources::getTextureView(FrameGraphResource const&resource) const
    {
      return m_textureViews.at(resource.resourceId);
    }

    FrameGraphBuffer const&
      FrameGraphResources::getBuffer(FrameGraphResource const&resource) const
    {
      return m_buffers.at(resource.resourceId);
    }

    FrameGraphBufferView const&
      FrameGraphResources::getBufferView(FrameGraphResource const&resource) const
    {
      return m_bufferViews.at(resource.resourceId);
    }

    Renderer::RenderableList const&
      FrameGraphResources::getRenderables(FrameGraphResource const&resource) const
    {
      return m_renderables.at(resource.resourceId);
    }


    bool FrameGraphMutableResources::addTexture(
      FrameGraphResource const&resource,
      FrameGraphTexture  const&texture)
    {
      return addIfNotAdded(m_textures, resource.resourceId, texture);
    }

    bool FrameGraphMutableResources::addTextureView(
      FrameGraphResource    const&resource,
      FrameGraphTextureView const&view)
    {
      return addIfNotAdded(m_textureViews, resource.resourceId, view);
    }

    bool FrameGraphMutableResources::addBuffer(
      FrameGraphResource const&resource,
      FrameGraphBuffer   const&buffer)
    {
      return addIfNotAdded(m_buffers, resource.resourceId, buffer);
    }

    bool FrameGraphMutableResources::addBufferView(
      FrameGraphResource   const&resource,
      FrameGraphBufferView const&view)
    {
      return addIfNotAdded(m_bufferViews, resource.resourceId, view);
    }

    bool FrameGraphMutableResources::addRenderables(
      FrameGraphResource       const&resource,
      Renderer::RenderableList const&renderables)
    {
      return true;
    }

    FrameGraphTexture&
      FrameGraphMutableResources::getMutableTexture(FrameGraphResource const&resource)
    {
      return m_textures.at(resource.resourceId);
    }

    FrameGraphTextureView&
      FrameGraphMutableResources::getMutableTextureView(FrameGraphResource const&resource)
    {
      return m_textureViews.at(resource.resourceId);
    }

    FrameGraphBuffer&
      FrameGraphMutableResources::getMutableBuffer(FrameGraphResource const&resource)
    {
      return m_buffers.at(resource.resourceId);
    }

    FrameGraphBufferView&
      FrameGraphMutableResources::getMutableBufferView(FrameGraphResource const&resource)
    {
      return m_bufferViews.at(resource.resourceId);
    }

    bool FrameGraphMutableResources::mergeIn(FrameGraphResources const&other)
    {
      #if defined SHIRABE_DEBUG || defined SHIRABE_TEST 
      try {
        #endif

        m_textures.insert(other.textures().begin(), other.textures().end());
        m_textureViews.insert(other.textureViews().begin(), other.textureViews().end());
        m_buffers.insert(other.buffers().begin(), other.buffers().end());
        m_bufferViews.insert(other.bufferViews().begin(), other.bufferViews().end());

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
}