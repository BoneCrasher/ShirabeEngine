#ifndef __SHIRABE_JSON_H__
#define __SHIRABE_JSON_H__

#include <istream>
#include <stack>

#include <nlohmann/json.hpp>
#include <platform/platform.h>
#include "core/enginetypehelper.h"

namespace engine
{
    namespace documents
    {

        /**
         * @brief The JSONDocumentOpenState enum
         */
        enum class JSONDocumentOpenState
        {
            FILE_OK        = 0,
            FILE_NOT_FOUND = 1,
            FILE_EMPTY     = 2,
            FILE_ERROR     = 4
        };

        struct JSONState;

        /**
         * @brief The IJSONNode class
         */
        class IJSONNode
        {
            SHIRABE_DECLARE_INTERFACE(IJSONNode);

        public_api:
            virtual operator int8_t  () const = 0;
            virtual operator int16_t () const = 0;
            virtual operator int32_t () const = 0;
            virtual operator int64_t () const = 0;
            virtual operator uint8_t () const = 0;
            virtual operator uint16_t() const = 0;
            virtual operator uint32_t() const = 0;
            virtual operator uint64_t() const = 0;

            virtual operator float()  const = 0;
            virtual operator double() const = 0;

            virtual operator std::string()    const = 0;

            virtual Unique<IJSONNode> operator=(int8_t   const& value) = 0;
            virtual Unique<IJSONNode> operator=(int16_t  const& value) = 0;
            virtual Unique<IJSONNode> operator=(int32_t  const& value) = 0;
            virtual Unique<IJSONNode> operator=(int64_t  const& value) = 0;
            virtual Unique<IJSONNode> operator=(uint8_t  const& value) = 0;
            virtual Unique<IJSONNode> operator=(uint16_t const& value) = 0;
            virtual Unique<IJSONNode> operator=(uint32_t const& value) = 0;
            virtual Unique<IJSONNode> operator=(uint64_t const& value) = 0;

            virtual Unique<IJSONNode> operator=(float  const& value) = 0;
            virtual Unique<IJSONNode> operator=(double const& value) = 0;

            virtual Unique<IJSONNode> operator=(uint8_t const* value)     = 0;
            virtual Unique<IJSONNode> operator=(std::string const& value) = 0;
        };

        /**
         * @brief The JSONDocument class
         */
        class SHIRABE_TEST_EXPORT JSONDocument
        {
        public:
            JSONDocument()  = default;
            ~JSONDocument();

            // To avoid uncontrolled access, prohibit copy
            // but allow moving it around to facilitate the
            // use within a std::unique_ptr.
            JSONDocument(JSONDocument const&)            = delete;
            JSONDocument& operator=(JSONDocument const&) = delete;

            JSONDocumentOpenState openFile(std::string const&filename);
            JSONDocumentOpenState openInMemory(std::string const&data);

            bool close();

            bool isOpen() const;

            bool focusedObjectContainsChild(std::string const&key);

            Unique<IJSONNode> focusChild(std::string const&key);
            Unique<IJSONNode> focusParent();

            Unique<IJSONNode> operator[](std::string const&key);

        private:
            struct JSONState {
                Unique<std::istream, std::function<void(std::istream*)>> stream;
                nlohmann::json                                     jsonRoot;
                std::stack<std::reference_wrapper<nlohmann::json>> jsonPathStack;
            };

            Unique<JSONState>&       jsonState();
            Unique<JSONState> const& jsonState() const;

            JSONDocumentOpenState openImpl();

            Unique<JSONState> mJSONState;
        };

    }
}

#endif
