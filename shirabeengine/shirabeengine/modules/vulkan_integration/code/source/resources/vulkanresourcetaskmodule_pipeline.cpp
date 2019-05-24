#include <graphicsapi/resources/types/pipeline.h>
#include <graphicsapi/resources/gfxapiresourcebackend.h>
#include "vulkan/vulkandevicecapabilities.h"
#include "vulkan/resources/vulkanresourcetaskbackend.h"
#include "vulkan/resources/types/vulkanpipelineresource.h"

namespace engine
{
    namespace vulkan
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult<> CVulkanResourceTaskBackend::fnPipelineCreationTask(
                CPipeline::CCreationRequest    const &aRequest,
                ResolvedDependencyCollection_t const &aDependencies,
                ResourceTaskFn_t                     &aOutTask)
        {
            SHIRABE_UNUSED(aDependencies);

            EEngineStatus status = EEngineStatus::Ok;

            CPipeline::SDescriptor const &desc = aRequest.resourceDescriptor();

            // Setup the stuff here...

            aOutTask = [=] () -> CEngineResult<SGFXAPIResourceHandleAssignment>
            {
                SVulkanPipelineResource *textureResource = new SVulkanPipelineResource();

                auto const deleter = [] (SVulkanPipelineResource const *p)
                {
                    if(p)
                    {
                        delete p;
                    }
                };

                SGFXAPIResourceHandleAssignment assignment ={ };
                assignment.publicResourceHandle   = desc.name;
                assignment.internalResourceHandle = CStdSharedPtr_t<SVulkanPipelineResource>(textureResource, deleter);

                return { EEngineStatus::Ok, assignment };
            };

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CVulkanResourceTaskBackend::fnPipelineUpdateTask(
                CPipeline::CUpdateRequest       const &aRequest,
                SGFXAPIResourceHandleAssignment const &aAssignment,
                ResolvedDependencyCollection_t  const &aDependencies,
                ResourceTaskFn_t                      &aOutTask)
        {
            SHIRABE_UNUSED(aRequest);
            SHIRABE_UNUSED(aAssignment);
            SHIRABE_UNUSED(aDependencies);
            SHIRABE_UNUSED(aOutTask);

            EEngineStatus status = EEngineStatus::Ok;

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CVulkanResourceTaskBackend::fnPipelineDestructionTask(
                CPipeline::CDestructionRequest  const &aRequest,
                SGFXAPIResourceHandleAssignment const &aAssignment,
                ResolvedDependencyCollection_t  const &aDependencies,
                ResourceTaskFn_t                      &aOutTask)
        {
            SHIRABE_UNUSED(aRequest);
            SHIRABE_UNUSED(aDependencies);

            EEngineStatus status = EEngineStatus::Ok;

            aOutTask = [=] () -> CEngineResult<SGFXAPIResourceHandleAssignment>
            {
                CStdSharedPtr_t<SVulkanPipelineResource> pipeline = std::static_pointer_cast<SVulkanPipelineResource>(aAssignment.internalResourceHandle);
                if(nullptr == pipeline)
                {
                    CLog::Error(logTag(), CString::format("Invalid internal data provided for pipeline destruction. Vulkan error: %0", VkResult::VK_ERROR_INVALID_EXTERNAL_HANDLE));
                    return { EEngineStatus::Error };
                }

                // Destory stuff here...

                SGFXAPIResourceHandleAssignment assignment = aAssignment;
                assignment.internalResourceHandle = nullptr;

                return { EEngineStatus::Ok, assignment };
            };

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CVulkanResourceTaskBackend::fnPipelineQueryTask(
                CPipeline::CQuery               const &aRequest,
                SGFXAPIResourceHandleAssignment const &aAssignment,
                ResourceTaskFn_t                      &aOutTask)
        {
            SHIRABE_UNUSED(aRequest);
            SHIRABE_UNUSED(aAssignment);
            SHIRABE_UNUSED(aOutTask);

            EEngineStatus status = EEngineStatus::Ok;

            return { status };
        }
        //<-----------------------------------------------------------------------------
    }
}
