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
            IMaterialSerializer &serializer = *this;

            bool const successful = aMaterial.acceptSerializer(serializer);
            if(successful)
            {
                nlohmann::json &serializedData = mRoot; // For debug...
                CStdSharedPtr_t<CMaterialSerializationResult> result = makeCStdSharedPtr<CMaterialSerializationResult>(serializedData);

                aOutResult = std::move(result);
            }

            return successful;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CMaterialSerializer::beginArray(std::string const &aName)
        {
            if(not hasCurrentItem())
            {
                // Invalid use!
                CLog::Error(logTag(), "Cannot add array to non-existent top level object.");
                return false;
            }

            nlohmann::json &top = getCurrentItem();
            if(top.is_array())
            {
                // Invalid use!
                CLog::Error(logTag(), "Cannot add array to array.");
                return false;
            }
            else
            {
                nlohmann::json &arr = top[aName] = nlohmann::json::array();
                mCurrentJSONState.push(arr);
            }

            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CMaterialSerializer::endArray()
        {
            if(not hasCurrentItem())
            {
                // Nothing to remove
                CLog::Warning(logTag(), "Trying to end array on non-existent top level object.");
                return true;
            }

            nlohmann::json &top = getCurrentItem();
            if(not top.is_array())
            {
                // Invalid use!
                CLog::Error(logTag(), "Cannot end array, if current toplevel item is not an array.");
                return false;
            }

            mCurrentJSONState.pop();

            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CMaterialSerializer::beginObject(std::string const &aName)
        {
            if(not hasCurrentItem())
            {
                // Invalid use!
                CLog::Warning(logTag(), "Cannot add object to non-existent top level object. Creating root object.");
                mRoot = nlohmann::json::object();
                mCurrentJSONState.push(mRoot);
                return true;
            }

            nlohmann::json &top = getCurrentItem();
            if(top.is_array())
            {
                top.push_back(nlohmann::json::object());
                mCurrentJSONState.push(top.back());
            }
            else
            {
                nlohmann::json &obj = top[aName] = nlohmann::json::object();
                mCurrentJSONState.push(obj);
            }

            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CMaterialSerializer::endObject()
        {
            if(not hasCurrentItem())
            {
                // Nothing to remove
                CLog::Warning(logTag(), "Trying to end object on non-existent top level object.");
                return true;
            }

            nlohmann::json &top = getCurrentItem();
            if(not top.is_object())
            {
                // Invalid use!
                CLog::Error(logTag(), "Cannot end object, if current toplevel item is not an object.");
                return false;
            }

            mCurrentJSONState.pop();

            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CMaterialSerializer::writeValue(std::string const &aKey, std::string const &aValue)
        {
            if(not hasCurrentItem())
            {
                // Invalid use!
                CLog::Error(logTag(), "Cannot add value to non-existent top-level item.");
                return false;
            }

            nlohmann::json &top = getCurrentItem();
            if(top.is_array())
            {
                top.push_back(aValue);
            }
            else
            {
                top[aKey] = aValue;
            }

            return true;
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
            aOutString = mResult.dump();

            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CMaterialSerializer::CMaterialSerializationResult::asBinaryBuffer(std::vector<uint8_t> &aOutBuffer) const
        {
            aOutBuffer = nlohmann::json::to_msgpack(mResult);

            return true;
        }
        //<-----------------------------------------------------------------------------
    }
}
