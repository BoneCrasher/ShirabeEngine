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
        #define ImplementTasksFor(Type)                                             \
            addCreator<Type>(                                                       \
                    std::bind(                                                      \
                            &CVulkanResourceTaskBackend::fn##Type##CreationTask,    \
                            this,                                                   \
                            std::placeholders::_1,                                  \
                            std::placeholders::_2,                                  \
                            std::placeholders::_3));                                \
            addUpdater<Type>(                                                       \
                    std::bind(                                                      \
                            &CVulkanResourceTaskBackend::fn##Type##UpdateTask,      \
                            this,                                                   \
                            std::placeholders::_1,                                  \
                            std::placeholders::_2,                                  \
                            std::placeholders::_3,                                  \
                            std::placeholders::_4));                                \
            addDestructor<Type>(                                                    \
                    std::bind                                                       \
                            &CVulkanResourceTaskBackend::fn##Type##DestructionTask, \
                            this,                                                   \
                            std::placeholders::_1,                                  \
                            std::placeholders::_2,                                  \
                            std::placeholders::_3,                                  \
                            std::placeholders::_4));                                \
            addQuery<Type>(                                                         \
                    std::bind(                                                      \
                            &CVulkanResourceTaskBackend::fn##Type##QueryTask,       \
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
            ImplementTasksFor(CTexture);
            ImplementTasksFor(CTextureView);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        void CVulkanResourceTaskBackend::deinitialize()
        {
            ImplementTasksFor(CTexture);
            ImplementTasksFor(CTextureView);
        }
        //<-----------------------------------------------------------------------------
    }
}
