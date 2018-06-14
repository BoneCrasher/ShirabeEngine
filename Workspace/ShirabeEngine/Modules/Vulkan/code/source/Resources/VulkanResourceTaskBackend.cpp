#include "Vulkan/Resources/VulkanResourceTaskBackend.h"

namespace Engine {
  namespace Vulkan {

    VulkanResourceTaskBackend::VulkanResourceTaskBackend(
      Ptr<VulkanEnvironment> const& vulkanEnvironment)
      : GFXAPIResourceTaskBackend()
      , m_vulkanEnvironment(vulkanEnvironment)
    {
      assert(vulkanEnvironment != nullptr);
    }

    #define ImplementTasksFor(Type) \
      addCreator<Type>   (std::bind(&VulkanResourceTaskBackend::fn##Type##CreationTask,    this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));                        \
      addUpdater<Type>   (std::bind(&VulkanResourceTaskBackend::fn##Type##UpdateTask,      this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)); \
      addDestructor<Type>(std::bind(&VulkanResourceTaskBackend::fn##Type##DestructionTask, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)); \
      addQuery<Type>     (std::bind(&VulkanResourceTaskBackend::fn##Type##QueryTask,       this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    void VulkanResourceTaskBackend::initialize() {
      ImplementTasksFor(Texture);
      ImplementTasksFor(TextureView);
    }
  }
}
