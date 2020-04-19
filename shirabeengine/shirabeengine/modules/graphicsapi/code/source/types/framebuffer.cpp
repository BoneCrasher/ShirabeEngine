#include "graphicsapi/resources/types/framebuffer.h"

namespace engine
{
    namespace gfxapi
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CFrameBufferDeclaration::SDescriptor::SDescriptor()
            : name("")
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        std::string CFrameBufferDeclaration::SDescriptor::toString() const
        {
            std::stringstream ss;
            ss
                    << "FrameBuffer::SDescriptor ('" << name << "'): ";

            return ss.str();
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CFrameBufferDeclaration::CCreationRequest::CCreationRequest(
                SDescriptor            const &aDescriptor,
                PublicResourceId_t     const &aRenderPassHandle,
                PublicResourceIdList_t const &aTextureViewHandles)
            : CBaseDeclaration::CCreationRequestBase<SDescriptor>(aDescriptor)
            , mRenderPassHandle(aRenderPassHandle)
            , mTextureViewHandles(aTextureViewHandles)
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        PublicResourceId_t const &CFrameBufferDeclaration::CCreationRequest::renderPassHandle() const
        {
            return mRenderPassHandle;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        PublicResourceIdList_t const &CFrameBufferDeclaration::CCreationRequest::textureViewHandles() const
        {
            return mTextureViewHandles;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        std::string CFrameBufferDeclaration::CCreationRequest::toString() const
        {
            std::stringstream ss;
            ss
                    << "RenderTargetViewCreationRequest: \n"
                    << std::endl;

            return ss.str();
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CFrameBuffer::CFrameBuffer(
                const CFrameBuffer::SDescriptor &aDescriptor)
            : CFrameBufferDeclaration()
            , CResourceDescriptorAdapter<CFrameBufferDeclaration::SDescriptor>(aDescriptor)
        {}
        //<-----------------------------------------------------------------------------
    }
}
