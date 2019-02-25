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
            return writeValueImpl(aKey, aValue);
        }

        bool CMaterialSerializer::writeValue(std::string const &aKey, int8_t const &aValue)
        {
            return writeValueImpl(aKey, aValue);
        }

        bool CMaterialSerializer::writeValue(std::string const &aKey, int16_t const &aValue)
        {
            return writeValueImpl(aKey, aValue);
        }

        bool CMaterialSerializer::writeValue(std::string const &aKey, int32_t const &aValue)
        {
            return writeValueImpl(aKey, aValue);
        }

        bool CMaterialSerializer::writeValue(std::string const &aKey, int64_t const &aValue)
        {
            return writeValueImpl(aKey, aValue);
        }

        bool CMaterialSerializer::writeValue(std::string const &aKey, uint8_t const &aValue)
        {
            return writeValueImpl(aKey, aValue);
        }

        bool CMaterialSerializer::writeValue(std::string const &aKey, uint16_t const &aValue)
        {
            return writeValueImpl(aKey, aValue);
        }

        bool CMaterialSerializer::writeValue(std::string const &aKey, uint32_t const &aValue)
        {
            return writeValueImpl(aKey, aValue);
        }

        bool CMaterialSerializer::writeValue(std::string const &aKey, uint64_t const &aValue)
        {
            return writeValueImpl(aKey, aValue);
        }

        bool CMaterialSerializer::writeValue(std::string const &aKey, float const &aValue)
        {
            return writeValueImpl(aKey, aValue);
        }

        bool CMaterialSerializer::writeValue(std::string const &aKey, double const &aValue)
        {
            return writeValueImpl(aKey, aValue);
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

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CMaterialDeserializer::initialize()
        {
            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CMaterialDeserializer::deinitialize()
        {
            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CMaterialDeserializer::deserialize(std::string const &aSource, CStdSharedPtr_t<IResult> &aOutResult)
        {
            nlohmann::json json = nlohmann::json::parse(aSource);
            mRoot = json;

            IMaterialDeserializer &deserializer = *this;

            auto material = makeCStdSharedPtr<SMaterial>();

            bool const successful = material->acceptDeserializer(deserializer);
            if(successful)
            {
                auto result = makeCStdSharedPtr<CMaterialDeserializationResult>(material);

                aOutResult = std::move(result);
            }

            return successful;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CMaterialDeserializer::deserialize(std::vector<int8_t> const &aSource, CStdSharedPtr_t<IResult> &aOutResult)
        {
            nlohmann::json json = nlohmann::json::from_msgpack(aSource);
            mRoot = json;

            IMaterialDeserializer &deserializer = *this;

            auto material = makeCStdSharedPtr<SMaterial>();

            bool const successful = material->acceptDeserializer(deserializer);
            if(successful)
            {
                auto result = makeCStdSharedPtr<CMaterialDeserializationResult>(material);

                aOutResult = std::move(result);
            }

            return successful;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CMaterialDeserializer::beginArray(std::string const &aName)
        {
            if(not hasCurrentItem())
            {
                // Invalid use!
                CLog::Error(logTag(), "Cannot read array w/ name '%0' from non-existent top level object.", aName);
                return false;
            }

            nlohmann::json &top = getCurrentItem();
            nlohmann::json &arr = top[aName];
            if(not arr.is_array())
            {
                // Invalid use!
                CLog::Error(logTag(), "Cannot read array '%0'. Incompatible types.", aName);
                return false;
            }

            mCurrentJSONState.push(arr);

            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CMaterialDeserializer::endArray()
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
        bool CMaterialDeserializer::beginObject(std::string const &aName)
        {
            if(not hasCurrentItem())
            {
                // Invalid use!
                CLog::Error(logTag(), "Cannot read object on non-existent top level object.");
                return true;
            }

            nlohmann::json &top = getCurrentItem();
            if(top.is_array())
            {
                nlohmann::json &obj = top.at(0); // TODO: How to properly read array.
                if(not obj.is_object())
                {
                    // Invalid use!
                    CLog::Error(logTag(), "Cannot read object '%0'. Incompatible types.", aName);
                    return false;
                }

                mCurrentJSONState.push(obj);
            }
            else
            {
                nlohmann::json &obj = top[aName];
                if(not obj.is_object())
                {
                    // Invalid use!
                    CLog::Error(logTag(), "Cannot read object '%0'. Incompatible types.", aName);
                    return false;
                }

                mCurrentJSONState.push(obj);
            }

            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CMaterialDeserializer::endObject()
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
        bool CMaterialDeserializer::readValue(std::string const &aKey, std::string &aOutValue)
        {
            return readValueImpl(aKey, aOutValue);
        }

        bool CMaterialDeserializer::readValue(std::string const &aKey, int8_t &aOutValue)
        {
            return readValueImpl(aKey, aOutValue);
        }

        bool CMaterialDeserializer::readValue(std::string const &aKey, int16_t &aOutValue)
        {
            return readValueImpl(aKey, aOutValue);
        }

        bool CMaterialDeserializer::readValue(std::string const &aKey, int32_t &aOutValue)
        {
            return readValueImpl(aKey, aOutValue);
        }

        bool CMaterialDeserializer::readValue(std::string const &aKey, int64_t &aOutValue)
        {
            return readValueImpl(aKey, aOutValue);
        }

        bool CMaterialDeserializer::readValue(std::string const &aKey, uint8_t &aOutValue)
        {
            return readValueImpl(aKey, aOutValue);
        }

        bool CMaterialDeserializer::readValue(std::string const &aKey, uint16_t &aOutValue)
        {
            return readValueImpl(aKey, aOutValue);
        }

        bool CMaterialDeserializer::readValue(std::string const &aKey, uint32_t &aOutValue)
        {
            return readValueImpl(aKey, aOutValue);
        }

        bool CMaterialDeserializer::readValue(std::string const &aKey, uint64_t &aOutValue)
        {
            return readValueImpl(aKey, aOutValue);
        }

        bool CMaterialDeserializer::readValue(std::string const &aKey, float &aOutValue)
        {
            return readValueImpl(aKey, aOutValue);
        }

        bool CMaterialDeserializer::readValue(std::string const &aKey, double &aOutValue)
        {
            return readValueImpl(aKey, aOutValue);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CMaterialDeserializer::CMaterialDeserializationResult::CMaterialDeserializationResult()
            : IDeserializer<SMaterial>::IResult()
            , mResult(nullptr)
        { }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CMaterialDeserializer::CMaterialDeserializationResult::CMaterialDeserializationResult(CStdSharedPtr_t<SMaterial> const &aResult)
            : IDeserializer<SMaterial>::IResult()
            , mResult(aResult)
        { }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CMaterialDeserializer::CMaterialDeserializationResult::asT(CStdSharedPtr_t<SMaterial> &aOutMaterial) const
        {
            aOutMaterial = mResult;

            return true;
        }
        //<-----------------------------------------------------------------------------
    }
}
