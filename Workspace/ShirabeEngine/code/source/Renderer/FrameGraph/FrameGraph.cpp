#include "Renderer/FrameGraph/FrameGraph.h"

#include "Resources/Core/IResourceManager.h"
#include "GFXAPI/Types/All.h"

namespace Engine {
  namespace FrameGraph {

    Graph::Accessor::Accessor(Graph const*graph)
      : m_graph(graph)
    { }

    AdjacencyListMap<PassUID_t> const&
      Graph::Accessor::passAdjacency() const
    {
      return m_graph->m_passAdjacency;
    }

    std::stack<PassUID_t> const&
      Graph::Accessor::passExecutionOrder() const
    {
      return m_graph->m_passExecutionOrder;
    }

    FrameGraphResourceIdList const&
      Graph::Accessor::resources() const
    {
      return m_graph->m_resources;
    }

    FrameGraphMutableResources const&
      Graph::Accessor::resourceData() const
    {
      return m_graph->m_resourceData;
    }

    AdjacencyListMap<FrameGraphResourceId_t> const&
      Graph::Accessor::resourceAdjacency() const
    {
      return m_graph->m_resourceAdjacency;
    }

    std::stack<FrameGraphResourceId_t> const&
      Graph::Accessor::resourceOrder() const
    {
      return m_graph->m_resourceOrder;
    }

    AdjacencyListMap<PassUID_t, FrameGraphResourceId_t> const&
      Graph::Accessor::passToResourceAdjacency() const
    {
      return m_graph->m_passToResourceAdjacency;
    }

    Graph::MutableAccessor::MutableAccessor(Graph *graph)
      : Accessor(graph)
      , m_graph(graph)
    {}

    AdjacencyListMap<PassUID_t>&
      Graph::MutableAccessor::mutablePassAdjacency()
    {
      return m_graph->m_passAdjacency;
    }

    std::stack<PassUID_t>&
      Graph::MutableAccessor::mutablePassExecutionOrder()
    {
      return m_graph->m_passExecutionOrder;
    }

    FrameGraphResourceIdList&
      Graph::MutableAccessor::mutableResources()
    {
      return m_graph->m_resources;
    }

    FrameGraphMutableResources&
      Graph::MutableAccessor::mutableResourceData()
    {
      return m_graph->m_resourceData;
    }

    AdjacencyListMap<FrameGraphResourceId_t>&
      Graph::MutableAccessor::mutableResourceAdjacency()
    {
      return m_graph->m_resourceAdjacency;
    }

    std::stack<FrameGraphResourceId_t>&
      Graph::MutableAccessor::mutableResourceOrder()
    {
      return m_graph->m_resourceOrder;
    }

    AdjacencyListMap<PassUID_t, FrameGraphResourceId_t>&
      Graph::MutableAccessor::mutablePassToResourceAdjacency()
    {
      return m_graph->m_passToResourceAdjacency;
    }

    void
      Graph::acceptSerializer(Ptr<IFrameGraphSerializer> s)
    {
      s->serializeGraph(*this);
    }


    void
      Graph::acceptDeserializer(Ptr<IFrameGraphDeserializer> const&d)
    {
      d->deserializeGraph(*this);
    }

    Graph&
      Graph::operator=(Graph const&other)
    {
      m_passes                  = other.m_passes;
      m_passAdjacency           = other.m_passAdjacency;
      m_passExecutionOrder      = other.m_passExecutionOrder;
      m_resourceAdjacency       = other.m_resourceAdjacency;
      m_resourceOrder           = other.m_resourceOrder;
      m_passToResourceAdjacency = other.m_passToResourceAdjacency;

      return (*this);
    }

    bool
      Graph::execute(Ptr<IFrameGraphRenderContext>&renderContext)
    {
      assert(renderContext != nullptr);

      std::stack<PassUID_t> copy = m_passExecutionOrder;
      while(!copy.empty()) {
        PassUID_t                     passUID  = copy.top();
        Ptr<PassBase>                 pass     = m_passes.at(passUID);
        UniquePtr<PassBase::Accessor> accessor = pass->getAccessor(PassKey<Graph>());

        FrameGraphResourceIdList const&passResources = accessor->resourceReferences();

        bool initialized = initializeResources(renderContext, passResources);
        if(!initialized) {

        }

        bool executed = pass->execute(m_resourceData, renderContext);
        if(!executed) {
          Log::Error(logTag(), String::format("Failed to execute pass %0", pass->passUID()));
        }

        bool deinitialized = deinitializeResources(renderContext, passResources);
        if(!deinitialized) {

        }

        copy.pop();
      }

      return true;
    }

    bool Graph::initializeResources(
      Ptr<IFrameGraphRenderContext>       renderContext,
      FrameGraphResourceIdList      const&resourceIds)
    {
      bool initialized = true;

      for(FrameGraphResourceId_t const&id : resourceIds)
      {
        Ptr<FrameGraphResource>    subjacent   = nullptr;
        Ptr<FrameGraphTexture>     texture     = nullptr;
        Ptr<FrameGraphTextureView> textureView = nullptr;
        
        FrameGraphResourceIdList::const_iterator it = m_instantiatedResources.end();

        Ptr<FrameGraphResource> const resource = m_resourceData.get<FrameGraphResource>(id);
        switch(resource->type) {
        case FrameGraphResourceType::Texture:
          texture = std::static_pointer_cast<FrameGraphTexture>(resource);

          it = std::find(m_instantiatedResources.begin(), m_instantiatedResources.end(), texture->resourceId);
          if(it == m_instantiatedResources.end()) {
            initialized |=
              initializeTexture(
                renderContext,
                texture);
            m_instantiatedResources.push_back(texture->resourceId);
          }

          break;
        case FrameGraphResourceType::TextureView:
          subjacent   = m_resourceData.get<FrameGraphResource>(resource->subjacentResource);
          texture     = std::static_pointer_cast<FrameGraphTexture>(subjacent);
          textureView = std::static_pointer_cast<FrameGraphTextureView>(resource);

          it = std::find(m_instantiatedResources.begin(), m_instantiatedResources.end(), textureView->resourceId);
          if(it == m_instantiatedResources.end()) {
            initialized |=
              initializeTextureView(
                renderContext,
                texture,
                textureView);
            m_instantiatedResources.push_back(textureView->resourceId);
          }
          break;
        }
      }

      return initialized;
    }

