#include <sstream>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <functional>
#include <memory>
#include <optional>

#include <nlohmann/json.hpp>

#include "Util/Documents/JSON.h"

namespace engine {
  namespace Documents {

    class NLohmannJSONNode 
      : public IJSONNode
    {
    public:
      NLohmannJSONNode(std::reference_wrapper<nlohmann::json> const&ref)
        : m_ref(ref)
      {}

      operator int8_t  () const { return m_ref.get().operator int8_t  (); }
      operator int16_t () const { return m_ref.get().operator int16_t (); }
      operator int32_t () const { return m_ref.get().operator int32_t (); }
      operator int64_t () const { return m_ref.get().operator int64_t (); }
      operator uint8_t () const { return m_ref.get().operator uint8_t (); }
      operator uint16_t() const { return m_ref.get().operator uint16_t(); }
      operator uint32_t() const { return m_ref.get().operator uint32_t(); }
      operator uint64_t() const { return m_ref.get().operator uint64_t(); }

      operator float()  const { return m_ref.get().operator float();  }
      operator double() const { return m_ref.get().operator double(); }

      operator std::string() const { return m_ref.get().operator std::string();    }

      UniqueCStdSharedPtr_t<IJSONNode> operator=(int8_t   const& value);
      UniqueCStdSharedPtr_t<IJSONNode> operator=(int16_t  const& value);
      UniqueCStdSharedPtr_t<IJSONNode> operator=(int32_t  const& value);
      UniqueCStdSharedPtr_t<IJSONNode> operator=(int64_t  const& value);
      UniqueCStdSharedPtr_t<IJSONNode> operator=(uint8_t  const& value);
      UniqueCStdSharedPtr_t<IJSONNode> operator=(uint16_t const& value);
      UniqueCStdSharedPtr_t<IJSONNode> operator=(uint32_t const& value);
      UniqueCStdSharedPtr_t<IJSONNode> operator=(uint64_t const& value);

      UniqueCStdSharedPtr_t<IJSONNode> operator=(float  const& value);
      UniqueCStdSharedPtr_t<IJSONNode> operator=(double const& value);

      UniqueCStdSharedPtr_t<IJSONNode> operator=(uint8_t const* value);
      UniqueCStdSharedPtr_t<IJSONNode> operator=(std::string const& value);

    private:
      std::reference_wrapper<nlohmann::json> m_ref;
    };
    
    JSONDocument::~JSONDocument() {
      close();
    }

    UniqueCStdSharedPtr_t<JSONDocument::JSONState>&
      JSONDocument::jsonState()
    {
      return m_jsonState;
    }

    UniqueCStdSharedPtr_t<JSONDocument::JSONState> const&
      JSONDocument::jsonState() const
    {
      return m_jsonState;
    }


    JSONDocumentOpenState
      JSONDocument::openInMemory(
        std::string const& data)
    {
      using namespace nlohmann;

      if(data.empty())
        return JSONDocumentOpenState::FILE_EMPTY;

      CStdUniquePtr_t<std::istream> strm = std::make_unique<std::istringstream>(data);
      jsonState()->stream = std::move(strm);

      return openImpl();
    }

    JSONDocumentOpenState
      JSONDocument::openFile(
        std::string const& filename)
    {
      namespace fs = std::experimental::filesystem;
      if(!fs::exists(fs::path(filename)))
        return JSONDocumentOpenState::FILE_NOT_FOUND;

      CStdUniquePtr_t<std::istream> strm = std::make_unique<std::ifstream>(filename);

      if(!strm->good()) {
        strm = nullptr;
        return JSONDocumentOpenState::FILE_ERROR;
      }

      if(strm->peek() == std::ifstream::traits_type::eof()) {
        strm = nullptr;
        return JSONDocumentOpenState::FILE_EMPTY;
      }

      jsonState()->stream = std::move(strm);

      return openImpl();
    }

    JSONDocumentOpenState
      JSONDocument::openImpl() {

      JSONDocumentOpenState state = JSONDocumentOpenState::FILE_OK;

      try {
        jsonState()->jsonRoot = nlohmann::json::parse(*(jsonState()->stream.get()));
        jsonState()->jsonPathStack.push(std::reference_wrapper<nlohmann::json>(jsonState()->jsonRoot));
      }
      catch(nlohmann::json::parse_error const&error) {
        state = JSONDocumentOpenState::FILE_ERROR;
      }

      jsonState()->stream = nullptr;

      return state;
    }

    bool
      JSONDocument::close()
    {
      if(jsonState()->stream)
        jsonState()->stream = nullptr;

      jsonState()->jsonRoot = nlohmann::json(nullptr);
      while(!jsonState()->jsonPathStack.empty())
        jsonState()->jsonPathStack.pop();

      return true;
    }

    bool
      JSONDocument::isOpen() const
    {
      return true; // No file in memory, just the JSON object...
    }

    bool
      JSONDocument::focusedObjectContainsChild(std::string const&key)
    {
      nlohmann::json::iterator node = jsonState()->jsonPathStack.top().get().find(key);
      return !(node == jsonState()->jsonPathStack.top().get().end());
    }

    UniqueCStdSharedPtr_t<IJSONNode>
      JSONDocument::focusChild(std::string const&key)
    {
      if(!focusedObjectContainsChild(key))
        return UniqueCStdSharedPtr_t<IJSONNode>();
        // throw std::out_of_range(key.c_str());
      
      if(!jsonState()->jsonPathStack.top().get()[key].is_object())
        return UniqueCStdSharedPtr_t<IJSONNode>();

      jsonState()->jsonPathStack.push(
        std::reference_wrapper<nlohmann::json>(
          jsonState()->jsonPathStack.top().get()[key]));
      
      return UniqueCStdSharedPtr_t<IJSONNode>();
    }

    UniqueCStdSharedPtr_t<IJSONNode>
      JSONDocument::focusParent()
    {
      if(jsonState()->jsonPathStack.size() > 1) {
        jsonState()->jsonPathStack.pop();

        // Link to IJSONNode somehow?!
        return UniqueCStdSharedPtr_t<IJSONNode>();
      }

      return UniqueCStdSharedPtr_t<IJSONNode>();
    }

    UniqueCStdSharedPtr_t<IJSONNode> 
      JSONDocument::operator[](std::string const&key)
    {
      if(!focusedObjectContainsChild(key))
        return UniqueCStdSharedPtr_t<IJSONNode>();

      // Link to IJSONNode somehow?!
      return UniqueCStdSharedPtr_t<IJSONNode>();
    }

  }
}