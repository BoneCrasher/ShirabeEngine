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

      if(data.empty)
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
        json interpreted = json::parse(*(jsonState().stream.get()));

        jsonState().json = interpreted;
      }
      catch(json::parse_error const&error) {
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

      jsonState().json.clear();

      return true;
    }

    bool
      JSONDocument::isOpen() const
    {
      return true; // No file in memory, just the JSON object...
    }

  }
}