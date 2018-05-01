#include "GFXAPI/DirectX/DX11/Resources/ResourceTaskBackend.h"


namespace Engine {
  namespace DX {
    namespace _11 {
      using namespace Engine::Resources;
      using namespace Engine::DX::_11;

      EEngineStatus
        DX11ResourceTaskBackend::
        creationTask(
          DepthStencilView::CreationRequest   const&request,
          ResolvedDependencyCollection        const&resolvedDependencies,
          ResourceTaskFn_t                         &outTask)
      {
        EEngineStatus status = EEngineStatus::Ok;

        return status;
      }

      EEngineStatus
        DX11ResourceTaskBackend::
        updateTask(
          DepthStencilView::UpdateRequest     const&request,
          ResolvedDependencyCollection        const&resolvedDependencies,
          ResourceTaskFn_t                         &outTask)
      {
        EEngineStatus status = EEngineStatus::Ok;

        return status;
      }

      EEngineStatus
        DX11ResourceTaskBackend::
        destructionTask(
          DepthStencilView::DestructionRequest const&request,
          ResolvedDependencyCollection         const&resolvedDependencies,
          ResourceTaskFn_t                          &outTask)
      {
        EEngineStatus status = EEngineStatus::Ok;

        return status;
      }

      EEngineStatus
        DX11ResourceTaskBackend::
        queryTask(
          DepthStencilView::Query const&request,
          ResourceTaskFn_t             &outTask)
      {
        EEngineStatus status = EEngineStatus::Ok;

        return status;
      }

    }
  }
}