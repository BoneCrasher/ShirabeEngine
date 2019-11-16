#include "graphicsapi/resources/types/buffer.h"

namespace engine
{
    namespace gfxapi
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        std::string CBufferDeclaration::SDescriptor::toString() const
        {
            std::stringstream ss;
            ss
                    << "BufferDescriptor";

            return ss.str();
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CBufferDeclaration::CCreationRequest::CCreationRequest(SDescriptor const &aDescriptor)
            : CBaseDeclaration::CCreationRequestBase<SDescriptor>(aDescriptor)
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        std::string CBufferDeclaration::CCreationRequest::toString() const
        {
            std::stringstream ss;
            ss
                    << "BufferCreationRequest: \n"
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
        CBuffer::CBuffer(CBufferDeclaration::SDescriptor const &aDescriptor)
            : CBufferDeclaration()
            , CResourceDescriptorAdapter<CBufferDeclaration::SDescriptor>(aDescriptor)
        {}
        //<-----------------------------------------------------------------------------
    }
}
