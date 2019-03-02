#ifndef __SHIRABE_MATERIAL_SERIALIZATION_H__
#define __SHIRABE_MATERIAL_SERIALIZATION_H__

#include <sstream>
#include <optional>
#include <stack>

#include <nlohmann/json.hpp>

#include <log/log.h>
#include <core/enginetypehelper.h>
#include <core/serialization/serialization.h>

#include "material/material_declaration.h"

namespace engine
{
    namespace serialization
    {
        using namespace engine::material;

        using engine::serialization::ISerializer;
        using engine::serialization::ISerializable;
        using engine::serialization::IDeserializable;

        /**
         * Read a string and convert it to an EShaderStage value (if possible).
         *
         * @param aString
         * @return
         */
        EShaderStage stageFromString(std::string const &aString);

        /**
         * Read an EShaderStage value and convert it to its string representation.
         *
         * @param aStage
         * @return
         */
        std::string const stageToString(EShaderStage const &aStage);

        /**
         * The IFrameGraphSerializer interface describes the basic requiremets
         * to serialize a material instance.
         */
        template <typename T>
        class IJSONSerializer
                : public ISerializer<T>
        {
            SHIRABE_DECLARE_INTERFACE(IJSONSerializer);

        public_api:
            /**
             * Begin a JSON array, to which the upcoming objects will be added.
             *
             * @param aName Name of the array to begin.
             * @return
             */
            virtual bool beginArray(std::string const &aName) = 0;

            /**
             * End the current array. If any.
             * @return
             */
            virtual bool endArray() = 0;

            /**
             * Begin a new object w/ the given name.
             *
             * @param aName
             * @return
             */
            virtual bool beginObject(std::string const &aName) = 0;

            /**
             * End the current object, if any.
             *
             * @return
             */
            virtual bool endObject() = 0;

            /**
             * Write a value to the current object/array.
             *
             * @param aKey
             * @param aValue
             * @return
             */
            virtual bool writeValue(std::string const &aKey, std::string const &aValue) = 0;
            virtual bool writeValue(std::string const &aKey, int8_t      const &aValue) = 0;
            virtual bool writeValue(std::string const &aKey, int16_t     const &aValue) = 0;
            virtual bool writeValue(std::string const &aKey, int32_t     const &aValue) = 0;
            virtual bool writeValue(std::string const &aKey, int64_t     const &aValue) = 0;
            virtual bool writeValue(std::string const &aKey, uint8_t     const &aValue) = 0;
            virtual bool writeValue(std::string const &aKey, uint16_t    const &aValue) = 0;
            virtual bool writeValue(std::string const &aKey, uint32_t    const &aValue) = 0;
            virtual bool writeValue(std::string const &aKey, uint64_t    const &aValue) = 0;
            virtual bool writeValue(std::string const &aKey, float       const &aValue) = 0;
            virtual bool writeValue(std::string const &aKey, double      const &aValue) = 0;
        };

        /**
         * The IFrameGraphSerializer interface describes the basic requiremets
         * to deserialize a material instance.
         */
        template <typename T>
        class IJSONDeserializer
                : public IDeserializer<T>
        {
            SHIRABE_DECLARE_INTERFACE(IJSONDeserializer);

        public_api:
            /**
             * Begin a JSON array, to which the upcoming objects will be added.
             *
             * @param aName Name of the array to begin.
             * @return
             */
            virtual bool beginArray(std::string const &aName, uint32_t &aOutArraySize) = 0;

            /**
             * End the current array. If any.
             * @return
             */
            virtual bool endArray() = 0;

            /**
             * Begin a new object w/ the given name.
             *
             * @param aName
             * @return
             */
            virtual bool beginObject(std::string const &aName) = 0;

            /**
             * Begin a new object w/ the given index into an array.
             *
             * @param aName
             * @return
             */
            virtual bool beginObject(uint32_t const &aIndex) = 0;

            /**
             * End the current object, if any.
             *
             * @return
             */
            virtual bool endObject() = 0;

            /**
             * Write a value to the current object/array.
             *
             * @param aKey
             * @param aValue
             * @return
             */
            virtual bool readValue(std::string const &aKey, std::string &aOutValue) = 0;
            virtual bool readValue(std::string const &aKey, int8_t      &aOutValue) = 0;
            virtual bool readValue(std::string const &aKey, int16_t     &aOutValue) = 0;
            virtual bool readValue(std::string const &aKey, int32_t     &aOutValue) = 0;
            virtual bool readValue(std::string const &aKey, int64_t     &aOutValue) = 0;
            virtual bool readValue(std::string const &aKey, uint8_t     &aOutValue) = 0;
            virtual bool readValue(std::string const &aKey, uint16_t    &aOutValue) = 0;
            virtual bool readValue(std::string const &aKey, uint32_t    &aOutValue) = 0;
            virtual bool readValue(std::string const &aKey, uint64_t    &aOutValue) = 0;
            virtual bool readValue(std::string const &aKey, float       &aOutValue) = 0;
            virtual bool readValue(std::string const &aKey, double      &aOutValue) = 0;
        };

