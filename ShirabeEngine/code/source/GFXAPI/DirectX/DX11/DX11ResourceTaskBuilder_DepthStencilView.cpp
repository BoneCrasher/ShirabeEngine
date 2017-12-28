#include "GFXAPI/DirectX/DX11/DX11ResourceTaskBuilder.h"

namespace Engine {
  namespace DX {
    namespace _11 {

      EEngineStatus DX11ResourceTaskBuilder
        ::creationTask(
          DepthStencilView::CreationRequest   const&request,
          ResolvedDependencyCollection        const&resolvedDependencies,
          ResourceTaskFn_t                         &outTask)
      {
        EEngineStatus status = EEngineStatus::Ok;

        return status;
      }

      EEngineStatus DX11ResourceTaskBuilder::updateTask(
        DepthStencilView::UpdateRequest     const&request,
        ResolvedDependencyCollection        const&resolvedDependencies,
        ResourceTaskFn_t                         &outTask)
      {
        EEngineStatus status = EEngineStatus::Ok;

        return status;
      }

      EEngineStatus DX11ResourceTaskBuilder::destructionTask(
        DepthStencilView::DestructionRequest const&request,
        ResolvedDependencyCollection         const&resolvedDependencies,
        ResourceTaskFn_t                          &outTask)
      {
        EEngineStatus status = EEngineStatus::Ok;

        return status;
      }

      EEngineStatus DX11ResourceTaskBuilder::queryTask(
        DepthStencilView::Query const&request,
        ResourceTaskFn_t             &outTask)
      {
        EEngineStatus status = EEngineStatus::Ok;

        return status;
      }

    }
  }
}
