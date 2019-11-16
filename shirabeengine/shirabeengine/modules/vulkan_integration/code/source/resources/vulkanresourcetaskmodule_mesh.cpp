#include <string>
#include <istream>
#include <resources/resourcetypes.h>
#include "vulkan_integration/vulkandevicecapabilities.h"
#include "vulkan_integration/resources/vulkanresourcetaskbackend.h"
#include "vulkan_integration/resources/types/vulkanmeshresource.h"

namespace engine
{
    namespace vulkan
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult<> CVulkanResourceTaskBackend::fnMeshCreationTask(
                CMesh::CCreationRequest        const &aRequest,
                ResolvedDependencyCollection_t const &aDependencies,
                ResourceTaskFn_t                     &aOutTask)
        {
            SHIRABE_UNUSED(aDependencies);

            EEngineStatus status = EEngineStatus::Ok;

            CMesh::SDescriptor const &desc = aRequest.resourceDescriptor();

            aOutTask = [=] () -> CEngineResult<SGFXAPIResourceHandleAssignment>
            {
                std::vector<VkBuffer>       vertexBuffers {};
                std::vector<VkDeviceMemory> vertexBufferMemoryObjects {};
                VkBuffer                    indexBuffer {};
                VkDeviceMemory              indexBufferMemory {};

                ByteBuffer const &data = desc.dataSourceAccessor();
                if(0 == data.size())
                {
                    return { EEngineStatus::Error };
                }

                // ...

                SVulkanMeshResource *textureResource = new SVulkanMeshResource();
                textureResource->vertexBuffer       = vertexBuffers;
                textureResource->vertexBufferMemory = vertexBufferMemoryObjects;
                textureResource->indexBuffer        = indexBuffer;
                textureResource->indexBufferMemory  = indexBufferMemory;

                auto const deleter = [] (SVulkanMeshResource const *p)
                {
                    delete p;
                };

                SGFXAPIResourceHandleAssignment assignment ={ };
                assignment.publicResourceHandle   = desc.name;
                assignment.internalResourceHandle = Shared<SVulkanMeshResource>(textureResource, deleter);

                return { EEngineStatus::Ok, assignment };
            };

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CVulkanResourceTaskBackend::fnMeshUpdateTask(
                CMesh::CUpdateRequest        const &aRequest,
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
        CEngineResult<> CVulkanResourceTaskBackend::fnMeshDestructionTask(
                CMesh::CDestructionRequest   const &aRequest,
                SGFXAPIResourceHandleAssignment const &aAssignment,
                ResolvedDependencyCollection_t  const &aDependencies,
                ResourceTaskFn_t                      &aOutTask)
        {
            SHIRABE_UNUSED(aRequest);
            SHIRABE_UNUSED(aDependencies);

            EEngineStatus status = EEngineStatus::Ok;

            aOutTask = [=] () -> CEngineResult<SGFXAPIResourceHandleAssignment>
            {
                Shared<SVulkanMeshResource> mesh = std::static_pointer_cast<SVulkanMeshResource>(aAssignment.internalResourceHandle);
                if(nullptr == mesh)
                {
                    CLog::Error(logTag(), CString::format("Invalid internal data provided for texture destruction. Vulkan error: {}", VkResult::VK_ERROR_INVALID_EXTERNAL_HANDLE));
                    return { EEngineStatus::Error };
                }

                VkDevice vkLogicalDevice = mVulkanEnvironment->getState().selectedLogicalDevice;

                vkFreeMemory(vkLogicalDevice, mesh->indexBufferMemory, nullptr);
                vkDestroyBuffer(vkLogicalDevice, mesh->indexBuffer, nullptr);

                for(auto const &memory : mesh->vertexBufferMemory)
                {
                    vkFreeMemory(vkLogicalDevice, memory, nullptr);
                }
                for(auto const &buffer : mesh->vertexBuffer)
                {
                    vkDestroyBuffer(vkLogicalDevice, buffer, nullptr);
                }

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
        CEngineResult<> CVulkanResourceTaskBackend::fnMeshQueryTask(
                CMesh::CQuery                const &aRequest,
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
