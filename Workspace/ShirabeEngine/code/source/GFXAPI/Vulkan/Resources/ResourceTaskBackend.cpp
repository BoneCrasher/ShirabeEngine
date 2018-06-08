#include "GFXAPI/Vulkan/Resources/ResourceTaskBackend.h"

namespace Engine {
  namespace Vulkan {

    VulkanResourceTaskBackend::VulkanResourceTaskBackend(
      Ptr<VulkanEnvironment> const& vulkanEnvironment)
      : GFXAPIResourceTaskBackend<EngineTypes>()
      , m_vulkanEnvironment(vulkanEnvironment)
    {
      assert(vulkanEnvironment != nullptr);
    }


  }
}
