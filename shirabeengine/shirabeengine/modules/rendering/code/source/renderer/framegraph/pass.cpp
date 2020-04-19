#include "renderer/framegraph/pass.h"

namespace engine
{
    namespace framegraph
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CPassBase::CAccessor::CAccessor(CPassBase const *aPass)
            : mPass(aPass)
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        FrameGraphResourceIdList const &CPassBase::CAccessor::resourceReferences() const
        {
            return mPass->mResourceReferences;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CPassBase::CMutableAccessor::CMutableAccessor(CPassBase *aPass)
            : CAccessor(aPass)
            , mPass(aPass)
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        FrameGraphResourceIdList &CPassBase::CMutableAccessor::mutableResourceReferences()
        {
            return mPass->mResourceReferences;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CPassBase::CMutableAccessor::registerResource(FrameGraphResourceId_t const &aResourceUID)
        {
            return mPass->registerResource(aResourceUID);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        Unique<CPassBase::CAccessor> CPassBase::getAccessor(CPassKey<CGraphBuilder> &&aPassKey) const
        {
            SHIRABE_UNUSED(aPassKey);

            return Unique<CPassBase::CAccessor>(new CPassBase::CAccessor(this));
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        Unique<CPassBase::CMutableAccessor> CPassBase::getMutableAccessor(CPassKey<CGraphBuilder> &&aPassKey)
        {
            SHIRABE_UNUSED(aPassKey);

            return Unique<CPassBase::CMutableAccessor>(new CPassBase::CMutableAccessor(this));
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        Unique<CPassBase::CAccessor> CPassBase::getAccessor(CPassKey<CPassBuilder> &&aPassKey) const
        {
            SHIRABE_UNUSED(aPassKey);

            return Unique<CPassBase::CAccessor>(new CPassBase::CAccessor(this));
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        Unique<CPassBase::CMutableAccessor> CPassBase::getMutableAccessor(CPassKey<CPassBuilder> &&aPassKey)
        {
            SHIRABE_UNUSED(aPassKey);

            return Unique<CPassBase::CMutableAccessor>(new CPassBase::CMutableAccessor(this));
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        Unique<CPassBase::CAccessor> CPassBase::getAccessor(CPassKey<CGraph> &&aPassKey) const
        {
            SHIRABE_UNUSED(aPassKey);

            return Unique<CPassBase::CAccessor>(new CPassBase::CAccessor(this));
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CPassBase::CPassBase(
                PassUID_t   const &aPassUID,
                std::string const &aPassName)
            : mPassUID(aPassUID)
            , mPassName(aPassName)
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        std::string const &CPassBase::passName() const
        {
            return mPassName;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        PassUID_t const &CPassBase::passUID() const
        {
            return mPassUID;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
#if defined SHIRABE_FRAMEGRAPH_ENABLE_SERIALIZATION
        bool CPassBase::acceptSerializer(IFrameGraphSerializer &aSerializer) const
        {
            bool const serialized = aSerializer.serializePass(*this);
            return serialized;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CPassBase::acceptDeserializer(IFrameGraphDeserializer &aDeserializer)
        {
            bool const deserialized = aDeserializer.deserializePass(*this);
            return deserialized;
        }
#endif
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CPassBase::registerResource(FrameGraphResourceId_t const &aResourceUID)
        {
            mResourceReferences.push_back(aResourceUID);
            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------
    }
}
