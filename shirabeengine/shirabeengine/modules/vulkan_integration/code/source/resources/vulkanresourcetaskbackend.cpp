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
        void CVulkanResourceTaskBackend::initialize()
        {
            SHIRABE_IMPLEMENT_TASKS_FOR(Texture)
            SHIRABE_IMPLEMENT_TASKS_FOR(TextureView);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        void CVulkanResourceTaskBackend::deinitialize()
        {
            SHIRABE_IMPLEMENT_TASKS_FOR(Texture);
            SHIRABE_IMPLEMENT_TASKS_FOR(TextureView);
        }
        //<-----------------------------------------------------------------------------
    }
}
