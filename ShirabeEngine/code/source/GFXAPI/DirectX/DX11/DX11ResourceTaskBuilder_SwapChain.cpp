#include "GFXAPI/DirectX/DX11/DX11ResourceTaskBuilder.h"

#include "GFXAPI/DirectX/DX11/DX11Common.h"
#include "GFXAPI/DirectX/DX11/DX11Linkage.h"
#include "GFXAPI/DirectX/DX11/DX11Types.h"
#include "GFXAPI/DirectX/DX11/DX11DeviceCapabilities.h"

namespace Engine {
  namespace DX {
    namespace _11 {

      EEngineStatus DX11ResourceTaskBuilder
        ::creationTask(
          SwapChain::CreationRequest   const& request,
          ResolvedDependencyCollection const&resolvedDependencies,
          ResourceTaskFn_t                  &outTask)
      {
        EEngineStatus status = EEngineStatus::Ok;

        return status;
      }

      EEngineStatus DX11ResourceTaskBuilder::updateTask(
        SwapChain::UpdateRequest     const&request,
        ResolvedDependencyCollection const&resolvedDependencies,
        ResourceTaskFn_t                  &outTask)
      {
        EEngineStatus status = EEngineStatus::Ok;

        return status;
      }

      EEngineStatus DX11ResourceTaskBuilder::destructionTask(
        SwapChain::DestructionRequest const&request,
        ResolvedDependencyCollection  const&resolvedDependencies,
        ResourceTaskFn_t                   &outTask)
      {
        EEngineStatus status = EEngineStatus::Ok;

        return status;
      }

      EEngineStatus DX11ResourceTaskBuilder::queryTask(
        SwapChain::Query const&request,
        ResourceTaskFn_t      &outTask)
      {
        EEngineStatus status = EEngineStatus::Ok;

        return status;
      }

    }
  }
}
