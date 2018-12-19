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
        bool CPassBase::CMutableAccessor::registerResource(FrameGraphResourceId_t const &aResourceUID)
        {
            return mPass->registerResource(aResourceUID);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CStdUniquePtr_t<CPassBase::CAccessor> CPassBase::getAccessor(CPassKey<CGraphBuilder> &&aPassKey) const
        {
            return std::move(CStdUniquePtr_t<CPassBase::CAccessor>(new CPassBase::CAccessor(this)));
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CStdUniquePtr_t<CPassBase::CMutableAccessor> CPassBase::getMutableAccessor(CPassKey<CGraphBuilder> &&aPassKey)
        {
            return std::move(CStdUniquePtr_t<CPassBase::CMutableAccessor>(new CPassBase::CMutableAccessor(this)));
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CStdUniquePtr_t<CPassBase::CAccessor> CPassBase::getAccessor(CPassKey<CPassBuilder> &&aPassKey) const
        {
            return std::move(CStdUniquePtr_t<CPassBase::CAccessor>(new CPassBase::CAccessor(this)));
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CStdUniquePtr_t<CPassBase::CMutableAccessor> CPassBase::getMutableAccessor(CPassKey<CPassBuilder> &&aPassKey)
        {
            return std::move(CStdUniquePtr_t<CPassBase::CMutableAccessor>(new CPassBase::CMutableAccessor(this)));
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CStdUniquePtr_t<CPassBase::CAccessor> CPassBase::getAccessor(CPassKey<CGraph> &&aPassKey) const
        {
            return std::move(CStdUniquePtr_t<CPassBase::CAccessor>(new CPassBase::CAccessor(this)));
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
        bool CPassBase::registerResource(FrameGraphResourceId_t const &aResourceUID)
        {
            mResourceReferences.push_back(aResourceUID);
            return true;
        }
        //<-----------------------------------------------------------------------------
    }
}
