#include <sstream>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <Functional>
#include <Memory>

#include <nlohmann/json.hpp>

#include "Util/Documents/JSON.h"

namespace Engine {
  namespace Documents {
    
    JSONDocument::~JSONDocument() {
      close();
    }

    JSONDocument::JSONState&
      JSONDocument::jsonState()
    {
      return m_jsonState;
    }

    JSONDocument::JSONState const&
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

      std::unique_ptr<std::istream> strm = std::make_unique<std::istringstream>(data);
      jsonState().stream = std::move(strm);

      return openImpl();
    }

    JSONDocumentOpenState
      JSONDocument::openFile(
        std::string const& filename)
    {
      namespace fs = std::experimental::filesystem;
      if(!fs::exists(fs::path(filename)))
        return JSONDocumentOpenState::FILE_NOT_FOUND;

      std::unique_ptr<std::istream> strm = std::make_unique<std::ifstream>(filename);

      if(!strm->good()) {
        strm = nullptr;
        return JSONDocumentOpenState::FILE_ERROR;
      }

      if(strm->peek() == std::ifstream::traits_type::eof()) {
        strm = nullptr;
        return JSONDocumentOpenState::FILE_EMPTY;
      }

      jsonState().stream = std::move(strm);

      return openImpl();
    }

    JSONDocumentOpenState
      JSONDocument::openImpl() {

      JSONDocumentOpenState state = JSONDocumentOpenState::FILE_OK;

      try {
        jsonState().jsonRoot = nlohmann::json::parse(*(jsonState().stream.get()));
        jsonState().jsonPathStack.push(std::reference_wrapper<nlohmann::json>(jsonState().jsonRoot));
      }
      catch(nlohmann::json::parse_error const&error) {
        state = JSONDocumentOpenState::FILE_ERROR;
      }

      jsonState().stream = nullptr;

      return state;
    }

    bool
      JSONDocument::close()
    {
      if(jsonState().stream)
        jsonState().stream = nullptr;

      jsonState().jsonRoot = nlohmann::json(nullptr);
      while(!jsonState().jsonPathStack.empty())
        jsonState().jsonPathStack.pop();

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
      nlohmann::json::iterator node = jsonState().jsonPathStack.top().get().find(key);
      return !(node == jsonState().jsonPathStack.top().get().end());
    }

    bool
      JSONDocument::focusChild(std::string const&key)
    {
      if(!focusedObjectContainsChild(key))
        return false;
        // throw std::out_of_range(key.c_str());
      
      if(!jsonState().jsonPathStack.top().get()[key].is_object())
        return false;
        // throw std::exception("Selected node is no json object.");

      jsonState().jsonPathStack.push(
        std::reference_wrapper<nlohmann::json>(
          jsonState().jsonPathStack.top().get()[key]));

      return true;
    }

    bool
      JSONDocument::focusParent()
    {
      if(jsonState().jsonPathStack.size() > 1) {
        jsonState().jsonPathStack.pop();
        return true;
      }

      return false;
    }

    bool
      JSONDocument::set(std::string const&key, )
    {
      if(!focusedObjectContainsChild(key))
        jsonState().jsonPathStack.top().get()[key] = nullptr;

      return jsonState().jsonPathStack.top().get()[key];
    }
  }
}