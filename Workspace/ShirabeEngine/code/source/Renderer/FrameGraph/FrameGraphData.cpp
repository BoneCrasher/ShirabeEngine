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

    FrameGraphTexture::FrameGraphTexture()
      : TextureInfo()
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

    Optional<RefWrapper<FrameGraphTexture const>>
      FrameGraphResources::getTexture(FrameGraphResourceId_t const&resource) const
    {
      if(m_textures.find(resource) == m_textures.end())
        return {};

      return RefWrapper<FrameGraphTexture const>(m_textures.at(resource));
    }

    Optional<RefWrapper<FrameGraphTextureView const>>
      FrameGraphResources::getTextureView(FrameGraphResourceId_t const&resource) const
    {
      if(m_textureViews.find(resource) == m_textureViews.end())
        return { };

      return RefWrapper<FrameGraphTextureView const>(m_textureViews.at(resource));
    }

    Optional<RefWrapper<FrameGraphBuffer const>>
      FrameGraphResources::getBuffer(FrameGraphResourceId_t const&resource) const
    {
      if(m_buffers.find(resource) == m_buffers.end())
        return { };

      return RefWrapper<FrameGraphBuffer const>(m_buffers.at(resource));
    }

    Optional<RefWrapper<FrameGraphBufferView const>>
      FrameGraphResources::getBufferView(FrameGraphResourceId_t const&resource) const
    {
      if(m_bufferViews.find(resource) == m_bufferViews.end())
        return { };

      return RefWrapper<FrameGraphBufferView const>(m_bufferViews.at(resource));
    }

    Optional<RefWrapper<FrameGraphRenderableList const>>
      FrameGraphResources::getRenderableList(FrameGraphResourceId_t const&resource) const
    {
      if(m_renderableLists.find(resource) == m_renderableLists.end())
        return { };

      return RefWrapper<FrameGraphRenderableList const>(m_renderableLists.at(resource));
    }

    Optional<RefWrapper<FrameGraphRenderableListView const>>
      FrameGraphResources::getRenderableListView(FrameGraphResourceId_t const&resource) const
    {
      if(m_renderableListViews.find(resource) == m_renderableListViews.end())
        return { };

      return RefWrapper<FrameGraphRenderableListView const>(m_renderableListViews.at(resource));
    }


    bool FrameGraphMutableResources::addTexture(
      FrameGraphResourceId_t const&resource,
      FrameGraphTexture      const&texture)
    {
      return addIfNotAdded(m_textures, resource, texture);
    }

    bool FrameGraphMutableResources::addTextureView(
      FrameGraphResourceId_t const&resource,
      FrameGraphTextureView  const&view)
    {
      return addIfNotAdded(m_textureViews, resource, view);
    }

    bool FrameGraphMutableResources::addBuffer(
      FrameGraphResourceId_t const&resource,
      FrameGraphBuffer   const&buffer)
    {
      return addIfNotAdded(m_buffers, resource, buffer);
    }

    bool FrameGraphMutableResources::addBufferView(
      FrameGraphResourceId_t   const&resource,
      FrameGraphBufferView const&view)
    {
      return addIfNotAdded(m_bufferViews, resource, view);
    }

    bool FrameGraphMutableResources::addRenderableList(
      FrameGraphResourceId_t       const&resource,
      FrameGraphRenderableList const&list)
    {
      return addIfNotAdded(m_renderableLists, resource, list);
    }

    bool FrameGraphMutableResources::addRenderableListView(
      FrameGraphResourceId_t           const&resource,
      FrameGraphRenderableListView const&view)
    {
      return addIfNotAdded(m_renderableListViews, resource, view);
    }

    Optional<RefWrapper<FrameGraphTexture>>
      FrameGraphMutableResources::getMutableTexture(FrameGraphResourceId_t const&resource)
    {
      if(m_textures.find(resource) == m_textures.end())
        return { };

      return RefWrapper<FrameGraphTexture>(m_textures.at(resource));
    }

    Optional<RefWrapper<FrameGraphTextureView>>
      FrameGraphMutableResources::getMutableTextureView(FrameGraphResourceId_t const&resource)
    {
      if(m_textureViews.find(resource) == m_textureViews.end())
        return { };

      return RefWrapper<FrameGraphTextureView>(m_textureViews.at(resource));
    }

    Optional<RefWrapper<FrameGraphBuffer>>
      FrameGraphMutableResources::getMutableBuffer(FrameGraphResourceId_t const&resource)
    {
      if(m_buffers.find(resource) == m_buffers.end())
        return { };

      return RefWrapper<FrameGraphBuffer>(m_buffers.at(resource));
    }

    Optional<RefWrapper<FrameGraphBufferView>>
      FrameGraphMutableResources::getMutableBufferView(FrameGraphResourceId_t const&resource)
    {
      if(m_bufferViews.find(resource) == m_bufferViews.end())
        return { };

      return RefWrapper<FrameGraphBufferView>(m_bufferViews.at(resource));
    }

    Optional<RefWrapper<FrameGraphRenderableList>>
      FrameGraphMutableResources::getMutableRenderableList(FrameGraphResourceId_t const&resource)
    {
      if(m_renderableLists.find(resource) == m_renderableLists.end())
        return { };

      return RefWrapper<FrameGraphRenderableList>(m_renderableLists.at(resource));
    }

    Optional<RefWrapper<FrameGraphRenderableListView>>
      FrameGraphMutableResources::getMutableRenderableListView(FrameGraphResourceId_t const&resource)
    {
      if(m_renderableListViews.find(resource) == m_renderableListViews.end())
        return { };

      return RefWrapper<FrameGraphRenderableListView>(m_renderableListViews.at(resource));
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
        m_renderableLists.insert(other.renderablesLists().begin(), other.renderablesLists().end());
        m_renderableListViews.insert(other.renderableListViews().begin(), other.renderableListViews().end());

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