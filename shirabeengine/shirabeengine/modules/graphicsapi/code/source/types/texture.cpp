#include "graphicsapi/resources/types/texture.h"

namespace engine
{
    namespace gfxapi
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        STextureInfo::STextureInfo()
            : width(0)
            , height(1)
            , depth(1)
            , format(EFormat::Undefined)
            , arraySize(1)
            , mipLevels(1)
            , multisampling()
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        void STextureInfo::assignTextureInfoParameters(STextureInfo const &aOther)
        {
            width         = aOther.width;
            height        = aOther.height;
            depth         = aOther.depth;
            format        = aOther.format;
            arraySize     = aOther.arraySize;
            mipLevels     = aOther.mipLevels;
            multisampling = aOther.multisampling;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        std::string CTextureDeclaration::SDescriptor::toString() const
        {
            std::stringstream ss;
            ss
                    << "TextureDescriptor"
                    << " ('" << name << "'): "
                    << " Dimensions: 3, "
                    << " Format: " << (uint8_t)textureInfo.format << ", "
                    << " MipMaps: " << (textureInfo.mipLevels > 1 ? "true" : "false") << "; Levels: " << textureInfo.mipLevels << ", "
                    << " Array: " << (textureInfo.arraySize > 1 ? "true" : "false") << "; Layers: " << textureInfo.arraySize << ", "
                    << " Multisampling: " << (textureInfo.multisampling.size > 1 ? "true" : "false")
                    << "; Count/Quality:  " << textureInfo.multisampling.size << "/" << textureInfo.multisampling.quality << ", "
                    << " CPU-GPU-Usage: " << (uint8_t)cpuGpuUsage << ", "
                    << " GPU-Binding:  "  << (uint8_t)gpuBinding.value() << ";";

            return ss.str();
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CTextureDeclaration::CCreationRequest::CCreationRequest(SDescriptor const &aDescriptor)
            : CBaseDeclaration::CCreationRequestBase<SDescriptor>(aDescriptor)
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        std::string CTextureDeclaration::CCreationRequest::toString() const
        {
            std::stringstream ss;
            ss
                    << "TextureCreationRequest: \n"
                    << "[\n"
                    << resourceDescriptor().toString() << "\n"
                    << "]"
                    << std::endl;

            return ss.str();
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CTexture::CTexture(CTextureDeclaration::SDescriptor const &aDescriptor)
            : CTextureDeclaration()
            , CResourceDescriptorAdapter<CTextureDeclaration::SDescriptor>(aDescriptor)
        {}
        //<-----------------------------------------------------------------------------
    }
}
