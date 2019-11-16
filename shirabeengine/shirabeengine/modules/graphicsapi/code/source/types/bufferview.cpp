#include "graphicsapi/resources/types/bufferview.h"

namespace engine
{
    namespace gfxapi
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        std::string CBufferViewDeclaration::SDescriptor::toString() const
        {
            std::stringstream ss;
            ss
                    << "BufferViewDescriptor";

            return ss.str();
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CBufferViewDeclaration::CCreationRequest::CCreationRequest(
                SDescriptor        const &aDescriptor,
                PublicResourceId_t const &aUnderlyingBufferHandle)
            : CBaseDeclaration::CCreationRequestBase<SDescriptor>(aDescriptor)
            , mUnderlyingBufferHandle(aUnderlyingBufferHandle)
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        std::string CBufferViewDeclaration::CCreationRequest::toString() const
        {
            std::stringstream ss;
            ss
                    << "BufferViewCreationRequest: \n"
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
        PublicResourceId_t const &CBufferViewDeclaration::CCreationRequest::underlyingBufferHandle() const
        {
            return mUnderlyingBufferHandle;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CBufferView::CBufferView(CBufferViewDeclaration::SDescriptor const &aDescriptor)
            : CBufferViewDeclaration()
            , CResourceDescriptorAdapter<CBufferViewDeclaration::SDescriptor>(aDescriptor)
        {}
        //<-----------------------------------------------------------------------------
    }
}
