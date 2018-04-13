#ifndef __SHIRABE_JSON_H__
#define __SHIRABE_JSON_H__

#include <istream>
#include <stack>

#include "External/nlohmann/json.hpp"

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

      nlohmann::json& focusChild(std::string const&key);
      nlohmann::json& focusParent();

      nlohmann::json& operator[](std::string const&key);

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