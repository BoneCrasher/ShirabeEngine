#include "graphicsapi/resources/types/rasterizerstate.h"

namespace engine
{
    namespace gfxapi
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CRasterizerStateDeclaration::SDescriptor
        ::SDescriptor()
            : name("")
            , fillMode(EFillMode::Solid)
            , cullMode(ECullMode::Back)
            , antialiasRasterLines(false)
            , multisamplingEnabled(false)
            , scissorEnabled(false)
            , depthClipEnabled(true)
        {}
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        std::string CRasterizerStateDeclaration::SDescriptor::toString() const
        {
            std::stringstream ss;
            ss << "RasterizerStateSDescriptor('" << name << "'):\n";

            return ss.str();
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CRasterizerStateDeclaration::CCreationRequest::CCreationRequest(SDescriptor const &aDescriptor)
            : CBaseDeclaration::CCreationRequestBase<SDescriptor>(aDescriptor)
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        std::string CRasterizerStateDeclaration::CCreationRequest::toString() const
        {
            std::stringstream ss;
            ss
                    << "RasterizerStateCreationRequest: \n"
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
        CRasterizerState::CRasterizerState(CRasterizerState::SDescriptor const &aDescriptor)
            : CRasterizerStateDeclaration()
            , CResourceDescriptorAdapter<CRasterizerState::SDescriptor>(aDescriptor)
        {}
        //<-----------------------------------------------------------------------------
    }
}
