#include "GFXAPI/DirectX/DX11/DX11ResourceTaskBuilder.h"

namespace Engine {
  namespace GFXAPI {
    using namespace Engine::Resources;
    using namespace Engine::DX::_11;

    EEngineStatus DX11ResourceTaskBuilder
      ::creationTask(
        Texture1D::CreationRequest   const& request,
        ResolvedDependencyCollection const&resolvedDependencies,
        ResourceTaskFn_t                  &outTask)
    {
      EEngineStatus status = EEngineStatus::Ok;

      return status;
    }

    EEngineStatus DX11ResourceTaskBuilder::updateTask(
      Texture1D::UpdateRequest     const&request,
      ResolvedDependencyCollection const&resolvedDependencies,
      ResourceTaskFn_t                  &outTask)
    {
      EEngineStatus status = EEngineStatus::Ok;

      return status;
    }

    EEngineStatus DX11ResourceTaskBuilder::destructionTask(
      Texture1D::DestructionRequest const&request,
      ResolvedDependencyCollection  const&resolvedDependencies,
      ResourceTaskFn_t                   &outTask)
    {
      EEngineStatus status = EEngineStatus::Ok;

      return status;
    }

    EEngineStatus DX11ResourceTaskBuilder::queryTask(
      Texture1D::Query const&request,
      ResourceTaskFn_t      &outTask)
    {
      EEngineStatus status = EEngineStatus::Ok;

      return status;
    }

    EEngineStatus DX11ResourceTaskBuilder
      ::creationTask(
        Texture2D::CreationRequest   const& request,
        ResolvedDependencyCollection const&resolvedDependencies,
        ResourceTaskFn_t                  &outTask)
    {
      EEngineStatus status = EEngineStatus::Ok;

      return status;
    }

    EEngineStatus DX11ResourceTaskBuilder::updateTask(
      Texture2D::UpdateRequest     const&request,
      ResolvedDependencyCollection const&resolvedDependencies,
      ResourceTaskFn_t                  &outTask)
    {
      EEngineStatus status = EEngineStatus::Ok;

      return status;
    }

    EEngineStatus DX11ResourceTaskBuilder::destructionTask(
      Texture2D::DestructionRequest const&request,
      ResolvedDependencyCollection  const&resolvedDependencies,
      ResourceTaskFn_t                   &outTask)
    {
      EEngineStatus status = EEngineStatus::Ok;

      return status;
    }

    EEngineStatus DX11ResourceTaskBuilder::queryTask(
      Texture2D::Query const&request,
      ResourceTaskFn_t      &outTask)
    {
      EEngineStatus status = EEngineStatus::Ok;

      return status;
    }

    EEngineStatus DX11ResourceTaskBuilder
      ::creationTask(
        Texture3D::CreationRequest   const& request,
        ResolvedDependencyCollection const&resolvedDependencies,
        ResourceTaskFn_t                  &outTask)
    {
      EEngineStatus status = EEngineStatus::Ok;

      return status;
    }

    EEngineStatus DX11ResourceTaskBuilder::updateTask(
      Texture3D::UpdateRequest     const&request,
      ResolvedDependencyCollection const&resolvedDependencies,
      ResourceTaskFn_t                  &outTask)
    {
      EEngineStatus status = EEngineStatus::Ok;

      return status;
    }

    EEngineStatus DX11ResourceTaskBuilder::destructionTask(
      Texture3D::DestructionRequest const&request,
      ResolvedDependencyCollection  const&resolvedDependencies,
      ResourceTaskFn_t                   &outTask)
    {
      EEngineStatus status = EEngineStatus::Ok;

      return status;
    }

    EEngineStatus DX11ResourceTaskBuilder::queryTask(
      Texture3D::Query const&request,
      ResourceTaskFn_t      &outTask)
    {
      EEngineStatus status = EEngineStatus::Ok;

      return status;
    }
  }
}
