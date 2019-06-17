#include "graphicsapi/resources/types/pipeline.h"

namespace engine
{
    namespace gfxapi
    {

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CPipelineDeclaration::SDescriptor::SDescriptor()
            : SDescriptorImplBase<EResourceType::GAPI_COMPONENT, EResourceSubType::PIPELINE>()
            , name("")
            , viewPort({})
            , scissor({})
            , inputAssemblyState({})
            , vertexInputBindings({})
            , vertexInputAttributes({})
            , rasterizerState({})
            , multiSampler({})
            , depthStencilState({})
            , colorBlendAttachmentStates({})
            , colorBlendState({})
            , pipelineLayout({})
            , descriptorSetLayoutCreateInfos({})
            , descriptorSetLayoutBindings({})
        {}

        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        std::string CPipelineDeclaration::SDescriptor::toString() const
        {
            std::stringstream ss;
            ss
                    << "CPipeline::SDescriptor ('" << name << "'): ";

            return ss.str();
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CPipelineDeclaration::CCreationRequest::CCreationRequest(
                SDescriptor            const &aDescriptor,
                PublicResourceId_t     const &aRenderPassHandle,
                PublicResourceIdList_t const &aTextureViewHandles,
                PublicResourceIdList_t const &aBufferViewHandles)
            : CBaseDeclaration::CCreationRequestBase<SDescriptor>(aDescriptor)
            , mRenderPassHandle  ( aRenderPassHandle   )
            , mTextureViewHandles( aTextureViewHandles )
            , mBufferViewHandles ( aBufferViewHandles  )
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        PublicResourceId_t const &CPipelineDeclaration::CCreationRequest::renderPassHandle() const
        {
            return mRenderPassHandle;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        PublicResourceIdList_t const &CPipelineDeclaration::CCreationRequest::textureViewHandles() const
        {
            return mTextureViewHandles;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        PublicResourceIdList_t const &CPipelineDeclaration::CCreationRequest::bufferViewHandles() const
        {
            return mBufferViewHandles;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        std::string CPipelineDeclaration::CCreationRequest::toString() const
        {
            std::stringstream ss;
            ss
                    << "PipelineCreationRequest: \n"
                    << std::endl;

            return ss.str();
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CPipeline::CPipeline(CPipeline::SDescriptor const &aDescriptor)
            : CPipelineDeclaration()
            , CResourceDescriptorAdapter<CPipelineDeclaration::SDescriptor>(aDescriptor)
        {}
        //<-----------------------------------------------------------------------------

    }
}
