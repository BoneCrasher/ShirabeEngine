#include "GFXAPI/Vulkan/Resources/ResourceTaskBackend.h"

namespace Engine {
  namespace Vulkan {

    VulkanResourceTaskBackend::VulkanResourceTaskBackend(
      Ptr<VulkanEnvironment> const& vulkanEnvironment)
      : GFXAPIResourceTaskBackend()
      , m_vulkanEnvironment(vulkanEnvironment)
    {
      assert(vulkanEnvironment != nullptr);
    }

    void VulkanResourceTaskBackend::initialize() {
      addCreator<Texture>(std::bind(&VulkanResourceTaskBackend::fnTextureCreationTask, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
      addUpdater<Texture>(std::bind(&VulkanResourceTaskBackend::fnTextureUpdateTask, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
      addDestructor<Texture>(std::bind(&VulkanResourceTaskBackend::fnTextureDestructionTask, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
      addQuery<Texture>(std::bind(&VulkanResourceTaskBackend::fnTextureQueryTask, this, std::placeholders::_1, std::placeholders::_2));
    }
  }
}
