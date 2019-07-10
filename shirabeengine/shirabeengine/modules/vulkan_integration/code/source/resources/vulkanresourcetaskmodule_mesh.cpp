#include <string>
#include <istream
#include <fx/gltf.h>
#include <graphicsapi/resources/types/mesh.h>
#include <graphicsapi/resources/gfxapiresourcebackend.h>
#include "vulkan/vulkandevicecapabilities.h"
#include "vulkan/resources/vulkanresourcetaskbackend.h"
#include "vulkan/resources/types/vulkanmeshresource.h"

namespace engine
{
    namespace vulkan
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        fx::gltf::Document const __loadFromData(ByteBuffer const &aInputBuffer)
        {
            try
            {
                nlohmann::json json;
                {
                    std::string const  data = std::string(reinterpret_cast<char const*>(aInputBuffer.data()));
                    std::istringstream stream(data);
                    stream >> json;
                }

                return fx::gltf::detail::Create(json, { detail::GetDocumentRootPath(documentFilePath), readQuotas });
            }
            catch (invalid_gltf_document &)
            {
                throw;
            }
            catch (std::system_error &)
            {
                throw;
            }
            catch (...)
            {
                std::throw_with_nested(invalid_gltf_document("Invalid glTF document. See nested exception for details."));
            }
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        struct SBufferInfo
        {
            fx::gltf::Accessor const *accessor;

            uint8_t const *data;
            uint32_t       dataStride;
            uint32_t       totalSize;

            bool hasData() const noexcept
            {
                return (nullptr != data);
            }
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        static uint32_t __calculateDataTypeSize(fx::gltf::Accessor const &aAccessor) noexcept
        {
            uint32_t elementSize = 0;
            switch (aAccessor.componentType)
            {
                case fx::gltf::Accessor::ComponentType::Byte:
                case fx::gltf::Accessor::ComponentType::UnsignedByte:
                    elementSize = 1;
                    break;
                case fx::gltf::Accessor::ComponentType::Short:
                case fx::gltf::Accessor::ComponentType::UnsignedShort:
                    elementSize = 2;
                    break;
                case fx::gltf::Accessor::ComponentType::Float:
                case fx::gltf::Accessor::ComponentType::UnsignedInt:
                    elementSize = 4;
                    break;
            }

            switch (aAccessor.type)
            {
                case fx::gltf::Accessor::Type::Mat2:
                    return (4 * elementSize);
                case fx::gltf::Accessor::Type::Mat3:
                    return (9 * elementSize);
                case fx::gltf::Accessor::Type::Mat4:
                    return (16 * elementSize);
                case fx::gltf::Accessor::Type::Scalar:
                    return (elementSize);
                case fx::gltf::Accessor::Type::Vec2:
                    return (2 * elementSize);
                case fx::gltf::Accessor::Type::Vec3:
                    return (3 * elementSize);
                case fx::gltf::Accessor::Type::Vec4:
                    return (4 * elementSize);
            }

            return 0;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        static SBufferInfo __getData(  fx::gltf::Document const &aDocument
                                     , fx::gltf::Accessor const &aAccessor)
        {
            fx::gltf::BufferView const &bufferView = aDocument.bufferViews[aAccessor.bufferView];
            fx::gltf::Buffer     const &buffer     = aDocument.buffers    [bufferView.buffer];

            uint32_t const dataTypeSize = __calculateDataTypeSize(aAccessor);
            return SBufferInfo
                   {
                       &(aAccessor)
                       , &(buffer.data[static_cast<uint64_t>(bufferView.byteOffset) + aAccessor.byteOffset])
                       , dataTypeSize
                       , (aAccessor.count * dataTypeSize)
                   };
        }
        //<-----------------------------------------------------------------------------

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

                // The buffer data contains the binary/text data of a .gltf-file.
                // Use fx-gltf to load the mesh data.
                fx::gltf::Document const document = __loadFromData(data);
                for(auto const &mesh : document.meshes)
                {
                    for(auto const &primitive : mesh.primitives)
                    {
                        SBufferInfo positionBufferInfo {};
                        SBufferInfo normalBufferInfo   {};
                        SBufferInfo tangentBufferInfo  {};
                        SBufferInfo texcoordBufferInfo {};
                        SBufferInfo indexBufferInfo    {};

                        for(auto const &[attributeID, attributeIndex] : primitive.attributes)
                        {
                            if("POSITION" == attributeID)
                            {
                                positionBufferInfo = __getData(document, document.accessors[attributeIndex]);
                                continue;
                            }

                            if("NORMAL" == attributeID)
                            {
                                normalBufferInfo = __getData(document, document.accessors[attributeIndex]);
                                continue;
                            }

                            if("TANGENT" == attributeID)
                            {
                                tangentBufferInfo = __getData(document, document.accessors[attributeIndex]);
                                continue;
                            }

                            if("TEXCOORD_0" == attributeID)
                            {
                                texcoordBufferInfo = __getData(document, document.accessors[attributeIndex]);
                                continue;
                            }
                        }

                        indexBufferInfo = __getData(document, document.accessors[primitive.indices]);

                        // We got all buffers... Generate Vk-Structs out of it...
                        auto const createBufferFn = [&] (SBufferInfo const &aBufferInfo) -> CEngineResult<std::tuple<VkBuffer, VkDeviceMemory>>
                        {
                            VkBufferCreateInfo bufferCreateInfo {};
                            bufferCreateInfo.sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                            bufferCreateInfo.pNext                 = nullptr;
                            bufferCreateInfo.flags                 = 0;
                            bufferCreateInfo.usage                 = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
                            bufferCreateInfo.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
                            bufferCreateInfo.size                  = aBufferInfo.totalSize;
                            // bufferCreateInfo.pQueueFamilyIndices   = ;
                            // bufferCreateInfo.queueFamilyIndexCount = ;

                            VkBuffer buffer = VK_NULL_HANDLE;

                            VkDevice         const &vkLogicalDevice  = mVulkanEnvironment->getState().selectedLogicalDevice;
                            VkPhysicalDevice const &vkPhysicalDevice = mVulkanEnvironment->getState().supportedPhysicalDevices.at(mVulkanEnvironment->getState().selectedPhysicalDevice).handle;

                            VkResult result = vkCreateBuffer(vkLogicalDevice, &bufferCreateInfo, nullptr, &buffer);
                            if(VkResult::VK_SUCCESS != result)
                            {
                                CLog::Error(logTag(), CString::format("Failed to create buffer. Vulkan result: %0", result));
                                return { EEngineStatus::Error };
                            }

                            VkMemoryRequirements vkMemoryRequirements ={ };
                            vkGetBufferMemoryRequirements(vkLogicalDevice, buffer, &vkMemoryRequirements);

                            VkMemoryAllocateInfo vkMemoryAllocateInfo ={ };
                            vkMemoryAllocateInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                            vkMemoryAllocateInfo.allocationSize  = vkMemoryRequirements.size;

                            CEngineResult<uint32_t> memoryTypeFetch =
                                                            CVulkanDeviceCapsHelper::determineMemoryType(
                                                                    vkPhysicalDevice,
                                                                    vkMemoryRequirements.memoryTypeBits,
                                                                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

                            if(not memoryTypeFetch.successful())
                            {
                                CLog::Error(logTag(), "Could not determine memory type index.");
                                return { EEngineStatus::Error };
                            }

                            vkMemoryAllocateInfo.memoryTypeIndex = memoryTypeFetch.data();

                            VkDeviceMemory bufferMemory = VK_NULL_HANDLE;

                            result = vkAllocateMemory(vkLogicalDevice, &vkMemoryAllocateInfo, nullptr, &bufferMemory);
                            if(VkResult::VK_SUCCESS != result)
                            {
                                CLog::Error(logTag(), CString::format("Failed to allocate image memory on GPU. Vulkan error: %0", result));
                                return { EEngineStatus::Error };
                            }

                            result = vkBindBufferMemory(vkLogicalDevice, buffer, bufferMemory, 0);
                            if(VkResult::VK_SUCCESS != result)
                            {
                                CLog::Error(logTag(), CString::format("Failed to bind image memory on GPU. Vulkan error: %0", result));
                                return { EEngineStatus::Error };
                            }

                            // Copy vertex data over...
                            void *data = nullptr;
                            vkMapMemory(vkLogicalDevice, bufferMemory, 0, bufferCreateInfo.size, 0, &(data) );
                            memcpy(data, aBufferInfo.data, static_cast<std::size_t>(aBufferInfo.totalSize));
                            vkUnmapMemory(vkLogicalDevice, bufferMemory);

                            return { EEngineStatus::Ok, { buffer, bufferMemory } };
                        };

                        auto position  = createBufferFn(positionBufferInfo);
                        auto normal    = createBufferFn(normalBufferInfo);
                        auto tangent   = createBufferFn(tangentBufferInfo);
                        auto texcoord0 = createBufferFn(texcoordBufferInfo);
                        auto index     = createBufferFn(indexBufferInfo);

                        // All components are required for compatibility...
                        if(not (   position .successful()
                                && normal   .successful()
                                && tangent  .successful()
                                && texcoord0.successful()
                                && index    .successful()))
                        {
                            return { EEngineStatus::Error };
                        }

                        vertexBuffers.push_back(std::get<0>(position .data()));
                        vertexBuffers.push_back(std::get<0>(normal   .data()));
                        vertexBuffers.push_back(std::get<0>(tangent  .data()));
                        vertexBuffers.push_back(std::get<0>(texcoord0.data()));
                        vertexBufferMemoryObjects.push_back(std::get<1>(position .data()));
                        vertexBufferMemoryObjects.push_back(std::get<1>(normal   .data()));
                        vertexBufferMemoryObjects.push_back(std::get<1>(tangent  .data()));
                        vertexBufferMemoryObjects.push_back(std::get<1>(texcoord0.data()));
                        indexBuffer       = std::get<0>(index.data());
                        indexBufferMemory = std::get<1>(index.data());

                        break; // For now, just process the first...
                    }

                    break; // For now, just process the first...
                }

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
                assignment.internalResourceHandle = CStdSharedPtr_t<SVulkanMeshResource>(textureResource, deleter);

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
                CStdSharedPtr_t<SVulkanTextureResource> texture = std::static_pointer_cast<SVulkanTextureResource>(aAssignment.internalResourceHandle);
                if(nullptr == texture)
                {
                    CLog::Error(logTag(), CString::format("Invalid internal data provided for texture destruction. Vulkan error: %0", VkResult::VK_ERROR_INVALID_EXTERNAL_HANDLE));
                    return { EEngineStatus::Error };
                }

                VkImage        vkImage         = texture->handle;
                VkDeviceMemory vkDeviceMemory  = texture->attachedMemory;
                VkDevice       vkLogicalDevice = mVulkanEnvironment->getState().selectedLogicalDevice;

                vkFreeMemory(vkLogicalDevice, vkDeviceMemory, nullptr);
                vkDestroyImage(vkLogicalDevice, vkImage, nullptr);

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
