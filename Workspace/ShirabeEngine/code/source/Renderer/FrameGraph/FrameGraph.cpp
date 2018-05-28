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

      // Don't care about explicit lifetimes for now. Keep it all up during execution and shutdown afterward.
      initializeTextures(renderContext);
      initializeTextureViews(renderContext);

      std::stack<PassUID_t> copy = m_passExecutionOrder;
      while(!copy.empty()) {
        PassUID_t     passUID = copy.top();
        Ptr<PassBase> pass    = m_passes.at(passUID);

        if(!pass->execute(m_resourceData, renderContext)) {
          Log::Error(logTag(), String::format("Failed to execute pass %0", pass->passUID()));
        }

        copy.pop();
      }

      deinitializeTextureViews(renderContext);
      deinitializeTextures(renderContext);

      return true;
    }

    bool Graph::initializeTextures(Ptr<IFrameGraphRenderContext> renderContext) {
      RefIndex const&textures = m_resourceData.textures();
      for(RefIndex::value_type const&textureRef : textures)
      {
        FrameGraphResourceId_t       id       =  textureRef;
        FrameGraphTexture      const&texture  = *m_resourceData.getMutable<FrameGraphTexture>(id);

        EEngineStatus status = EEngineStatus::Ok;

        if(texture.isExternalResource)
          status = renderContext->importTexture(texture);
        else
          status = renderContext->createTexture(texture);

        HandleEngineStatusError(status, "Failed to load texture for FrameGraphExecution.");
      }

      return true;
    }

    bool Graph::initializeTextureViews(Ptr<IFrameGraphRenderContext> renderContext) {
      RefIndex const&textureViews = m_resourceData.textureViews();
      for(RefIndex::value_type const&textureViewRef : textureViews)
      {
        FrameGraphResourceId_t id           = textureViewRef;
        FrameGraphTextureView  &textureView = *m_resourceData.get<FrameGraphTextureView>(id);
        FrameGraphTexture      &texture     = *m_resourceData.get<FrameGraphTexture>(textureView.subjacentResource);

        EEngineStatus status = renderContext->createTextureView(texture, textureView);
        HandleEngineStatusError(status, "Failed to load texture view for FrameGraphExecution.");
      }

      return true;
    }

    bool Graph::deinitializeTextureViews(Ptr<IFrameGraphRenderContext> renderContext) {
      RefIndex const&textureViews = m_resourceData.textureViews();
      for(RefIndex::value_type const&textureViewRef : textureViews)
      {
        FrameGraphResourceId_t id           = textureViewRef;
        FrameGraphTextureView  &textureView = *m_resourceData.get<FrameGraphTextureView>(id);

        EEngineStatus status = renderContext->destroyTextureView(textureView);
        HandleEngineStatusError(status, "Failed to unload texture view for FrameGraphExecution.");
      }

      return true;
    }

    bool Graph::deinitializeTextures(Ptr<IFrameGraphRenderContext> renderContext) {
      RefIndex const&textures = m_resourceData.textures();
      for(RefIndex::value_type const&textureRef : textures)
      {
        FrameGraphResourceId_t       id       =  textureRef;
        FrameGraphTexture      const&texture  = *m_resourceData.getMutable<FrameGraphTexture>(id);

        if(texture.isExternalResource)
          continue;

        EEngineStatus status = renderContext->destroyTexture(texture);
        HandleEngineStatusError(status, "Failed to unload texture for FrameGraphExecution.");
      }

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