#include "material/materialserialization.h"

namespace engine
{
    namespace serialization
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        bool CMaterialSerializer::initialize()
        {
            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CMaterialSerializer::deinitialize()
        {
            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CMaterialSerializer::serialize(SMaterial const &aMaterial, CStdSharedPtr_t<IResult> &aOutResult)
        {
            nlohmann::json serializedData {};

            bool const successful = aMaterial.acceptSerializer(*this);
            if(successful)
            {
                CStdSharedPtr_t<CMaterialSerializationResult> result = makeCStdSharedPtr<CMaterialSerializationResult>(serializedData);

                aOutResult = std::move(result);
            }

            return successful;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CMaterialSerializer::serializeStage(SMaterialStage const &aStage)
        {
            bool const successful = aStage.acceptSerializer(*this);

            return successful;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CMaterialSerializer::CMaterialSerializationResult::CMaterialSerializationResult()
            : ISerializer<SMaterial>::IResult()
            , mResult({})
        { }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CMaterialSerializer::CMaterialSerializationResult::CMaterialSerializationResult(nlohmann::json const &aResult)
            : ISerializer<SMaterial>::IResult()
            , mResult(aResult)
        { }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CMaterialSerializer::CMaterialSerializationResult::asString(std::string &aOutString) const
        {
            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CMaterialSerializer::CMaterialSerializationResult::asBinaryBuffer(std::vector<uint8_t> &aOutBuffer) const
        {
            return true;
        }
        //<-----------------------------------------------------------------------------
    }
}
