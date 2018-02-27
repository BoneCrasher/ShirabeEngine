#include "GFXAPI/DirectX/DX11/DX11ResourceTaskBackend.h"

namespace Engine {
  namespace DX {
    namespace _11 {
      using namespace Engine::Resources;
      using namespace Engine::DX::_11;

      EEngineStatus
        DX11ResourceTaskBackend::
        creationTask(
          Texture1D::CreationRequest   const& request,
          ResolvedDependencyCollection const&resolvedDependencies,
          ResourceTaskFn_t                  &outTask)
      {
        EEngineStatus status = EEngineStatus::Ok;

        return status;
      }

      EEngineStatus
        DX11ResourceTaskBackend::
        updateTask(
          Texture1D::UpdateRequest     const&request,
          ResolvedDependencyCollection const&resolvedDependencies,
          ResourceTaskFn_t                  &outTask)
      {
        EEngineStatus status = EEngineStatus::Ok;

        return status;
      }

      EEngineStatus
        DX11ResourceTaskBackend::
        destructionTask(
          Texture1D::DestructionRequest const&request,
          ResolvedDependencyCollection  const&resolvedDependencies,
          ResourceTaskFn_t                   &outTask)
      {
        EEngineStatus status = EEngineStatus::Ok;

        return status;
      }

      EEngineStatus
        DX11ResourceTaskBackend::
        queryTask(
          Texture1D::Query const&request,
          ResourceTaskFn_t      &outTask)
      {
        EEngineStatus status = EEngineStatus::Ok;

        return status;
      }

      EEngineStatus
        DX11ResourceTaskBackend::
        creationTask(
          Texture2D::CreationRequest   const& request,
          ResolvedDependencyCollection const&resolvedDependencies,
          ResourceTaskFn_t                  &outTask)
      {
        EEngineStatus status = EEngineStatus::Ok;

        return status;
      }

      EEngineStatus
        DX11ResourceTaskBackend::
        updateTask(
          Texture2D::UpdateRequest     const&request,
          ResolvedDependencyCollection const&resolvedDependencies,
          ResourceTaskFn_t                  &outTask)
      {
        EEngineStatus status = EEngineStatus::Ok;

        return status;
      }

      EEngineStatus
        DX11ResourceTaskBackend::
        destructionTask(
          Texture2D::DestructionRequest const&request,
          ResolvedDependencyCollection  const&resolvedDependencies,
          ResourceTaskFn_t                   &outTask)
      {
        EEngineStatus status = EEngineStatus::Ok;

        return status;
      }

      EEngineStatus
        DX11ResourceTaskBackend::
        queryTask(
          Texture2D::Query const&request,
          ResourceTaskFn_t      &outTask)
      {
        EEngineStatus status = EEngineStatus::Ok;

        return status;
      }

      EEngineStatus
        DX11ResourceTaskBackend::
        creationTask(
          Texture3D::CreationRequest   const& request,
          ResolvedDependencyCollection const&resolvedDependencies,
          ResourceTaskFn_t                  &outTask)
      {
        EEngineStatus status = EEngineStatus::Ok;

        return status;
      }

      EEngineStatus
        DX11ResourceTaskBackend::
        updateTask(
          Texture3D::UpdateRequest     const&request,
          ResolvedDependencyCollection const&resolvedDependencies,
          ResourceTaskFn_t                  &outTask)
      {
        EEngineStatus status = EEngineStatus::Ok;

        return status;
      }

      EEngineStatus
        DX11ResourceTaskBackend::
        destructionTask(
          Texture3D::DestructionRequest const&request,
          ResolvedDependencyCollection  const&resolvedDependencies,
          ResourceTaskFn_t                   &outTask)
      {
        EEngineStatus status = EEngineStatus::Ok;

        return status;
      }

      EEngineStatus
        DX11ResourceTaskBackend::
        queryTask(
          Texture3D::Query const&request,
          ResourceTaskFn_t      &outTask)
      {
        EEngineStatus status = EEngineStatus::Ok;

        return status;
      }
    }
  }
}