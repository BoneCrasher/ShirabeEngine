#include "Renderer/FrameGraph/FrameGraph.h"

#include "Resources/Core/IResourceManager.h"
#include "GFXAPI/Types/All.h"

namespace Engine {
  namespace FrameGraph {

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

    PassMap&
      Graph::passes()
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