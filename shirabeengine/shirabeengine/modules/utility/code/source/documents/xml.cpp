#include <sstream>
#include <iostream>

#include "util/documents/xml.h"

namespace engine
{
    namespace documents
    {

        namespace xpath
        {
            /**
             * The EXPathType enum describes which kind of xpath we deal with.
             */
            enum class EXPathType
            {
                PATH            = 1,
                PATH_VALUE      = 2,
                ATTRIBUTE       = 4,
                ATTRIBUTE_VALUE = 8
            };

            /**
             * The SXPath struct wraps all possible result data of an xpath select op.
             */
            struct SXPath
            {
                EXPathType  type;
                std::string path;
                std::string path_value;
                std::string attribute_key;
                std::string attribute_value;
            };

            // https://regexr.com/3j62r
            static const std::string xpathMatchRegex = "((?:[/][\\d\\w]+)+)((?:[[]@([\\d\\w]+)=\'([\\d\\w]+)\'[\\]])|(?:(?:=\'([\\d\\w]+)\')))";

            // Matches
            //   /This
            //   /Is/A
            //   /Is[@id='test']
            //   /Is='1212'
            //
            // Does not match
            //   /Is[@id='test']=121
        }

        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CXMLDocumentSelectError::CXMLDocumentSelectError(std::string const &aWhat)
            : std::runtime_error(aWhat.c_str())
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CXMLDocument::~CXMLDocument()
        {
            close();
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CXMLDocument::LibXml2State &CXMLDocument::xmlState()
        {
            return mLibXML2State;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CXMLDocument::LibXml2State const &CXMLDocument::xmlState() const
        {
            return mLibXML2State;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EXMLDocumentOpenState CXMLDocument::openInMemory(std::string const &aData)
        {
            uint8_t const *data = ((uint8_t *)aData.c_str());
            xmlState().document = xmlParseDoc(data);
            return openImpl();
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EXMLDocumentOpenState CXMLDocument::openFile(std::string const &aFilename)
        {
            xmlState().document = xmlParseFile(aFilename.c_str());

            EXMLDocumentOpenState const state = openImpl();
            return state;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EXMLDocumentOpenState CXMLDocument::openImpl()
        {
            if(!xmlState().documentValid())
            {
                return EXMLDocumentOpenState::FILE_NOT_FOUND;
            }

            xmlDocPtr &document = xmlState().document;

            xmlState().documentRoot = xmlDocGetRootElement(document);
            if(xmlState().empty())
            {
                // No data.
                close();
                return EXMLDocumentOpenState::FILE_EMPTY;
            }

            return EXMLDocumentOpenState::FILE_OK;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CXMLDocument::close()
        {
            if(xmlState().document)
            {
                xmlFreeDoc(xmlState().document);
                xmlState().document = nullptr;
            }

            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CXMLDocument::isOpen() const
        {
            LibXml2State const &state  = xmlState();
            bool         const  hasDoc = (state.document != nullptr);

            return hasDoc;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CXMLDocument::LibXml2State::documentValid() const
        {
            bool const hasDoc = (document != nullptr);
            return hasDoc;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CXMLDocument::LibXml2State::empty() const
        {
            bool const hasRoot = (documentRoot == nullptr);
            return hasRoot;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        xmlNodeSetPtr CXMLDocument::xmlSelect(
                std::string const &aXpath,
                uint32_t    const &aExpect) const
        {
            try {
                xmlXPathContextPtr xpathContext{};
                xmlXPathObjectPtr  xpathResult{};

                xmlChar *path =(xmlChar*)aXpath.c_str();

                xpathContext = xmlXPathNewContext(xmlState().document);
                xpathResult  = xmlXPathEvalExpression(path, xpathContext);
                xmlXPathFreeContext(xpathContext);

                if(xmlXPathNodeSetIsEmpty(xpathResult->nodesetval))
                {
                    xmlXPathFreeObject(xpathResult);
                    return nullptr;
                }

                if(aExpect > 0 && static_cast<uint32_t>(xpathResult->nodesetval->nodeNr) != aExpect)
                {
                    // Error, too many results
                    std::stringstream ss;
                    ss
                            << "Too many results for selector "
                            << "'" << aXpath << "'"
                            << "(Found: " << xpathResult->nodesetval->nodeNr << "; Expected: " << aExpect << ")."
                            << std::endl;
                    throw CXMLDocumentSelectError(ss.str());
                }

                return xpathResult->nodesetval;
            }
            catch(CXMLDocumentSelectError xmlSelectError)
            {
                std::cout << "Failed to select value in XMLDocument::xmlSelect(...): " << xmlSelectError.what() << std::endl;
                return nullptr;
            }
            catch(std::exception exception)
            {
                std::cout << "An error occurred during XMLDocument::xmlSelect(...): " << exception.what() << std::endl;
                return nullptr;
            }
            catch(...)
            {
                std::cout << "An unknown error occurred during XMLConfig::selectValue(...)" << std::endl;
                return nullptr;
            }
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CXMLDocument::xmlWrite(
                std::string const &aXpath,
                std::string const &aValue)
        {
            // Let's try something new... Use xpath to write!
            //
            // To make it work we will restrict xpath though:
            //   "//" will be reduced to "/".
            //
            // Otherwise, any other syntax is possible.

            return true;
        }
        //<-----------------------------------------------------------------------------
    }
}
