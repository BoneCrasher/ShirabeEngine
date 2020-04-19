#include "graphicsapi/resources/types/renderpass.h"

namespace engine
{
    namespace gfxapi
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CRenderPassDeclaration::SDescriptor::SDescriptor()
            : name("")
            , attachmentDescriptions()
            , subpassDescriptions()
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        std::string CRenderPassDeclaration::SDescriptor::toString() const
        {
            std::stringstream ss;
            ss
                    << "Render Pass Descriptor ('" << name << "'):";

            return ss.str();
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CRenderPassDeclaration::CCreationRequest::CCreationRequest(SDescriptor const &aDescriptor)
            : CBaseDeclaration::CCreationRequestBase<SDescriptor>(aDescriptor)
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        std::string CRenderPassDeclaration::CCreationRequest::toString() const
        {
            std::stringstream ss;
            ss
                    << "RenderPassCreationRequest: \n"
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
        CRenderPass::CRenderPass(const CRenderPass::SDescriptor &aDescriptor)
            : CRenderPassDeclaration()
            , CResourceDescriptorAdapter<CRenderPassDeclaration::SDescriptor>(aDescriptor)
        {}
        //<-----------------------------------------------------------------------------
    }
}
