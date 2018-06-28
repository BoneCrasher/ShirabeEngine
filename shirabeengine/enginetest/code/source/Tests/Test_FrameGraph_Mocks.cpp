#include "Tests/Test_FrameGraph_Mocks.h"

namespace Test {
  namespace FrameGraph {

    EEngineStatus MockRenderContext::bindResource(PublicResourceId_t const&pid) { 
      Log::Verbose(logTag(), String::format("bindResource(ID: %0);", pid));

      return EEngineStatus::Ok; 
    }

    EEngineStatus MockRenderContext::unbindResource(PublicResourceId_t const&pid) {
      Log::Verbose(logTag(), String::format("unbindResource(ID: %0);", pid));

      return EEngineStatus::Ok; 
    }

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
      Log::Verbose(logTag(), String::format("ImportTexture(...):\n%0", to_string(texture)));

      return EEngineStatus::Ok;
    }

    EEngineStatus MockFrameGraphRenderContext::createTexture(
      FrameGraphTexture const&texture)
    {
      Log::Verbose(logTag(), String::format("CreateTexture(...):\n%0", to_string(texture)));

      return EEngineStatus::Ok;
    }

    EEngineStatus MockFrameGraphRenderContext::createTextureView(
      FrameGraphTexture      const&texture,
      FrameGraphTextureView  const&view)
    {
      Log::Verbose(logTag(), String::format("CreateTextureView(...):\n%0", to_string(view)));
      return EEngineStatus::Ok;
    }

    EEngineStatus MockFrameGraphRenderContext::createBuffer(FrameGraphResourceId_t const&, FrameGraphResource const&, FrameGraphBuffer      const&buffer)
    {
      Log::Verbose(logTag(), String::format("CreateBuffer(...):\n%0", to_string(buffer)));
      return EEngineStatus::Ok;
    }

    EEngineStatus MockFrameGraphRenderContext::createBufferView(FrameGraphResourceId_t const&, FrameGraphResource const&, FrameGraphBufferView  const&view)
    {
      Log::Verbose(logTag(), String::format("CreateBufferView(...):\n%0", to_string(view)));
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
      Log::Verbose(logTag(), String::format("BindTextureView(...):\n%0", to_string(view)));
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
      Log::Verbose(logTag(), String::format("UnbindTextureView(...):\n%0", to_string(view)));
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
      Log::Verbose(logTag(), String::format("DestroyTexture(...):\n%0", to_string(texture)));
      return EEngineStatus::Ok;
    }

    EEngineStatus MockFrameGraphRenderContext::destroyTextureView(
      FrameGraphTextureView  const&view)
    {
      Log::Verbose(logTag(), String::format("DestroyTextureView(...):\n%0", to_string(view)));
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
      Log::Verbose(logTag(), String::format("Render(...):\n", to_string(renderable)));
      return EEngineStatus::Ok;
    }


