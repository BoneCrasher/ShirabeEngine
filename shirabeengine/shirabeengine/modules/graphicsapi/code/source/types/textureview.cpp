#include "graphicsapi/resources/types/textureview.h"

namespace engine
{
    namespace gfxapi
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CTextureViewDeclaration::SDescriptor::SDescriptor()
            : name("")
            , subjacentTexture()
            , textureFormat(EFormat::Undefined)
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        std::string CTextureViewDeclaration::SDescriptor::toString() const
        {
            std::stringstream ss;
            ss
                    << "RenderTargetSDescriptor ('" << name << "'): "
                    << " Format: " << (uint8_t)textureFormat << ";";

            return ss.str();
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CTextureViewDeclaration::CCreationRequest::CCreationRequest(
                SDescriptor        const &aDescriptor,
                PublicResourceId_t const &aUnderlyingTextureHandle)
            : CBaseDeclaration::CCreationRequestBase<SDescriptor>(aDescriptor)
            , mUnderlyingTextureHandle(aUnderlyingTextureHandle)
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        PublicResourceId_t const &CTextureViewDeclaration::CCreationRequest::underlyingTextureHandle() const
        {
            return mUnderlyingTextureHandle;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        std::string CTextureViewDeclaration::CCreationRequest::toString() const
        {
            std::stringstream ss;
            ss
                    << "RenderTargetViewCreationRequest: \n"
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
        CTextureView::CTextureView(
                const CTextureView::SDescriptor &aDescriptor)
            : CTextureViewDeclaration()
            , CResourceDescriptorAdapter<CTextureViewDeclaration::SDescriptor>(aDescriptor)
        {}
        //<-----------------------------------------------------------------------------
    }
}
