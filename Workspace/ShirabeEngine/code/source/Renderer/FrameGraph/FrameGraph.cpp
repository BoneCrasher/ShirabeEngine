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
      FrameGraphTextureMap const&textures = m_resourceData.textures();
      for(FrameGraphTextureMap::value_type const&textureAssignment : textures)
      {
        FrameGraphResourceId_t id       = textureAssignment.first;
        FrameGraphResource     resource = m_resources.at(id);
        FrameGraphTexture      texture  = textureAssignment.second;

        EEngineStatus status = renderContext->createTexture(id, resource, texture);
        HandleEngineStatusError(status, "Failed to load texture for FrameGraphExecution.");
      }

      return true;
    }

    bool Graph::initializeTextureViews(Ptr<IFrameGraphRenderContext> renderContext) {
      FrameGraphTextureViewMap const&textureViews = m_resourceData.textureViews();
      for(FrameGraphTextureViewMap::value_type const&textureViewAssignment : textureViews)
      {
        FrameGraphResourceId_t id          = textureViewAssignment.first;
        FrameGraphResource     resource    = m_resources.at(id);
        FrameGraphTexture      texture     = m_resourceData.getTexture(resource.subjacentResource);
        FrameGraphTextureView  textureView = textureViewAssignment.second;

        EEngineStatus status = renderContext->createTextureView(id, resource, texture, textureView);
        HandleEngineStatusError(status, "Failed to load texture view for FrameGraphExecution.");
      }

      return true;
    }

    bool Graph::deinitializeTextureViews(Ptr<IFrameGraphRenderContext> renderContext) {
      FrameGraphTextureViewMap const&textureViews = m_resourceData.textureViews();
      for(FrameGraphTextureViewMap::value_type const&textureViewAssignment : textureViews)
      {
        FrameGraphResourceId_t id          = textureViewAssignment.first;
        FrameGraphTextureView  textureView = textureViewAssignment.second;

        EEngineStatus status = renderContext->destroyTextureView(id, textureView);
        HandleEngineStatusError(status, "Failed to unload texture view for FrameGraphExecution.");
      }

      return true;
    }

    bool Graph::deinitializeTextures(Ptr<IFrameGraphRenderContext> renderContext) {
      FrameGraphTextureMap const&textures = m_resourceData.textures();
      for(FrameGraphTextureMap::value_type const&textureAssignment : textures)
      {
        FrameGraphResourceId_t id       = textureAssignment.first;

        EEngineStatus status = renderContext->destroyTexture(id);
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