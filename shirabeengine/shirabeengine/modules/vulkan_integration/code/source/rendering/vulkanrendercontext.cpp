#include "vulkan/rendering/vulkanrendercontext.h"

namespace engine
{
    namespace vulkan
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        EEngineStatus CVulkanRenderContext::bindResource(PublicResourceId_t const &aId)
        {
            CLog::Verbose(logTag(), CString::format("Binding resource with id %0", aId));
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CVulkanRenderContext::unbindResource(PublicResourceId_t const &aId)
        {
            CLog::Verbose(logTag(), CString::format("Unbinding resource with id %0", aId));
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CVulkanRenderContext::render(SRenderable const &aRenderable)
        {
            CLog::Verbose(logTag(), CString::format("Rendering renderable: %0", to_string(aRenderable)));
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------
    }
}