    #define Mock_DefineResourceMethods(resource)              \
                                                              \
      EEngineStatus MockResourceManager::create##resource(    \
        resource::CreationRequest const&inRequest,            \
        PublicResourceId_t             &outId,                \
        bool                            deferLoad             \
      ) {                                                     \
        Log::Verbose(logTag(), String::format("create%0(...);", #resource));    \
        load##resource(outId);                                \
        return EEngineStatus::Ok;                             \
      }                                                       \
                                                              \
      EEngineStatus MockResourceManager::load##resource(      \
        PublicResourceId_t const&inId                         \
      ) {                                                     \
        Log::Verbose(logTag(), String::format("load%0(...);", #resource));    \
        return EEngineStatus::Ok;                             \
      }                                                       \
                                                              \
      EEngineStatus MockResourceManager::update##resource(    \
        PublicResourceId_t      const&inId,                   \
        resource::UpdateRequest const&inRequest               \
      ) {                                                     \
        Log::Verbose(logTag(), String::format("update%0(...);", #resource));    \
        return EEngineStatus::Ok;                             \
      }                                                       \
                                                              \
      EEngineStatus MockResourceManager::unload##resource(    \
        PublicResourceId_t const&inId                         \
      ) {                                                     \
        Log::Verbose(logTag(), String::format("unload%0(...);", #resource));    \
        return EEngineStatus::Ok;                             \
      }                                                       \
                                                              \
      EEngineStatus MockResourceManager::destroy##resource(   \
        PublicResourceId_t const&inId                         \
      ) {                                                     \
        unload##resource(inId);                               \
        Log::Verbose(logTag(), String::format("destroy%0(...);", #resource));    \
        return EEngineStatus::Ok;                             \
      }

    Mock_DefineResourceMethods(SwapChain);
    Mock_DefineResourceMethods(Texture);
    Mock_DefineResourceMethods(RenderTargetView);
    Mock_DefineResourceMethods(ShaderResourceView);
    Mock_DefineResourceMethods(DepthStencilView);
    Mock_DefineResourceMethods(DepthStencilState);
    Mock_DefineResourceMethods(RasterizerState);

    bool MockResourceManager::clear() { std::cout << "Cleared resource manager \n"; return true; }

    Ptr<BasicGFXAPIResourceBackend>& MockResourceManager::backend() { throw std::exception("Don't call me..."); }

    #define Mock_DefineTaskBuilderModule(Type)                                                                                                                  \
          EEngineStatus MockGFXAPITaskBackend::creationTask   (Type::CreationRequest    const&request, ResolvedDependencyCollection const&depencies, ResourceTaskFn_t &outTask)   \
          {                                                                                                                                                                       \
            Log::Verbose(logTag(), String::format("creationTask<%0>(...)", #Type));                                                                                               \
            outTask = [&, this] () -> GFXAPIResourceHandleAssignment                                                                                                              \
            {                                                                                                                                                                     \
              GFXAPIResourceHandleAssignment assignment ={ };                                                                                                                     \
              assignment.publicHandle   = 1;                                                                                                                                      \
              assignment.internalHandle = std::static_pointer_cast<void>(MakeSharedPointerType<int>());                                                                           \
              return assignment;                                                                                                                                                  \
            };                                                                                                                                                                    \
            return EEngineStatus::Ok;                                                                                                                                             \
          }                                                                                                                                                                       \
          EEngineStatus MockGFXAPITaskBackend::updateTask     (Type::UpdateRequest      const&request, ResolvedDependencyCollection const&depencies, ResourceTaskFn_t &outTask)   \
          {                                                                                                                                                                       \
            Log::Verbose(logTag(), String::format("updateTask<%0>(...)", #Type));                                                                                                 \
            outTask = [&, this] () -> GFXAPIResourceHandleAssignment                                                                                                              \
            {                                                                                                                                                                     \
              GFXAPIResourceHandleAssignment assignment ={ };                                                                                                                     \
              assignment.publicHandle   = 1;                                                                                                                                      \
              assignment.internalHandle = std::static_pointer_cast<void>(MakeSharedPointerType<int>());                                                                           \
              return assignment;                                                                                                                                                  \
            };                                                                                                                                                                    \
            return EEngineStatus::Ok;                                                                                                                                             \
          }                                                                                                                                                                       \
          EEngineStatus MockGFXAPITaskBackend::destructionTask(Type::DestructionRequest const&request, ResolvedDependencyCollection const&depencies, ResourceTaskFn_t &outTask)   \
          {                                                                                                                                                                       \
            Log::Verbose(logTag(), String::format("destructionTask<%0>(...)", #Type));                                                                                            \
            outTask = [&, this] () -> GFXAPIResourceHandleAssignment                                                                                                              \
            {                                                                                                                                                                     \
              GFXAPIResourceHandleAssignment assignment ={ };                                                                                                                     \
              assignment.publicHandle   = 1;                                                                                                                                      \
              assignment.internalHandle = std::static_pointer_cast<void>(MakeSharedPointerType<int>());                                                                           \
              return assignment;                                                                                                                                                  \
            };                                                                                                                                                                    \
            return EEngineStatus::Ok;                                                                                                                                             \
          }                                                                                                                                                                       \
          EEngineStatus MockGFXAPITaskBackend::queryTask      (Type::Query              const&request, ResourceTaskFn_t &outTask)                                                 \
          {                                                                                                                                                                       \
            Log::Verbose(logTag(), String::format("queryTask<%0>(...)", #Type));                                                                                                  \
            outTask = [&, this] () -> GFXAPIResourceHandleAssignment                                                                                                              \
            {                                                                                                                                                                     \
              GFXAPIResourceHandleAssignment assignment ={ };                                                                                                                     \
              assignment.publicHandle   = 1;                                                                                                                                      \
              assignment.internalHandle = std::static_pointer_cast<void>(MakeSharedPointerType<int>());                                                                           \
              return assignment;                                                                                                                                                  \
            };                                                                                                                                                                    \
            return EEngineStatus::Ok;                                                                                                                                             \
          }

    Mock_DefineTaskBuilderModule(Texture);
    Mock_DefineTaskBuilderModule(ShaderResourceView);
    Mock_DefineTaskBuilderModule(RenderTargetView);
    Mock_DefineTaskBuilderModule(DepthStencilView);
    Mock_DefineTaskBuilderModule(DepthStencilState);
    Mock_DefineTaskBuilderModule(RasterizerState);
    Mock_DefineTaskBuilderModule(SwapChain);
    Mock_DefineTaskBuilderModule(SwapChainBuffer);

  }
}