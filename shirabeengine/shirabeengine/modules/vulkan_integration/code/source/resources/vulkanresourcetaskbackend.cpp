#include "vulkan/resources/vulkanresourcetaskbackend.h"

namespace engine
{
    namespace vulkan
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CVulkanResourceTaskBackend::CVulkanResourceTaskBackend(CStdSharedPtr_t<CVulkanEnvironment> const &aVulkanEnvironment)
            : CGFXAPIResourceTaskBackend()
            , mVulkanEnvironment(aVulkanEnvironment)
        {
            assert(mVulkanEnvironment != nullptr);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        #define SHIRABE_IMPLEMENT_TASKS_FOR(type)                                   \
            addCreator<C##type>(                                                    \
                    std::bind(                                                      \
                            &CVulkanResourceTaskBackend::fn##type##CreationTask,    \
                            this,                                                   \
                            std::placeholders::_1,                                  \
                            std::placeholders::_2,                                  \
                            std::placeholders::_3));                                \
            addUpdater<C##type>(                                                    \
                    std::bind(                                                      \
                            &CVulkanResourceTaskBackend::fn##type##UpdateTask,      \
                            this,                                                   \
                            std::placeholders::_1,                                  \
                            std::placeholders::_2,                                  \
                            std::placeholders::_3,                                  \
                            std::placeholders::_4));                                \
            addDestructor<C##type>(                                                 \
                    std::bind(                                                      \
                            &CVulkanResourceTaskBackend::fn##type##DestructionTask, \
                            this,                                                   \
                            std::placeholders::_1,                                  \
                            std::placeholders::_2,                                  \
                            std::placeholders::_3,                                  \
                            std::placeholders::_4));                                \
            addQuery<C##type>(                                                      \
                    std::bind(                                                      \
                            &CVulkanResourceTaskBackend::fn##type##QueryTask,       \
                            this,                                                   \
                            std::placeholders::_1,                                  \
                            std::placeholders::_2,                                  \
                            std::placeholders::_3));
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CVulkanResourceTaskBackend::initialize()
        {
            SHIRABE_IMPLEMENT_TASKS_FOR(Texture)
            SHIRABE_IMPLEMENT_TASKS_FOR(TextureView);
            SHIRABE_IMPLEMENT_TASKS_FOR(RenderPass);
            SHIRABE_IMPLEMENT_TASKS_FOR(FrameBuffer);

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CVulkanResourceTaskBackend::deinitialize()
        {
            // SHIRABE_IMPLEMENT_TASKS_FOR(Texture);
            // SHIRABE_IMPLEMENT_TASKS_FOR(TextureView);
            // SHIRABE_IMPLEMENT_TASKS_FOR(RenderPass);

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------
    }
}
