#include "Tests/Test_FrameGraph_Mocks.h"

namespace Test {
  namespace FrameGraph {

    EEngineStatus MockRenderContext::bindResource(PublicResourceId_t const&) { return EEngineStatus::Ok; }
    EEngineStatus MockRenderContext::unbindResource(PublicResourceId_t const&) { return EEngineStatus::Ok; }

    EEngineStatus
      MockRenderContext::render(Renderable const&renderable)
    {
      std::string message =
        String::format(
          "operation -> render(Renderable const&):\n"
          "Renderable: %0\n"
          "  MeshId:     %1\n"
          "  MaterialId: %2\n",
          renderable.name,
          renderable.meshId,
          renderable.materialId);
      Log::Verbose(logTag(), message);

      return EEngineStatus::Ok;
    }
    
    MockFrameGraphRenderContext::MockFrameGraphRenderContext(Ptr<IRenderContext> renderer)
      : m_renderer(renderer)
    {}

    Ptr<IFrameGraphRenderContext> 
      MockFrameGraphRenderContext::fromRenderer(Ptr<IRenderContext> renderer)
    {
      assert(renderer != nullptr);

      Ptr<FrameGraph::IFrameGraphRenderContext> context = Ptr<MockFrameGraphRenderContext>(new MockFrameGraphRenderContext(renderer));
      if(!context)
        Log::Error(logTag(), "Failed to create render context from renderer.");
      return context;
    }

    EEngineStatus MockFrameGraphRenderContext::importTexture(
      FrameGraphTexture const&texture)
    {
      std::cout << "ImportTexture(...):\n" << to_string(texture) << "\n";

      return EEngineStatus::Ok;
    }

    EEngineStatus MockFrameGraphRenderContext::createTexture(
      FrameGraphTexture const&texture)
    {
      std::cout << "CreateTexture(...):\n" << to_string(texture) << "\n";

      return EEngineStatus::Ok;
    }

    EEngineStatus MockFrameGraphRenderContext::createTextureView(
      FrameGraphTexture      const&texture,
      FrameGraphTextureView  const&view)
    {
      std::cout << "CreateTextureView(...):\n" << to_string(view) << "\n";
      return EEngineStatus::Ok;
    }

    EEngineStatus MockFrameGraphRenderContext::createBuffer(FrameGraphResourceId_t const&, FrameGraphResource const&, FrameGraphBuffer      const&buffer)
    {
      std::cout << "CreateBuffer(...):\n" << to_string(buffer) << "\n";
      return EEngineStatus::Ok;
    }

    EEngineStatus MockFrameGraphRenderContext::createBufferView(FrameGraphResourceId_t const&, FrameGraphResource const&, FrameGraphBufferView  const&view)
    {
      std::cout << "CreateBufferView(...):\n" << to_string(view) << "\n";
      return EEngineStatus::Ok;
    }


    EEngineStatus MockFrameGraphRenderContext::loadTextureAsset(AssetId_t const&)
    {
      return EEngineStatus::Ok;
    }

    EEngineStatus MockFrameGraphRenderContext::loadBufferAsset(AssetId_t  const&)
    {
      return EEngineStatus::Ok;
    }

    EEngineStatus MockFrameGraphRenderContext::loadMeshAsset(AssetId_t    const&)
    {
      return EEngineStatus::Ok;
    }


    EEngineStatus MockFrameGraphRenderContext::bindTextureView(
      FrameGraphTextureView const&view)
    {
      std::cout << "BindTextureView(...):\n" << to_string(view) << "\n";
      return EEngineStatus::Ok;
    }

    EEngineStatus MockFrameGraphRenderContext::bindBufferView(FrameGraphResourceId_t  const&)
    {
      return EEngineStatus::Ok;
    }

    EEngineStatus MockFrameGraphRenderContext::bindMesh(AssetId_t const&)
    {
      return EEngineStatus::Ok;
    }


    EEngineStatus MockFrameGraphRenderContext::unbindTextureView(
      FrameGraphTextureView const&view)
    {
      std::cout << "UnbindTextureView(...):\n" << to_string(view) << "\n";
      return EEngineStatus::Ok;
    }

    EEngineStatus MockFrameGraphRenderContext::unbindBufferView(FrameGraphResourceId_t  const&)
    {
      return EEngineStatus::Ok;
    }

    EEngineStatus MockFrameGraphRenderContext::unbindMesh(AssetId_t const&)
    {
      return EEngineStatus::Ok;
    }

    EEngineStatus MockFrameGraphRenderContext::unloadTextureAsset(AssetId_t const&)
    {
      return EEngineStatus::Ok;
    }

    EEngineStatus MockFrameGraphRenderContext::unloadBufferAsset(AssetId_t  const&)
    {
      return EEngineStatus::Ok;
    }

    EEngineStatus MockFrameGraphRenderContext::unloadMeshAsset(AssetId_t    const&)
    {
      return EEngineStatus::Ok;
    }

    EEngineStatus MockFrameGraphRenderContext::destroyTexture(
      FrameGraphTexture const&texture)
    {
      std::cout << "DestroyTexture(...):\n" << to_string(texture) << "\n";
      return EEngineStatus::Ok;
    }

    EEngineStatus MockFrameGraphRenderContext::destroyTextureView(
      FrameGraphTextureView  const&view)
    {
      std::cout << "DestroyTextureView(...):\n" << to_string(view) << "\n";
      return EEngineStatus::Ok;
    }

    EEngineStatus MockFrameGraphRenderContext::destroyBuffer(FrameGraphResourceId_t      const&)
    {
      return EEngineStatus::Ok;
    }

    EEngineStatus MockFrameGraphRenderContext::destroyBufferView(FrameGraphResourceId_t  const&)
    {
      return EEngineStatus::Ok;
    }

    EEngineStatus MockFrameGraphRenderContext::render(Renderable const&renderable)
    {
      std::cout << "Render(...):\n" << to_string(renderable) << "\n";
      return EEngineStatus::Ok;
    }
  }
}