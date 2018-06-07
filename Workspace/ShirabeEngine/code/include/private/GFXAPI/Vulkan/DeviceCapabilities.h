#ifndef __SHIRABE_VULKAN_DEVICECAPABILITIES_H__
#define __SHIRABE_VULKAN_DEVICECAPABILITIES_H__

#include <vulkan/vulkan.h>

#include "Core/BasicTypes.h"
#include "Core/EngineStatus.h"
#include "Core/EngineTypeHelper.h"
#include "Core/BitField.h"
#include "Log/Log.h"

#include "Resources/Subsystems/GFXAPI/Types/Definition.h"

#include "GFXAPI/Capabilities.h"


namespace Engine {
  namespace Vulkan {

    using namespace GFXAPI;
    using Engine::Resources::Format;
    using Engine::Core::BitField;

    class VulkanDeviceCapsHelper {
      DeclareLogTag(VulkanDeviceCapsHelper)
    public:

      static VkFormat convertFormatToVk(const Format& fmt);
      static Format   convertFormatFromVk(const VkFormat& fmt);

    };

  }
}

#endif