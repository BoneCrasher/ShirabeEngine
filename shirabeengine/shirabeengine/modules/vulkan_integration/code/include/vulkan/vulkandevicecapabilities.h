#ifndef __SHIRABE_VULKAN_DEVICECAPABILITIES_H__
#define __SHIRABE_VULKAN_DEVICECAPABILITIES_H__

#include <vulkan/vulkan.h>

#include <core/basictypes.h>
#include <core/enginestatus.h>
#include <core/enginetypehelper.h>
#include <core/bitfield.h>
#include <log/log.h>
#include <graphicsapi/resources/types/definition.h>

namespace engine
{
    namespace vulkan
    {
        using engine::resources::EFormat;
        using engine::core::CBitField;

        /**
         * Wrapper-class around several static helper functions related to vulkan API.
         */
        class CVulkanDeviceCapsHelper
        {
            SHIRABE_DECLARE_LOG_TAG(VulkanDeviceCapsHelper);

        public_static_functions:
            /**
             * Convert an engine internal format to the suitable vulkan format.
             *
             * @param aFormat Engine internal format value.
             * @return        Vulkan-format value.
             */
            static VkFormat convertFormatToVk(EFormat const &aFormat);

            /**
             * Convert a vulkan format to the suitable engine internal format.
             *
             * @param aFormat Vulkan-format value.
             * @return        Engine internal format value.
             */
            static EFormat convertFormatFromVk(VkFormat const &aFormat);

            /**
             * Determine an appropriate memory type index on a specific physical device
             * by an externally provided memory type value and memory property flags.
             *
             * @param aPhysicalDevice The physical device on which a memory type index should be determined.
             * @param aRequiredType   The required memory type to look for.
             * @param aProperties     Additional property type flags to be matched by the lookup.
             * @return
             */
            static CEngineResult<uint32_t> determineMemoryType(
                    VkPhysicalDevice      const &aPhysicalDevice,
                    uint32_t              const &aRequiredType,
                    VkMemoryPropertyFlags const &aProperties)
            {
                VkPhysicalDeviceMemoryProperties memProperties{};
                vkGetPhysicalDeviceMemoryProperties(aPhysicalDevice, &memProperties);

                for(uint32_t k = 0; k < memProperties.memoryTypeCount; ++k)
                {
                    uint32_t const typeFlag = (aRequiredType & (1 << k));
                    bool     const propFlag = ((memProperties.memoryTypes[k].propertyFlags & aProperties) == aProperties);

                    if(typeFlag && propFlag)
                    {
                        return { EEngineStatus::Ok, k };
                    }
                }

                return { EEngineStatus::Error };
            }
        };

    }
}

#endif
