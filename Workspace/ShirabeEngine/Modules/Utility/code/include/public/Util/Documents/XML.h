#ifndef __SHIRABE_XML_H__
#define __SHIRABE_XML_H__

#include <memory>
#include <string>
#include <map>
#include <vector>
#include <exception>

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>

namespace Engine {
  namespace Documents {
          
    class XMLDocumentSelectError
      : public std::exception
    {
    public:
      XMLDocumentSelectError(std::string const& what);
    };

    enum class XMLDocumentOpenState {
      FILE_OK        = 0,
      FILE_NOT_FOUND = 1,
      FILE_EMPTY     = 2,
      FILE_ERROR     = 4
    };

    class XMLDocument {
    public:
      XMLDocument()  = default;
      ~XMLDocument();

      // To avoid uncontrolled access, prohibit copy
      // but allow moving it around to facilitate the 
      // use within a std::unique_ptr.
      XMLDocument(XMLDocument const&)            = delete;
      XMLDocument& operator=(XMLDocument const&) = delete;
      
      XMLDocumentOpenState openFile(std::string const&filename);
      XMLDocumentOpenState openInMemory(std::string const&data);

      bool close();

      bool isOpen() const;

      /**********************************************************************************************//**
       * \fn  xmlNodeSetPtr XMLDocument::xmlSelect(std::string const&, uint32_t const&expect = 0) const;
       *
       * \brief Select a set of nodes using an xpath-string and optionally a number of nodes expected.
       *
       * \param xpath  XPath-compliant path string.
       * \param expect (Optional) The expected number of nodes. 0 for "all".
       *
       * \return  An xmlNodeSetPtr.
       **************************************************************************************************/
      xmlNodeSetPtr xmlSelect(std::string const&xpath, uint32_t const&expect = 0) const;
      bool xmlWrite(std::string const&xpath, std::string const&value);
     
      xmlNodeSetPtr operator[](std::string const&xpath) { return xmlSelect(xpath, 0); }

    private:
      struct LibXml2State {
        xmlDocPtr  document;
        xmlNodePtr documentRoot;

        bool documentValid() const;
        bool empty() const;
      };

      LibXml2State&       xmlState();
      LibXml2State const& xmlState() const;

      XMLDocumentOpenState openImpl();

      LibXml2State m_libXml2State; // Link to libXml2 data structures and ecosystem
    };

  }
}

#endif