#ifndef __SHIRABE_JSON_H__
#define __SHIRABE_JSON_H__

#include <istream>
#include <stack>

#include "Platform/Platform.h"
#include "Core/EngineTypeHelper.h"

namespace Engine {
  namespace Documents {

    enum class JSONDocumentOpenState {
      FILE_OK        = 0,
      FILE_NOT_FOUND = 1,
      FILE_EMPTY     = 2,
      FILE_ERROR     = 4
    };

    struct JSONState;

    DeclareInterface(IJSONNode);
    public:
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

      virtual UniquePtr<IJSONNode> operator=(int8_t   const& value) = 0;
      virtual UniquePtr<IJSONNode> operator=(int16_t  const& value) = 0;
      virtual UniquePtr<IJSONNode> operator=(int32_t  const& value) = 0;
      virtual UniquePtr<IJSONNode> operator=(int64_t  const& value) = 0;
      virtual UniquePtr<IJSONNode> operator=(uint8_t  const& value) = 0;
      virtual UniquePtr<IJSONNode> operator=(uint16_t const& value) = 0;
      virtual UniquePtr<IJSONNode> operator=(uint32_t const& value) = 0;
      virtual UniquePtr<IJSONNode> operator=(uint64_t const& value) = 0;

      virtual UniquePtr<IJSONNode> operator=(float  const& value) = 0;
      virtual UniquePtr<IJSONNode> operator=(double const& value) = 0;

      virtual UniquePtr<IJSONNode> operator=(uint8_t const* value)     = 0;
      virtual UniquePtr<IJSONNode> operator=(std::string const& value) = 0;      
     DeclareInterfaceEnd(JSONNode);

    class SHIRABE_TEST_EXPORT JSONDocument {
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

      UniquePtr<IJSONNode> focusChild(std::string const&key);
      UniquePtr<IJSONNode> focusParent();

      UniquePtr<IJSONNode> operator[](std::string const&key);

    private:
      struct JSONState {
        std::unique_ptr<std::istream, std::function<void(std::istream*)>> stream;
        nlohmann::json                                     jsonRoot;
        std::stack<std::reference_wrapper<nlohmann::json>> jsonPathStack;
      };

      UniquePtr<JSONState>&       jsonState();
      UniquePtr<JSONState> const& jsonState() const;

      JSONDocumentOpenState openImpl();

      UniquePtr<JSONState> m_jsonState;
    };

  }
}

#endif