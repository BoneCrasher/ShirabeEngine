#include <sstream>
#include <iostream>

#include "Util/Documents/XML.h"

namespace engine {
  namespace documents {

    namespace xpath {
      enum class XPathType {
        PATH            = 1,
        PATH_VALUE      = 2,
        ATTRIBUTE       = 4,
        ATTRIBUTE_VALUE = 8
      };

      struct XPath {
        XPathType
          type;
        std::string
          path,
          path_value,
          attribute_key,
          attribute_value;
      };
   
      // https://regexr.com/3j62r
      static const std::string xpathMatchRegex
        = "((?:[/][\\d\\w]+)+)((?:[[]@([\\d\\w]+)=\'([\\d\\w]+)\'[\]])|(?:(?:=\'([\\d\\w]+)\')))";

      // Matches
      //   /This
      //   /Is/A
      //   /Is[@id='test']
      //   /Is='1212'
      // 
      // Does not match
      //   /Is[@id='test']=121
    }

    XMLDocumentSelectError::XMLDocumentSelectError(std::string const&what)
      : std::exception(what.c_str())
    {}

    XMLDocument::~XMLDocument() {
      close();
    }

    XMLDocument::LibXml2State&
      XMLDocument::xmlState()
    {
      return m_libXml2State;
    }

    XMLDocument::LibXml2State const&
      XMLDocument::xmlState() const
    {
      return m_libXml2State;
    }

    XMLDocumentOpenState
      XMLDocument::openInMemory(
        std::string const& data)
    {
      xmlState().document = xmlParseDoc((uint8_t *)data.c_str());
      return openImpl();
    }

    XMLDocumentOpenState
      XMLDocument::openFile(
        std::string const& filename)
    {
      xmlState().document = xmlParseFile(filename.c_str());
      return openImpl();
    }

    XMLDocumentOpenState
      XMLDocument::openImpl() {
      if(!xmlState().documentValid()) {
        // Error.
        return XMLDocumentOpenState::FILE_NOT_FOUND;
      }

      xmlDocPtr& document = xmlState().document;

      xmlState().documentRoot = xmlDocGetRootElement(document);
      if(xmlState().empty()) {
        // No data.
        close();
        return XMLDocumentOpenState::FILE_EMPTY;
      }

      return XMLDocumentOpenState::FILE_OK;
    }

    bool
      XMLDocument::close()
    {
      if(xmlState().document) {
        xmlFreeDoc(xmlState().document);
        xmlState().document = nullptr;
      }

      return true;
    }

    bool
      XMLDocument::isOpen() const
    {
      LibXml2State const& state = xmlState();
      return (state.document != nullptr);
    }

    bool
      XMLDocument::LibXml2State::documentValid() const
    {
      return (document != nullptr);
    }

    bool
      XMLDocument::LibXml2State::empty() const
    {
      return (documentRoot == nullptr);
    }

    xmlNodeSetPtr
      XMLDocument::xmlSelect(
        std::string const&selector,
        uint32_t    const&expected) const
    {
      try {
        xmlXPathContextPtr xpathContext;
        xmlXPathObjectPtr  xpathResult;

        xpathContext = xmlXPathNewContext(xmlState().document);
        xpathResult  = xmlXPathEvalExpression((xmlChar*)selector.c_str(), xpathContext);
        xmlXPathFreeContext(xpathContext);

        if(xmlXPathNodeSetIsEmpty(xpathResult->nodesetval)) {
          xmlXPathFreeObject(xpathResult);
          return nullptr;
        }

        if(expected > 0 && xpathResult->nodesetval->nodeNr != expected) {
          // Error, too many results
          std::stringstream ss;
          ss
            << "Too many results for selector "
            << "'" << selector << "'"
            << "(Found: " << xpathResult->nodesetval->nodeNr << "; Expected: " << expected << ")."
            << std::endl;
          throw XMLDocumentSelectError(ss.str());
        }

        return xpathResult->nodesetval;
      } catch(XMLDocumentSelectError xmlSelectError) {
        std::cout << "Failed to select value in XMLDocument::xmlSelect(...): " << xmlSelectError.what() << std::endl;
        return nullptr;
      } catch(std::exception exception) {
        std::cout << "An error occurred during XMLDocument::xmlSelect(...): " << exception.what() << std::endl;
        return nullptr;
      } catch(...) {
        std::cout << "An unknown error occurred during XMLConfig::selectValue(...)" << std::endl;
        return nullptr;
      }
    }
    
    bool
      XMLDocument::xmlWrite(
        std::string const&xpath,
        std::string const&value)
    {
      // Let's try something new... Use xpath to write!
      // 
      // To make it work we will restrict xpath though:
      //   "//" will be reduced to "/".
      // 
      // Otherwise, any other syntax is possible.

      return true;
    }

  }
}