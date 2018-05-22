#ifndef __SHIRABE_FRAMEGRAPH_FRAMEGRAPHDATA_H__
#define __SHIRABE_FRAMEGRAPH_FRAMEGRAPHDATA_H__

#include <stdint.h>
#include <type_traits>
#include <unordered_map>
#include <vector>
#include <variant>

// #include <better-enums/enum.h>

#include "Platform/Platform.h"
#include "Core/BitField.h"
#include "Core/BasicTypes.h"

#include "Resources/Types/Definition.h"
#include "Resources/Types/Texture.h"

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
      BufferView,
      RenderableList,
      RenderableListView // To support renderable filtering...
    };

    /**********************************************************************************************//**
     * \enum  FrameGraphFormat
     *
     * \brief Values that represent texture and buffer formats
     **************************************************************************************************/
    using FrameGraphFormat = Engine::Resources::Format;

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

    enum class FrameGraphReadSource
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

    enum class FrameGraphViewSource
      : uint8_t 
    {
      Undefined = 0,
      Color,
      Depth
    };

    enum class FrameGraphViewAccessMode 
      : uint8_t 
    {
      Undefined = 0,
      Read,
      Write
    };

    SHIRABE_TEST_EXPORT std::ostream& operator<<(std::ostream &strm, FrameGraphFormat const&e);
    SHIRABE_TEST_EXPORT std::ostream& operator<<(std::ostream &strm, FrameGraphResourceType const&e);
    
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
      bool
        isExternalResource;

      FrameGraphResource();

      inline operator FrameGraphResourceId_t() { return resourceId; }
    };
    DeclareMapType(FrameGraphResourceId_t, RefWrapper<FrameGraphResource>, FrameGraphResourceRef);

    SHIRABE_TEST_EXPORT bool operator<(FrameGraphResource const&l, FrameGraphResource const&r);

    struct SHIRABE_TEST_EXPORT FrameGraphBuffer
      : public FrameGraphResource
    {
      uint32_t
        elementSize,
        elementCount;

      FrameGraphBuffer();
    };
    DeclareMapType(FrameGraphResourceId_t, FrameGraphBuffer, FrameGraphBuffer);

    struct SHIRABE_TEST_EXPORT FrameGraphBufferView
      : public FrameGraphResource {
      Range
        subrange;
      FrameGraphFormat
        format;
      BitField<FrameGraphViewAccessMode>
        mode;

      FrameGraphBufferView();
    };
    DeclareMapType(FrameGraphResourceId_t, FrameGraphBufferView, FrameGraphBufferView);

    struct SHIRABE_TEST_EXPORT FrameGraphTexture
      : public FrameGraphResource
      , public Resources::TextureInfo
    {
      FrameGraphResourceInitState
        initialState;
      BitField<FrameGraphResourceUsage>
        permittedUsage,
        requestedUsage;

      FrameGraphTexture();

      virtual bool validate() const;
    };
    DeclareMapType(FrameGraphResourceId_t, FrameGraphTexture, FrameGraphTexture);

    struct SHIRABE_TEST_EXPORT FrameGraphTextureView
      : public FrameGraphResource {
      Range
        arraySliceRange,
        mipSliceRange;
      FrameGraphFormat
        format;
      BitField<FrameGraphViewAccessMode>
        mode;
      FrameGraphViewSource
        source;

      FrameGraphTextureView();
    };
    DeclareMapType(FrameGraphResourceId_t, FrameGraphTextureView, FrameGraphTextureView);

    struct SHIRABE_TEST_EXPORT FrameGraphResourceFlags {
      FrameGraphFormat
        requiredFormat;

      FrameGraphResourceFlags();
    };

    struct SHIRABE_TEST_EXPORT FrameGraphReadTextureFlags
      : public FrameGraphResourceFlags
    {
      FrameGraphReadSource
        source;
    };

    struct SHIRABE_TEST_EXPORT FrameGraphWriteTextureFlags
      : public FrameGraphResourceFlags
    {
      FrameGraphWriteTarget
        writeTarget;
    };

    struct FrameGraphRenderableListView {
      std::vector<uint64_t> renderableRefIndices;
    };

    using FrameGraphRenderable     = Renderer::Renderable;
    using FrameGraphRenderableList = Renderer::RenderableList;
    DeclareMapType(FrameGraphResourceId_t, FrameGraphRenderableList, FrameGraphRenderableList);
    DeclareMapType(FrameGraphResourceId_t, FrameGraphRenderableListView, FrameGraphRenderableListView);

    SHIRABE_TEST_EXPORT bool operator!=(FrameGraphResource const&l, FrameGraphResource const&r);

    DeclareMapType(FrameGraphResourceId_t, FrameGraphResource, FrameGraphResource);
    
    template <typename TUnderlyingIDFrom, typename TUnderlyingIDTo = TUnderlyingIDFrom>
    using AdjacencyListMap = std::unordered_map<TUnderlyingIDFrom, std::vector<TUnderlyingIDTo>>;

    DeclareMapType(FrameGraphResourceId_t, Renderer::RenderableList, RenderableList);
    class FrameGraphResources {
    public:
      Optional<RefWrapper<FrameGraphTexture const>>     getTexture(FrameGraphResourceId_t const&)     const;
      Optional<RefWrapper<FrameGraphTextureView const>> getTextureView(FrameGraphResourceId_t const&) const;
      Optional<RefWrapper<FrameGraphBuffer const>>      getBuffer(FrameGraphResourceId_t const&)      const;
      Optional<RefWrapper<FrameGraphBufferView const>>  getBufferView(FrameGraphResourceId_t const&)  const;
      
      Optional<RefWrapper<FrameGraphRenderableList const>>     getRenderableList(FrameGraphResourceId_t const&) const;
      Optional<RefWrapper<FrameGraphRenderableListView const>> getRenderableListView(FrameGraphResourceId_t const&) const;

      inline FrameGraphTextureMap            const&textures()            const { return m_textures; }
      inline FrameGraphTextureViewMap        const&textureViews()        const { return m_textureViews; }
      inline FrameGraphBufferMap             const&buffers()             const { return m_buffers; }
      inline FrameGraphBufferViewMap         const&bufferViews()         const { return m_bufferViews; }
      inline FrameGraphRenderableListMap     const&renderablesLists()    const { return m_renderableLists; }
      inline FrameGraphRenderableListViewMap const&renderableListViews() const { return m_renderableListViews; }

    protected:
      FrameGraphTextureMap            m_textures;
      FrameGraphTextureViewMap        m_textureViews;
      FrameGraphBufferMap             m_buffers;
      FrameGraphBufferViewMap         m_bufferViews;
      FrameGraphRenderableListMap     m_renderableLists;
      FrameGraphRenderableListViewMap m_renderableListViews;
    };

    class FrameGraphMutableResources
      : public FrameGraphResources
    {
    public:
      bool addTexture(FrameGraphResourceId_t const&, FrameGraphTexture const&);
      bool addTextureView(FrameGraphResourceId_t const&, FrameGraphTextureView const&);
      bool addBuffer(FrameGraphResourceId_t const&, FrameGraphBuffer const&);
      bool addBufferView(FrameGraphResourceId_t const&, FrameGraphBufferView const&);
      bool addRenderableList(FrameGraphResourceId_t const&, FrameGraphRenderableList const&);
      bool addRenderableListView(FrameGraphResourceId_t const&, FrameGraphRenderableListView const&);

      Optional<RefWrapper<FrameGraphTexture>>     getMutableTexture(FrameGraphResourceId_t const&);
      Optional<RefWrapper<FrameGraphTextureView>> getMutableTextureView(FrameGraphResourceId_t const&);
      Optional<RefWrapper<FrameGraphBuffer>>      getMutableBuffer(FrameGraphResourceId_t const&);
      Optional<RefWrapper<FrameGraphBufferView>>  getMutableBufferView(FrameGraphResourceId_t const&);

      Optional<RefWrapper<FrameGraphRenderableList>>     getMutableRenderableList(FrameGraphResourceId_t const&);
      Optional<RefWrapper<FrameGraphRenderableListView>> getMutableRenderableListView(FrameGraphResourceId_t const&);

      bool mergeIn(FrameGraphResources const&other);
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