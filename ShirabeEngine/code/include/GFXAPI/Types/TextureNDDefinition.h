#ifndef __SHIRABE_RESOURCES_TEXTUREND_DEFINITION_H__
#define __SHIRABE_RESOURCES_TEXTUREND_DEFINITION_H__

#include <stdint.h>

namespace Engine {
  namespace Resources {
    
    /**********************************************************************************************//**
     * \struct	TextureMipMapDescriptor
     *
     * \brief	A texture mip map descriptor.
     **************************************************************************************************/
    struct TextureMipMapDescriptor {
      bool    useMipMaps;
      uint8_t mipLevels;
      uint8_t firstMipMapLevel;
    };

    /**********************************************************************************************//**
     * \struct	TextureArrayDescriptor
     *
     * \brief	A texture array descriptor.
     **************************************************************************************************/
    struct TextureArrayDescriptor {
      bool    isTextureArray;
      uint8_t size;
      uint8_t firstArraySlice;
    };

    /**********************************************************************************************//**
     * \struct	TextureMultisapmlingDescriptor
     *
     * \brief	A texture multisapmling descriptor.
     **************************************************************************************************/
    struct TextureMultisapmlingDescriptor {
      bool    useMultisampling;
      uint8_t size;
      uint8_t quality;
    };
  }
}

#endif
