#include "GFXAPI/DirectX/DX11/Resources/ResourceTaskBackend.h"

namespace Engine {
  namespace DX {
    namespace _11 {
      using namespace Engine::Resources;
      using namespace Engine::DX::_11;

      EEngineStatus
        DX11ResourceTaskBackend::
        creationTask(
          Texture::CreationRequest     const& request,
          ResolvedDependencyCollection const&resolvedDependencies,
          ResourceTaskFn_t                  &outTask)
      {
        EEngineStatus status = EEngineStatus::Ok;

        return status;
      }

      EEngineStatus
        DX11ResourceTaskBackend::
        updateTask(
          Texture::UpdateRequest       const&request,
          ResolvedDependencyCollection const&resolvedDependencies,
          ResourceTaskFn_t                  &outTask)
      {
        EEngineStatus status = EEngineStatus::Ok;

        return status;
      }

      EEngineStatus
        DX11ResourceTaskBackend::
        destructionTask(
          Texture::DestructionRequest   const&request,
          ResolvedDependencyCollection  const&resolvedDependencies,
          ResourceTaskFn_t                   &outTask)
      {
        EEngineStatus status = EEngineStatus::Ok;

        return status;
      }

      EEngineStatus
        DX11ResourceTaskBackend::
        queryTask(
          Texture::Query   const&request,
          ResourceTaskFn_t      &outTask)
      {
        EEngineStatus status = EEngineStatus::Ok;

        return status;
      }

    }
  }
}