        /**
         * The CMaterialSerializer class implements material deserialziation.
         */
        template <typename T>
        class SHIRABE_TEST_EXPORT CJSONSerializer
                : public IJSONSerializer<T>
        {
            SHIRABE_DECLARE_LOG_TAG(CJSONSerializer);

        public_destructors:
            virtual ~CJSONSerializer() = default;

        public_structs:
            /*!
             * The IResult interface of the ISerializer<T> interface declares required
             * signatures for result retrieval from a serialization process.
             */
            class SHIRABE_TEST_EXPORT CSerializationResult
                    : public ISerializer<T>::IResult
            {
            public_constructors:
                CSerializationResult();

                CSerializationResult(nlohmann::json const &aResult);

            public_destructors:
                virtual ~CSerializationResult() = default;

            public_methods:
                bool asString      (std::string          &aOutString) const;
                bool asBinaryBuffer(std::vector<uint8_t> &aOutBuffer) const;

            private_members:
                nlohmann::json const mResult;
            };

        public_methods:
            /**
             * Initialize the serializer and prepare for serialization calls.
             *
             * @return True, if successful. False otherwise.
             */
            bool initialize();

            /**
             * Cleanup and shutdown...
             *
             * @return True, if successful. False otherwise.
             */
            bool deinitialize();

            /*!
             * Serialize an instance of type SShaderCompilationUnit into whichever internal representation and
             * provide it using a pointer to IResult.
             *
             * @param aInput  Input data for serialization.
             * @param aOutResult Result-Instance holding the serialized data, providing access to
             *                   it in various output formats.
             * @return
             */
            bool serialize(T const &aInput, CStdSharedPtr_t<typename IJSONSerializer<T>::IResult> &aOutResult);

            /**
             * Begin a JSON array, to which the upcoming objects will be added.
             *
             * @param aName Name of the array to begin.
             * @return
             */
            bool beginArray(std::string const &aName);

            /**
             * End the current array. If any.
             * @return
             */
            bool endArray();

            /**
             * Begin a new object w/ the given name.
             *
             * @param aName
             * @return
             */
            bool beginObject(std::string const &aName);

            /**
             * End the current object, if any.
             *
             * @return
             */
            bool endObject();

            /**
             * Write a string value to the current object/array.
             *
             * @param aKey
             * @param aValue
             * @return
             */
            bool writeValue(std::string const &aKey, std::string const &aValue);
            bool writeValue(std::string const &aKey, int8_t      const &aValue);
            bool writeValue(std::string const &aKey, int16_t     const &aValue);
            bool writeValue(std::string const &aKey, int32_t     const &aValue);
            bool writeValue(std::string const &aKey, int64_t     const &aValue);
            bool writeValue(std::string const &aKey, uint8_t     const &aValue);
            bool writeValue(std::string const &aKey, uint16_t    const &aValue);
            bool writeValue(std::string const &aKey, uint32_t    const &aValue);
            bool writeValue(std::string const &aKey, uint64_t    const &aValue);
            bool writeValue(std::string const &aKey, float       const &aValue);
            bool writeValue(std::string const &aKey, double      const &aValue);

        private_methods:

            /**
             *
             *
             * @param aKey
             * @param aValue
             * @return
             */
            template <typename TValue>
            bool writeValueImpl(std::string const &aKey, TValue const &aValue)
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

            /**
             * Check, whether there's a current top level item.
             *
             * @return
             */
            bool hasCurrentItem() const
            {
                return (not mCurrentJSONState.empty());
            }

            /**
             * Return a reference to the current top level item.
             *
             * @return
             */
            SHIRABE_INLINE
            nlohmann::json &getCurrentItem()
            {
                return mCurrentJSONState.top().get();
            }

        private_members:

            nlohmann::json                                     mRoot;
            std::stack<std::reference_wrapper<nlohmann::json>> mCurrentJSONState;
        };

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename T>
        bool CJSONSerializer<T>::initialize()
        {
            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        bool CJSONSerializer<T>::deinitialize()
        {
            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        bool CJSONSerializer<T>::serialize(T const &aInput, CStdSharedPtr_t<typename IJSONSerializer<T>::IResult> &aOutResult)
        {
            IJSONSerializer<T> &serializer = *this;

            bool const successful = aInput.acceptSerializer(serializer);
            if(successful)
            {
                nlohmann::json &serializedData = mRoot; // For debug...
                CStdSharedPtr_t<CSerializationResult> result = makeCStdSharedPtr<CSerializationResult>(serializedData);

                aOutResult = std::move(result);
            }

            return successful;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        template <typename T>
        bool CJSONSerializer<T>::beginArray(std::string const &aName)
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
        template <typename T>
        bool CJSONSerializer<T>::endArray()
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
        template <typename T>
        bool CJSONSerializer<T>::beginObject(std::string const &aName)
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
        template <typename T>
        bool CJSONSerializer<T>::endObject()
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
        template <typename T>
        bool CJSONSerializer<T>::writeValue(std::string const &aKey, std::string const &aValue)
        {
            return writeValueImpl(aKey, aValue);
        }
        template <typename T>
        bool CJSONSerializer<T>::writeValue(std::string const &aKey, int8_t const &aValue)
        {
            return writeValueImpl(aKey, aValue);
        }
        template <typename T>
        bool CJSONSerializer<T>::writeValue(std::string const &aKey, int16_t const &aValue)
        {
            return writeValueImpl(aKey, aValue);
        }
        template <typename T>
        bool CJSONSerializer<T>::writeValue(std::string const &aKey, int32_t const &aValue)
        {
            return writeValueImpl(aKey, aValue);
        }
        template <typename T>
        bool CJSONSerializer<T>::writeValue(std::string const &aKey, int64_t const &aValue)
        {
            return writeValueImpl(aKey, aValue);
        }
        template <typename T>
        bool CJSONSerializer<T>::writeValue(std::string const &aKey, uint8_t const &aValue)
        {
            return writeValueImpl(aKey, aValue);
        }
        template <typename T>
        bool CJSONSerializer<T>::writeValue(std::string const &aKey, uint16_t const &aValue)
        {
            return writeValueImpl(aKey, aValue);
        }
        template <typename T>
        bool CJSONSerializer<T>::writeValue(std::string const &aKey, uint32_t const &aValue)
        {
            return writeValueImpl(aKey, aValue);
        }
        template <typename T>
        bool CJSONSerializer<T>::writeValue(std::string const &aKey, uint64_t const &aValue)
        {
            return writeValueImpl(aKey, aValue);
        }
        template <typename T>
        bool CJSONSerializer<T>::writeValue(std::string const &aKey, float const &aValue)
        {
            return writeValueImpl(aKey, aValue);
        }
        template <typename T>
        bool CJSONSerializer<T>::writeValue(std::string const &aKey, double const &aValue)
        {
            return writeValueImpl(aKey, aValue);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        CJSONSerializer<T>::CSerializationResult::CSerializationResult()
            : ISerializer<T>::IResult()
            , mResult({})
        { }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        CJSONSerializer<T>::CSerializationResult::CSerializationResult(nlohmann::json const &aResult)
            : ISerializer<T>::IResult()
            , mResult(aResult)
        { }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        bool CJSONSerializer<T>::CSerializationResult::asString(std::string &aOutString) const
        {
            aOutString = mResult.dump();

            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        bool CJSONSerializer<T>::CSerializationResult::asBinaryBuffer(std::vector<uint8_t> &aOutBuffer) const
        {
            aOutBuffer = nlohmann::json::to_msgpack(mResult);

            return true;
        }

        /**
         * The CJSONDeserializer class implements material deserialization from JSON formatted files.
         */
        template <typename T>
        class SHIRABE_TEST_EXPORT CJSONDeserializer
                : public IJSONDeserializer<T>
        {
            SHIRABE_DECLARE_LOG_TAG(CJSONDeserializer);

        public_destructors:
            virtual ~CJSONDeserializer() = default;

        public_structs:
            /*!
             * The IResult interface of the IDeserializer<T> interface declares required
             * signatures for result retrieval from a serialization process.
             */
            class SHIRABE_TEST_EXPORT CDeserializationResult
                    : public IDeserializer<T>::IResult
            {
            public_constructors:
                CDeserializationResult();

                CDeserializationResult(CStdSharedPtr_t<T> const &aResult);

            public_destructors:
                virtual ~CDeserializationResult() = default;

            public_methods:
                bool asT(CStdSharedPtr_t<T> &aOutMaterial) const;

            private_members:
                CStdSharedPtr_t<T> mResult;
            };

        public_methods:
            /**
             * Initialize the serializer and prepare for serialization calls.
             *
             * @return True, if successful. False otherwise.
             */
            bool initialize();

            /**
             * Cleanup and shutdown...
             *
             * @return True, if successful. False otherwise.
             */
            bool deinitialize();

            /*!
             * Serialize an instance of type SShaderCompilationUnit into whichever internal representation and
             * provide it using a pointer to IResult.
             *
             * @param aInput  Input data for serialization.
             * @param aOutResult Result-Instance holding the serialized data, providing access to
             *                   it in various output formats.
             * @return
             */
            bool deserialize(std::string const &aSource, CStdSharedPtr_t<typename IDeserializer<T>::IResult> &aOutResult);

            /*!
             * Serialize an instance of type SShaderCompilationUnit into whichever internal representation and
             * provide it using a pointer to IResult.
             *
             * @param aInput  Input data for serialization.
             * @param aOutResult Result-Instance holding the serialized data, providing access to
             *                   it in various output formats.
             * @return
             */
            bool deserialize(std::vector<uint8_t> const &aSource, CStdSharedPtr_t<typename IDeserializer<T>::IResult> &aOutResult);

            /**
             * Begin a JSON array, to which the upcoming objects will be added.
             *
             * @param aName Name of the array to begin.
             * @return
             */
            bool beginArray(std::string const &aName, uint32_t &aOutArraySize);

            /**
             * End the current array. If any.
             * @return
             */
            bool endArray();

            /**
             * Begin a new object w/ the given name.
             *
             * @param aName
             * @return
             */
            bool beginObject(std::string const &aName);

            /**
             * Begin a new object w/ the given name.
             *
             * @param aName
             * @return
             */
            bool beginObject(uint32_t const &aIndex);

            /**
             * End the current object, if any.
             *
             * @return
             */
            bool endObject();

            /**
             * Write a string value to the current object/array.
             *
             * @param aKey
             * @param aValue
             * @return
             */
            bool readValue(std::string const &aKey, std::string &aOutValue);
            bool readValue(std::string const &aKey, int8_t      &aOutValue);
            bool readValue(std::string const &aKey, int16_t     &aOutValue);
            bool readValue(std::string const &aKey, int32_t     &aOutValue);
            bool readValue(std::string const &aKey, int64_t     &aOutValue);
            bool readValue(std::string const &aKey, uint8_t     &aOutValue);
            bool readValue(std::string const &aKey, uint16_t    &aOutValue);
            bool readValue(std::string const &aKey, uint32_t    &aOutValue);
            bool readValue(std::string const &aKey, uint64_t    &aOutValue);
            bool readValue(std::string const &aKey, float       &aOutValue);
            bool readValue(std::string const &aKey, double      &aOutValue);

        private_methods:

            /**
             *
             *
             * @param aKey
             * @param aValue
             * @return
             */
            template <typename TValue>
            bool readValueImpl(std::string const &aKey, TValue &aOutValue)
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
                    // Invalid use!
                    CLog::Error(logTag(), "Cannot read value '%0' from json entity, which is not an object.", aKey);
                    return false;
                }
                else
                {
                    nlohmann::json const &valueNode = top[aKey];
                    TValue value = valueNode.is_null() ? TValue() : static_cast<TValue>(valueNode);
                    aOutValue = value;
                }

                return true;
            }

            /**
             * Check, whether there's a current top level item.
             *
             * @return
             */
            bool hasCurrentItem() const
            {
                return (not mCurrentJSONState.empty());
            }

            /**
             * Return a reference to the current top level item.
             *
             * @return
             */
            SHIRABE_INLINE
            nlohmann::json &getCurrentItem()
            {
                return mCurrentJSONState.top().get();
            }

        private_members:

            nlohmann::json                                     mRoot;
            std::stack<std::reference_wrapper<nlohmann::json>> mCurrentJSONState;
        };

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename T>
        bool CJSONDeserializer<T>::initialize()
        {
            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        bool CJSONDeserializer<T>::deinitialize()
        {
            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        bool CJSONDeserializer<T>::deserialize(std::string const &aSource, CStdSharedPtr_t<typename IDeserializer<T>::IResult> &aOutResult)
        {
            nlohmann::json json = nlohmann::json::parse(aSource);
            mRoot = json;
            mCurrentJSONState.push(mRoot);

            IJSONDeserializer<T> &deserializer = *this;

            auto material = makeCStdSharedPtr<T>();

            bool const successful = material->acceptDeserializer(deserializer);
            if(successful)
            {
                auto result = makeCStdSharedPtr<CDeserializationResult>(material);

                aOutResult = std::move(result);
            }

            return successful;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        bool CJSONDeserializer<T>::deserialize(std::vector<uint8_t> const &aSource, CStdSharedPtr_t<typename IDeserializer<T>::IResult> &aOutResult)
        {
            nlohmann::json json = nlohmann::json::from_msgpack(aSource);
            mRoot = json;
            mCurrentJSONState.push(mRoot);

            IJSONDeserializer<T> &deserializer = *this;

            auto material = makeCStdSharedPtr<T>();

            bool const successful = material->acceptDeserializer(deserializer);
            if(successful)
            {
                auto result = makeCStdSharedPtr<CDeserializationResult>(material);

                aOutResult = std::move(result);
            }

            return successful;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        bool CJSONDeserializer<T>::beginArray(std::string const &aName, uint32_t &aOutArraySize)
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

            aOutArraySize = static_cast<uint32_t>(arr.size());
            mCurrentJSONState.push(arr);

            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        bool CJSONDeserializer<T>::endArray()
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
        template <typename T>
        bool CJSONDeserializer<T>::beginObject(std::string const &aName)
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
                // Invalid use!
                CLog::Error(logTag(), "Cannot read object '%0'. Incompatible types.", aName);
                return false;
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
        template <typename T>
        bool CJSONDeserializer<T>::beginObject(uint32_t const &aIndex)
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
                nlohmann::json &obj = top.at(aIndex); // TODO: How to properly read array.
                if(not obj.is_object())
                {
                    // Invalid use
                    CLog::Error(logTag(), "Cannot read object. Incompatible types.");
                    return false;
                }

                mCurrentJSONState.push(obj);
            }
            else
            {
                // Invalid use!
                CLog::Error(logTag(), "Cannot read object, since current item is no array.");
                return false;
            }

            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        bool CJSONDeserializer<T>::endObject()
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
        template <typename T>
        bool CJSONDeserializer<T>::readValue(std::string const &aKey, std::string &aOutValue)
        {
            return readValueImpl(aKey, aOutValue);
        }

        template <typename T>
        bool CJSONDeserializer<T>::readValue(std::string const &aKey, int8_t &aOutValue)
        {
            return readValueImpl(aKey, aOutValue);
        }

        template <typename T>
        bool CJSONDeserializer<T>::readValue(std::string const &aKey, int16_t &aOutValue)
        {
            return readValueImpl(aKey, aOutValue);
        }

        template <typename T>
        bool CJSONDeserializer<T>::readValue(std::string const &aKey, int32_t &aOutValue)
        {
            return readValueImpl(aKey, aOutValue);
        }

        template <typename T>
        bool CJSONDeserializer<T>::readValue(std::string const &aKey, int64_t &aOutValue)
        {
            return readValueImpl(aKey, aOutValue);
        }

        template <typename T>
        bool CJSONDeserializer<T>::readValue(std::string const &aKey, uint8_t &aOutValue)
        {
            return readValueImpl(aKey, aOutValue);
        }

        template <typename T>
        bool CJSONDeserializer<T>::readValue(std::string const &aKey, uint16_t &aOutValue)
        {
            return readValueImpl(aKey, aOutValue);
        }

        template <typename T>
        bool CJSONDeserializer<T>::readValue(std::string const &aKey, uint32_t &aOutValue)
        {
            return readValueImpl(aKey, aOutValue);
        }

        template <typename T>
        bool CJSONDeserializer<T>::readValue(std::string const &aKey, uint64_t &aOutValue)
        {
            return readValueImpl(aKey, aOutValue);
        }

        template <typename T>
        bool CJSONDeserializer<T>::readValue(std::string const &aKey, float &aOutValue)
        {
            return readValueImpl(aKey, aOutValue);
        }

        template <typename T>
        bool CJSONDeserializer<T>::readValue(std::string const &aKey, double &aOutValue)
        {
            return readValueImpl(aKey, aOutValue);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        CJSONDeserializer<T>::CDeserializationResult::CDeserializationResult()
            : IDeserializer<T>::IResult()
            , mResult(nullptr)
        { }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        CJSONDeserializer<T>::CDeserializationResult::CDeserializationResult(CStdSharedPtr_t<T> const &aResult)
            : IDeserializer<T>::IResult()
            , mResult(aResult)
        { }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        bool CJSONDeserializer<T>::CDeserializationResult::asT(CStdSharedPtr_t<T> &aOutMaterial) const
        {
            aOutMaterial = mResult;

            return true;
        }
        //<-----------------------------------------------------------------------------

    }
}

#endif
