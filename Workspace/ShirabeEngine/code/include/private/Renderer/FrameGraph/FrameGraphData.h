#ifndef __SHIRABE_FRAMEGRAPH_FRAMEGRAPHDATA_H__
#define __SHIRABE_FRAMEGRAPH_FRAMEGRAPHDATA_H__

#include <stdint.h>
#include <type_traits>
#include <unordered_map>
#include <vector>
#include <variant>
#include <memory>

// #include <better-enums/enum.h>

#include "Platform/Platform.h"
#include "Core/BitField.h"
#include "Core/BasicTypes.h"

#include "Resources/Subsystems/GFXAPI/Types/Definition.h"
#include "Resources/Subsystems/GFXAPI/Types/Texture.h"

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
      uint32_t
        referenceCount;
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
    DeclareMapType(FrameGraphResourceId_t, FrameGraphResource, FrameGraphResource);

    SHIRABE_TEST_EXPORT bool operator<(FrameGraphResource const&l, FrameGraphResource const&r);

    struct SHIRABE_TEST_EXPORT FrameGraphBuffer
      : public FrameGraphResource
    {
      uint32_t
        elementSize,
        elementCount;

      FrameGraphBuffer();
    };
    DeclareListType(FrameGraphBuffer, FrameGraphBuffer);
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
    DeclareListType(FrameGraphBufferView, FrameGraphBufferView);
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

      void assignTextureParameters(FrameGraphTexture const&other);

      virtual bool validate() const;
    };
    DeclareListType(FrameGraphTexture, FrameGraphTexture);
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
    DeclareListType(FrameGraphTextureView, FrameGraphTextureView);
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

    struct FrameGraphRenderableList
      : public FrameGraphResource
    {
      Renderer::RenderableList renderableList;
    };

    struct FrameGraphRenderableListView 
      : public FrameGraphResource
    {
      std::vector<uint64_t> renderableRefIndices;
    };

    using FrameGraphRenderable = Renderer::Renderable;
    DeclareListType(FrameGraphRenderableList, FrameGraphRenderableList);
    DeclareMapType(FrameGraphResourceId_t, FrameGraphRenderableList, FrameGraphRenderableList);
    DeclareListType(FrameGraphRenderableListView, FrameGraphRenderableListView);
    DeclareMapType(FrameGraphResourceId_t, FrameGraphRenderableListView, FrameGraphRenderableListView);

    SHIRABE_TEST_EXPORT bool operator!=(FrameGraphResource const&l, FrameGraphResource const&r);

    DeclareMapType(FrameGraphResourceId_t, FrameGraphResource, FrameGraphResource);
    
    template <typename TUnderlyingIDFrom, typename TUnderlyingIDTo = TUnderlyingIDFrom>
    using AdjacencyListMap = std::unordered_map<TUnderlyingIDFrom, std::vector<TUnderlyingIDTo>>;

    DeclareMapType(FrameGraphResourceId_t, Renderer::RenderableList, RenderableList);

    using Index    = Vector<Ptr<FrameGraphResource>>;
    using RefIndex = Vector<FrameGraphResourceId_t>;
  
    template <typename T>
    class FrameGraphResourcesRef {
    protected:
      void insert(FrameGraphResourceId_t const&ref) {
        m_index.push_back(ref);
      }

      RefIndex const&get() const  { return m_index; }
      RefIndex      &getMutable() { return m_index; }

    private:
      RefIndex
        m_index;
    };

    #define FrameGraphResources_SupportedTypes \
      FrameGraphTexture, \
      FrameGraphTextureView, \
      FrameGraphBuffer, \
      FrameGraphBufferView, \
      FrameGraphRenderableList, \
      FrameGraphRenderableListView

    template <typename... T>
    class FrameGraphResourcesRefContainer
      : public FrameGraphResourcesRef<T>...
    {};

    class FrameGraphResources 
      : public FrameGraphResourcesRefContainer<FrameGraphResources_SupportedTypes>
    {
    public:
      FrameGraphResources();

      template <typename T>
      Ptr<typename std::enable_if_t<std::is_base_of_v<FrameGraphResource, T>, T>> const 
        get(FrameGraphResourceId_t const&id) const
      {
        #if defined SHIRABE_DEBUG || defined SHIRABE_TEST
        if(m_resources.size() <= id)
          throw std::runtime_error("Resource handle not found.");
        #endif

        Ptr<FrameGraphResource> resource = m_resources.at(id);
        #if defined SHIRABE_DEBUG || defined SHIRABE_TEST
        if(nullptr == resource)
          throw std::runtime_error("Resource handle is empty.");
        #endif

        Ptr<T> ptr = std::static_pointer_cast<T>(resource);

        return ptr;
      }

      inline Index    const&resources()           const { return m_resources; }
      inline RefIndex const&textures()            const { return FrameGraphResourcesRef<FrameGraphTexture>::get(); }
      inline RefIndex const&textureViews()        const { return FrameGraphResourcesRef<FrameGraphTextureView>::get(); }
      inline RefIndex const&buffers()             const { return FrameGraphResourcesRef<FrameGraphBuffer>::get(); }
      inline RefIndex const&bufferViews()         const { return FrameGraphResourcesRef<FrameGraphBufferView>::get(); }
      inline RefIndex const&renderablesLists()    const { return FrameGraphResourcesRef<FrameGraphRenderableList>::get(); }
      inline RefIndex const&renderableListViews() const { return FrameGraphResourcesRef<FrameGraphRenderableListView>::get(); }

    protected:
      Index
        m_resources;
    };
        
    class FrameGraphMutableResources
      : public FrameGraphResources
    {
    public:
      template <typename T> // with T : FrameGraphResource
      typename std::enable_if_t<std::is_base_of_v<FrameGraphResource, T>, T>& 
        spawnResource()
      {
        Ptr<T> ptr = std::make_shared<T>();
        ptr->resourceId = m_resources.size();

        m_resources.push_back(ptr);

        FrameGraphResourcesRef<T>::insert(ptr->resourceId);

        return (*ptr);
      }

      template <typename T> // with T : FrameGraphResource
      Ptr<typename std::enable_if_t<std::is_base_of_v<FrameGraphResource, T>, T>> 
        getMutable(FrameGraphResourceId_t const&id)
      {
        return *const_cast<Ptr<T>*>(&static_cast<FrameGraphResources*>(this)->get<T>(id));
      }

      bool mergeIn(FrameGraphResources const&other);
    };

  }

  template <>
  SHIRABE_TEST_EXPORT std::string to_string<FrameGraph::FrameGraphResourceType>(FrameGraph::FrameGraphResourceType const&type);
  template <>
  SHIRABE_TEST_EXPORT std::string to_string<FrameGraph::FrameGraphFormat>(FrameGraph::FrameGraphFormat const&format);
  template <>
  SHIRABE_TEST_EXPORT std::string to_string<FrameGraph::FrameGraphResourceUsage>(FrameGraph::FrameGraphResourceUsage const&usage);
  template <>
  SHIRABE_TEST_EXPORT std::string to_string<FrameGraph::FrameGraphWriteTarget>(FrameGraph::FrameGraphWriteTarget const&target);
  template <>
  SHIRABE_TEST_EXPORT std::string to_string<FrameGraph::FrameGraphResourceAccessibility>(FrameGraph::FrameGraphResourceAccessibility const&accessibility);
  template <>
  SHIRABE_TEST_EXPORT std::string to_string<FrameGraph::FrameGraphResourceInitState>(FrameGraph::FrameGraphResourceInitState const&state);
  template <>
  SHIRABE_TEST_EXPORT std::string to_string<FrameGraph::FrameGraphViewAccessMode>(FrameGraph::FrameGraphViewAccessMode const&accessMode);


  template <>
  SHIRABE_TEST_EXPORT std::string to_string<FrameGraph::FrameGraphTexture>(FrameGraph::FrameGraphTexture const&);
  template <>
  SHIRABE_TEST_EXPORT std::string to_string<FrameGraph::FrameGraphTextureView>(FrameGraph::FrameGraphTextureView const&);
  template <>
  SHIRABE_TEST_EXPORT std::string to_string<FrameGraph::FrameGraphBuffer>(FrameGraph::FrameGraphBuffer const&);
  template <>
  SHIRABE_TEST_EXPORT  std::string to_string<FrameGraph::FrameGraphBufferView>(FrameGraph::FrameGraphBufferView const&);

}

#endif