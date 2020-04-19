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

#include <core/enginetypehelper.h>

namespace engine
{
    namespace documents
    {
        /**
         * The CXMLDocumentSelectError class wraps a std::runtime_error.
         */
        class CXMLDocumentSelectError
                : public std::runtime_error
        {
        public_constructors:
            CXMLDocumentSelectError(std::string const &aWhat);
        };

        /**
         * The EXMLDocumentOpenState enum indicates various states of interaction
         * with XML-files.
         */
        enum class EXMLDocumentOpenState
        {
            FILE_OK        = 0,
            FILE_NOT_FOUND = 1,
            FILE_EMPTY     = 2,
            FILE_ERROR     = 4
        };

        /**
         * The CXMLDocument class describes an XML document and all interaction with it.
         */
        class CXMLDocument
        {
        public_constructors:
            /**
             * Create an empty XML document.
             */
            CXMLDocument() = default;

            /**
             * Deny Copy-Construction, since one specific document is considered
             * unique during runtime.
             *
             * @param aOther Another instance we refuse to be copied.
             */
            CXMLDocument(CXMLDocument const &aOther) = delete;

            /**
             * Deny Copy-Assignment, since one specific document is considered
             * unique during runtime.
             *
             * @param aOther Another instance we refuse to be copied.
             */
            CXMLDocument &operator=(CXMLDocument const &aOther) = delete;

        public_destructors:
            /**
             * Destroy and run...
             */
            ~CXMLDocument();

        public_methods:
            /**
             * Open a file, if it exists and create an internal libxml2 state.
             *
             * @param aFilename The filename of the file to be opened.
             * @return          One of the flags of the EXMLDocumentOpenState enum.
             */
            EXMLDocumentOpenState openFile(std::string const &aFilename);

            /**
             * Read XML from 'memory' and create an internal libxml2 state.
             *
             * @param aData String-data containing plain-text XML data.
             * @return      One of the flags of the EXMLDocumentOpenState enum.
             */
            EXMLDocumentOpenState openInMemory(std::string const &aData);

            /**
             * Close the document and erase the xml2 state.
             *
             * @return True, if successful. False otherwise.
             */
            bool close();

            /**
             * Check, whether the document has a file opened.
             *
             * @return True if opened successfully. False otherwise.
             */
            bool isOpen() const;

            /**
             * Select a set of nodes using an xpath-string and optionally a number of nodes expected.
             *
             * @param aXpath  XPath-compliant path string.
             * @param aExpect (Optional) The expected number of nodes. 0 for "all".
             * @return        An xmlNodeSetPtr if found. nullptr otherwise.
             */
            xmlNodeSetPtr xmlSelect(
                    std::string const &aXpath,
                    uint32_t    const &aExpect = 0) const;

            /**
             * Write a value to the selected xpath.
             * Currently unimplemented.
             *
             * @param aXpath Location to write, identified by an xpath expression.
             * @param aValue Value to write.
             * @return       True, if successful. False otherwise.
             */
            bool xmlWrite(
                    std::string const &aXpath,
                    std::string const &aValue);

            /**
             * Try to read a value at 'aXPath'.
             *
             * @param aXpath XPath-compliant path string of the value to read.
             * @return       An xmlNodeSetPtr if found. nullptr otherwise.
             */
            xmlNodeSetPtr operator[](std::string const &aXpath)
            {
                xmlNodeSetPtr ptr = xmlSelect(aXpath, 0);
                return ptr;
            }

        private_structs:
            /**
             * The LibXml2State struct wraps all necessary state to interact
             * with libxm2 and xml documents.
             */
            struct LibXml2State
            {
            public_methods:
                /**
                 * Check, whether the active document is valid.
                 *
                 * @return See brief.
                 */
                bool documentValid() const;

                /**
                 * Check, whether the active document is empty.
                 *
                 * @return See brief.
                 */
                bool empty() const;

            public_members:
                xmlDocPtr  document;
                xmlNodePtr documentRoot;
            };

        private_members:
            LibXml2State&       xmlState();
            LibXml2State const& xmlState() const;

            EXMLDocumentOpenState openImpl();

            LibXml2State mLibXML2State; // Link to libXml2 data structures and ecosystem
        };

    }
}

#endif
