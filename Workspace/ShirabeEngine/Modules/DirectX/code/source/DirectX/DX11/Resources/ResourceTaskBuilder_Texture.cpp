#include "GFXAPI/DirectX/DX11/Resources/ResourceTaskBackend.h"

namespace Engine {
  namespace DX {
    namespace _11 {
      using namespace Engine::Resources;
      using namespace Engine::DX::_11;

      //EEngineStatus
      //  DX11ResourceTaskBackend::
      //  creationTask(
      //    Texture::CreationRequest     const& request,
      //    ResolvedDependencyCollection const&resolvedDependencies,
      //    ResourceTaskFn_t                  &outTask)
      //{
      //  EEngineStatus status = EEngineStatus::Ok;
      //  
      //  Texture::Descriptor const&desc = request.resourceDescriptor();
      //  if(desc.textureInfo.depth > 1) {
      //    if(desc.textureInfo.arraySize > 1) {
      //      // 3D Texture Array
      //    }
      //    else {
      //      // 3D Texture
      //    }
      //  }
      //  else if(desc.textureInfo.height > 1) {
      //    if(desc.textureInfo.arraySize > 1) {
      //      // 2D Texture Array
      //    }
      //    else {
      //      // 2D Texture
      //    }
      //  }
      //  else {
      //    if(desc.textureInfo.arraySize > 1) {
      //      // 1D Texture Array
      //    }
      //    else {
      //      // 1D Texture
      //    }
      //  }

      //  return status;
      //}

      //EEngineStatus
      //  DX11ResourceTaskBackend::
      //  updateTask(
      //    Texture::UpdateRequest       const&request,
      //    ResolvedDependencyCollection const&resolvedDependencies,
      //    ResourceTaskFn_t                  &outTask)
      //{
      //  EEngineStatus status = EEngineStatus::Ok;

      //  return status;
      //}

      //EEngineStatus
      //  DX11ResourceTaskBackend::
      //  destructionTask(
      //    Texture::DestructionRequest   const&request,
      //    ResolvedDependencyCollection  const&resolvedDependencies,
      //    ResourceTaskFn_t                   &outTask)
      //{
      //  EEngineStatus status = EEngineStatus::Ok;

      //  return status;
      //}

      //EEngineStatus
      //  DX11ResourceTaskBackend::
      //  queryTask(
      //    Texture::Query   const&request,
      //    ResourceTaskFn_t      &outTask)
      //{
      //  EEngineStatus status = EEngineStatus::Ok;

      //  return status;
      //}

    }
  }
}