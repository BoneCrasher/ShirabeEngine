#include "graphicsapi/resources/types/swapchain.h"

namespace engine
{
    namespace gfxapi
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CSwapChainBufferDeclaration::SDescriptor::SDescriptor()
            : name("")
            , texture()
            , backBufferIndex(0)
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        std::string CSwapChainBufferDeclaration::SDescriptor::toString() const
        {
            std::stringstream ss;
            ss
                    << "SwapChainBufferDescriptor ('" << name << "'): ";

            return ss.str();
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CSwapChainBufferDeclaration::CCreationRequest::CCreationRequest(SDescriptor const& desc)
            : CBaseDeclaration::CreationRequestBase<SDescriptor>(desc)
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        std::string CSwapChainBufferDeclaration::CCreationRequest::toString() const
        {
            std::stringstream ss;

            ss
                    << "SwapChainBufferCreationRequest: \n"
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
        CStdSharedPtr_t<CSwapChainBuffer> CSwapChainBuffer::create(CSwapChainBuffer::SDescriptor const &aDescriptor)
        {
             CStdSharedPtr_t<CSwapChainBuffer> buffer = makeCStdSharedPtr<CSwapChainBuffer>(aDescriptor);
             return buffer;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CSwapChainBuffer::CSwapChainBuffer(CSwapChainBuffer::SDescriptor const &aDescriptor)
            : CSwapChainBufferDeclaration()
            , CResourceDescriptorAdapter<CSwapChainBufferDeclaration::SDescriptor>(aDescriptor)
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        std::string CSwapChainDeclaration::SDescriptor::toString() const
        {
            std::stringstream ss;
            ss << "SwapChainDescriptor ('" << name << "'): ";

            return ss.str();
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CSwapChainDeclaration::CCreationRequest::CCreationRequest(SDescriptor const &aDescriptor)
            : CBaseDeclaration::CCreationRequestBase<SDescriptor>(aDescriptor)
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        std::string CSwapChainDeclaration::CCreationRequest::toString() const
        {
            std::stringstream ss;
            ss
                    << "SwapChainCreationRequest: \n"
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
        CStdSharedPtr_t<CSwapChain>
        CSwapChain::create(CSwapChain::SDescriptor const &desc) {
            return CStdSharedPtr_t<CSwapChain>(new CSwapChain(desc));
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CStdSharedPtr_t<CTextureView> const CSwapChain::getBackBufferRenderTargetView(uint32_t const &aIndex)
        {
            if(aIndex < 0 || aIndex >= mBackBufferRenderTargetViews.size())
                return nullptr;

            return mBackBufferRenderTargetViews.at(aIndex);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CStdSharedPtr_t<CTextureView> const CSwapChain::getCurrentBackBufferRenderTargetView()
        {
            CStdSharedPtr_t<CTextureView> view = getBackBufferRenderTargetView(mCurrentBackBufferIndex);
            return view;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CSwapChain::CSwapChain(CSwapChain::SDescriptor const &aDescriptor)
            : CSwapChainDeclaration()
            , CResourceDescriptorAdapter<CSwapChainDeclaration::SDescriptor>(aDescriptor)
            , mCurrentBackBufferIndex(0)
            , mBackBufferRenderTargetViews()
        {}
        //<-----------------------------------------------------------------------------
    }
}
