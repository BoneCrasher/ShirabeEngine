#ifndef __SHIRABE_RESOURCES_TEXTURE_H__
#define __SHIRABE_RESOURCES_TEXTURE_H__

#include "Platform/Platform.h"
#include "Core/BasicTypes.h"
#include "Resources/Types/Definition.h"

namespace Engine {
  namespace Resources {

    struct Multisapmling {
      uint8_t size;
      uint8_t quality;
    };

    struct SHIRABE_TEST_EXPORT TextureInfo {
      uint32_t
        width,  // 0 - Undefined
        height, // At least 1
        depth;  // At least 1
      Format
        format;
      uint16_t
        arraySize; // At least 1 (basically everything is a vector...)
      uint16_t
        mipLevels;
      Multisapmling
        multisampling;
      
      TextureInfo();
    };

    using ArraySlices = Range;
    using MipSlices   = Range;

  }
}

#endif