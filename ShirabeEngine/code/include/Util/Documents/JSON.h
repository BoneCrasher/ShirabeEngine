#ifndef __SHIRABE_JSON_H__
#define __SHIRABE_JSON_H__

#include <istream>
#include <nlohmann/json.hpp>

namespace Engine {
  namespace Documents {

    using namespace nlohmann;

    enum class JSONDocumentOpenState {
      FILE_OK        = 0,
      FILE_NOT_FOUND = 1,
      FILE_EMPTY     = 2,
      FILE_ERROR     = 4
    };

    class JSONDocument {
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

    private:
      struct JSONState {
        std::unique_ptr<std::istream, std::function<void(std::istream*)>> stream;
        nlohmann::json                json;
      };

      JSONState&       jsonState();
      JSONState const& jsonState() const;

      JSONDocumentOpenState openImpl();

      JSONState m_jsonState;
    };

  }
}

#endif