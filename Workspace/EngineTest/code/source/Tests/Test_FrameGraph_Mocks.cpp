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

    struct Dummy {};

    #define Mock_DefineTaskBuilderModule(Type)                                                                                                                  \
          EEngineStatus MockGFXAPITaskBackend::fn##Type##CreationTask(Type::CreationRequest const&request, ResolvedDependencyCollection const&depencies, ResourceTaskFn_t &outTask)   \
          {                                                                                                                                                                       \
            Log::Verbose(logTag(), String::format("creationTask<%0>(...)", #Type));                                                                                               \
            outTask = [&, this] () -> GFXAPIResourceHandleAssignment                                                                                                              \
            {                                                                                                                                                                     \
              GFXAPIResourceHandleAssignment assignment ={ };                                                                                                                     \
              assignment.publicHandle   = 1;                                                                                                                                      \
              assignment.internalHandle = Ptr<Dummy>(new Dummy());                                                                                                                \
              return assignment;                                                                                                                                                  \
            };                                                                                                                                                                    \
            return EEngineStatus::Ok;                                                                                                                                             \
          }                                                                                                                                                                       \
          EEngineStatus MockGFXAPITaskBackend::fn##Type##UpdateTask(Type::UpdateRequest const&request, GFXAPIResourceHandleAssignment const&, ResolvedDependencyCollection const&depencies, ResourceTaskFn_t &outTask)   \
          {                                                                                                                                                                       \
            Log::Verbose(logTag(), String::format("updateTask<%0>(...)", #Type));                                                                                                 \
            outTask = [&, this] () -> GFXAPIResourceHandleAssignment                                                                                                              \
            {                                                                                                                                                                     \
              GFXAPIResourceHandleAssignment assignment ={ };                                                                                                                     \
              assignment.publicHandle   = 1;                                                                                                                                      \
              assignment.internalHandle = Ptr<Dummy>(new Dummy());                                                                                                                \
              return assignment;                                                                                                                                                  \
            };                                                                                                                                                                    \
            return EEngineStatus::Ok;                                                                                                                                             \
          }                                                                                                                                                                       \
          EEngineStatus MockGFXAPITaskBackend::fn##Type##DestructionTask(Type::DestructionRequest const&request, GFXAPIResourceHandleAssignment const&, ResolvedDependencyCollection const&depencies, ResourceTaskFn_t &outTask)   \
          {                                                                                                                                                                       \
            Log::Verbose(logTag(), String::format("destructionTask<%0>(...)", #Type));                                                                                            \
            outTask = [&, this] () -> GFXAPIResourceHandleAssignment                                                                                                              \
            {                                                                                                                                                                     \
              GFXAPIResourceHandleAssignment assignment ={ };                                                                                                                     \
              assignment.publicHandle   = 1;                                                                                                                                      \
              assignment.internalHandle = Ptr<Dummy>(new Dummy());                                                                                                                \
              return assignment;                                                                                                                                                  \
            };                                                                                                                                                                    \
            return EEngineStatus::Ok;                                                                                                                                             \
          }                                                                                                                                                                       \
          EEngineStatus MockGFXAPITaskBackend::fn##Type##QueryTask(Type::Query const&request, GFXAPIResourceHandleAssignment const&, ResourceTaskFn_t &outTask)                                                 \
          {                                                                                                                                                                       \
            Log::Verbose(logTag(), String::format("queryTask<%0>(...)", #Type));                                                                                                  \
            outTask = [&, this] () -> GFXAPIResourceHandleAssignment                                                                                                              \
            {                                                                                                                                                                     \
              GFXAPIResourceHandleAssignment assignment ={ };                                                                                                                     \
              assignment.publicHandle   = 1;                                                                                                                                      \
              assignment.internalHandle = Ptr<Dummy>(new Dummy());                                                                                                                \
              return assignment;                                                                                                                                                  \
            };                                                                                                                                                                    \
            return EEngineStatus::Ok;                                                                                                                                             \
          }

    #define Mock_AddFunctionsForType(Type) \
      addCreator   <Type>(std::bind(&MockGFXAPITaskBackend::fn##Type##CreationTask,    this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));   \
      addUpdater   <Type>(std::bind(&MockGFXAPITaskBackend::fn##Type##UpdateTask,      this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));   \
      addDestructor<Type>(std::bind(&MockGFXAPITaskBackend::fn##Type##DestructionTask, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));   \
      addQuery     <Type>(std::bind(&MockGFXAPITaskBackend::fn##Type##QueryTask,       this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    void MockGFXAPITaskBackend::initialize()
    {
      Mock_AddFunctionsForType(Texture);
      Mock_AddFunctionsForType(TextureView);
      Mock_AddFunctionsForType(DepthStencilState);
      Mock_AddFunctionsForType(RasterizerState);
      Mock_AddFunctionsForType(SwapChain);
      Mock_AddFunctionsForType(SwapChainBuffer);
    }

    Mock_DefineTaskBuilderModule(Texture);
    Mock_DefineTaskBuilderModule(TextureView);
    Mock_DefineTaskBuilderModule(DepthStencilState);
    Mock_DefineTaskBuilderModule(RasterizerState);
    Mock_DefineTaskBuilderModule(SwapChain);
    Mock_DefineTaskBuilderModule(SwapChainBuffer);

  }
}