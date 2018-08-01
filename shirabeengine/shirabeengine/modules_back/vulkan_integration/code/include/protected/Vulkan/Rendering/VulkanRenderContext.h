#ifndef __SHIRABE_VULKAN_RENDERCONTEXT_H__
#define __SHIRABE_VULKAN_RENDERCONTEXT_H__

#include "Log/Log.h"

#include "Renderer/IRenderer.h"

namespace Engine {
  namespace Vulkan {
    using namespace Engine::Rendering;

    class VulkanRenderContext
      : public IRenderContext
    {
      SHIRABE_DECLARE_LOG_TAG(VulkanRenderContext);
    public:
      EEngineStatus bindResource(PublicResourceId_t const&);
      EEngineStatus unbindResource(PublicResourceId_t const&);

      EEngineStatus render(Renderable const&);
    };

    EEngineStatus VulkanRenderContext::bindResource(PublicResourceId_t const&id) {
      Log::Verbose(logTag(), String::format("Binding resource with id %0", id));
      return EEngineStatus::Ok;
    }

    EEngineStatus VulkanRenderContext::unbindResource(PublicResourceId_t const&id) {
      Log::Verbose(logTag(), String::format("Unbinding resource with id %0", id));
      return EEngineStatus::Ok;
    }

    EEngineStatus VulkanRenderContext::render(Renderable const&renderable) {
      Log::Verbose(logTag(), String::format("Rendering renderable: %0", to_string(renderable)));
      return EEngineStatus::Ok;
    }
  }
}

#endif