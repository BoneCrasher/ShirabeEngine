#include "GFXAPI/DirectX/DX11/DX11ResourceTaskBuilder.h"


namespace Engine {
  namespace GFXAPI {
    using namespace Engine::Resources;
    using namespace Engine::DX::_11;

    EEngineStatus
      GFXAPIResourceTaskBackendImpl<DepthStencilView>::
      creationTask(
        DepthStencilView::CreationRequest   const&request,
        ResolvedDependencyCollection        const&resolvedDependencies,
        ResourceTaskFn_t                         &outTask)
    {
      EEngineStatus status = EEngineStatus::Ok;

      return status;
    }

    EEngineStatus
      GFXAPIResourceTaskBackendImpl<DepthStencilView>::
      updateTask(
        DepthStencilView::UpdateRequest     const&request,
        ResolvedDependencyCollection        const&resolvedDependencies,
        ResourceTaskFn_t                         &outTask)
    {
      EEngineStatus status = EEngineStatus::Ok;

      return status;
    }

    EEngineStatus
      GFXAPIResourceTaskBackendImpl<DepthStencilView>::
      destructionTask(
        DepthStencilView::DestructionRequest const&request,
        ResolvedDependencyCollection         const&resolvedDependencies,
        ResourceTaskFn_t                          &outTask)
    {
      EEngineStatus status = EEngineStatus::Ok;

      return status;
    }

    EEngineStatus
      GFXAPIResourceTaskBackendImpl<DepthStencilView>::
      queryTask(
        DepthStencilView::Query const&request,
        ResourceTaskFn_t             &outTask)
    {
      EEngineStatus status = EEngineStatus::Ok;

      return status;
    }

  }
}