    bool Graph::deinitializeResources(
      Ptr<IFrameGraphRenderContext>       renderContext,
      FrameGraphResourceIdList      const&resourceIds)
    {
      std::function<bool(FrameGraphResourceId_t const&, bool)> recurse = nullptr;
      recurse = [&, this] (FrameGraphResourceId_t const&id, bool includeSubjacent) -> bool
      {
        bool deinitialized = true;

        Ptr<FrameGraphResource> resource = m_resourceData.getMutable<FrameGraphResource>(id);
        
        Ptr<FrameGraphResource>    subjacent   = nullptr;
        Ptr<FrameGraphTexture>     texture     = nullptr;
        Ptr<FrameGraphTextureView> textureView = nullptr;

        switch(resource->type) {
        case FrameGraphResourceType::Texture:
          if(includeSubjacent) {
            if(resource->referenceCount == 0) {
              texture = std::static_pointer_cast<FrameGraphTexture>(resource);
              deinitialized |=
                deinitializeTexture(
                  renderContext,
                  texture);
              std::remove_if(m_instantiatedResources.begin(), m_instantiatedResources.end(), [&] (FrameGraphResourceId_t const&id) -> bool { return (id == texture->resourceId); });
            }
          }
          break;
        case FrameGraphResourceType::TextureView:
          // Decrease the texture view's count
          --(resource->referenceCount);
          std::cout
            << String::format("TextureView Id %0 -> RefCount: %1\n", resource->resourceId, resource->referenceCount);

          if(resource->referenceCount == 0) {
            subjacent   = m_resourceData.get<FrameGraphResource>(resource->subjacentResource);
            texture     = std::static_pointer_cast<FrameGraphTexture>(subjacent);
            textureView = std::static_pointer_cast<FrameGraphTextureView>(resource);
            deinitialized |=
              deinitializeTextureView(
                renderContext,
                texture,
                textureView);
            std::remove_if(m_instantiatedResources.begin(), m_instantiatedResources.end(), [&] (FrameGraphResourceId_t const&id) -> bool { return (id == textureView->resourceId); });

            --(texture->referenceCount);
            std::cout
              << String::format("Texture Id %0 -> RefCount: %1\n", texture->resourceId, texture->referenceCount);

            if(texture->referenceCount == 0) {
              deinitialized &= recurse(texture->resourceId, true);
            }
          }

          break;
        }

        return deinitialized;
      };
      
      bool deinitialized = true;

      for(FrameGraphResourceId_t const&id : resourceIds)
        deinitialized &= recurse(id, false);

      return deinitialized;
    }

    bool Graph::initializeTexture(
      Ptr<IFrameGraphRenderContext> renderContext,
      Ptr<FrameGraphTexture>        texture)
    {
      EEngineStatus status = EEngineStatus::Ok;

      if(texture->isExternalResource)
        status = renderContext->importTexture(*texture);
      else
        status = renderContext->createTexture(*texture);

      HandleEngineStatusError(status, "Failed to load texture for FrameGraphExecution.");

      return true;
    }

    bool Graph::initializeTextureView(
      Ptr<IFrameGraphRenderContext> renderContext,
      Ptr<FrameGraphTexture>        texture,
      Ptr<FrameGraphTextureView>    textureView)
    {
      FrameGraphResourceId_t id = textureView->resourceId;

      EEngineStatus status = renderContext->createTextureView(*texture, *textureView);
      HandleEngineStatusError(status, "Failed to load texture view for FrameGraphExecution.");

      return true;
    }

    bool Graph::deinitializeTextureView(
      Ptr<IFrameGraphRenderContext> renderContext,
      Ptr<FrameGraphTexture>        texture,
      Ptr<FrameGraphTextureView>    textureView)
    {
      EEngineStatus status = renderContext->destroyTextureView(*textureView);
      HandleEngineStatusError(status, "Failed to unload texture view for FrameGraphExecution.");


      return true;
    }

    bool Graph::deinitializeTexture(
      Ptr<IFrameGraphRenderContext> renderContext,
      Ptr<FrameGraphTexture>        texture)
    {
      if(texture->isExternalResource)
        return true;

      EEngineStatus status = renderContext->destroyTexture(*texture);
      HandleEngineStatusError(status, "Failed to unload texture for FrameGraphExecution.");

      return true;
    }

    PassMap const&
      Graph::passes() const
    {
      return m_passes;
    }

    bool
      Graph::addPass(Ptr<PassBase> const&pass)
    {
      if(m_passes.find(pass->passUID()) != m_passes.end())
        return false;

      m_passes[pass->passUID()] = pass;

      return true;
    }

  }
}