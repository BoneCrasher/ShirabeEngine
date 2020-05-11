#include "renderer/framegraph/pass.h"

namespace engine
{
    namespace framegraph
    {
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
        bool CPassBase::acceptSerializer(IRenderGraphSerializer &aSerializer) const
        {
            bool const serialized = aSerializer.serializePass(*this);
            return serialized;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CPassBase::acceptDeserializer(IRenderGraphDeserializer &aDeserializer)
        {
            bool const deserialized = aDeserializer.deserializePass(*this);
            return deserialized;
        }
#endif
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CPassBase::registerResource(RenderGraphResourceId_t const &aResourceUID)
        {
            mResourceReferences.push_back(aResourceUID);
            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------
    }
}
