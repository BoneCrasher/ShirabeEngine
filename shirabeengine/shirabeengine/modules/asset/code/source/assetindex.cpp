#include <experimental/filesystem>

#include <core/string.h>
#include "asset/assetindex.h"

namespace engine
{
    namespace asset
    {

        namespace xml = engine::documents;

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        /**
         * Helper function to read an asset registry from a file.
         *
         * @param aFile        The XML-document containing all asset data.
         * @param aOutRegistry Filled up registry output handle
         */
        void __readAssets(
                xml::CXMLDocument      const &aFile,
                CAssetRegistry<SAsset>       &aOutRegistry);
        /**
         * Read an asset entry from XML.
         *
         * @param aAsset       XML-node pointer to a list of assets.
         * @param aFile        The XML-document containing the above node pointer.
         * @param aOutRegistry Filled up registry output handle
         */
        void __readAsset(
                xmlNodePtr             const &aAsset,
                xml::CXMLDocument      const &aFile,
                CAssetRegistry<SAsset>       &aOutRegistry);
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CAssetRegistry<SAsset> CAssetIndex::loadIndexById(std::string const&indexId)
        {
            namespace fs  = std::experimental::filesystem;

            CAssetRegistry<SAsset> reg = {};

            std::string const filename = CString::format("./assets/%0.assetindex.xml", indexId);

            xml::CXMLDocument          file  = {};
            xml::EXMLDocumentOpenState state = xml::EXMLDocumentOpenState::FILE_OK;

            state = file.openFile(filename);
            switch(state)
            {
            case xml::EXMLDocumentOpenState::FILE_NOT_FOUND:
            case xml::EXMLDocumentOpenState::FILE_ERROR:
            case xml::EXMLDocumentOpenState::FILE_EMPTY:
                break;
            case xml::EXMLDocumentOpenState::FILE_OK:
                __readAssets(file, reg);
                break;
            }

            return reg;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        void __readAssets(
                xml::CXMLDocument      const &aFile,
                CAssetRegistry<SAsset>       &aOutRegistry)
        {
            std::string   const path   = CString::format("/Index/Asset");
            xmlNodeSetPtr const assets = aFile.xmlSelect(path);

            for(uint32_t k=0; k<assets->nodeNr; ++k)
            {
                xmlNodePtr const asset = assets->nodeTab[k];
                __readAsset(asset, aFile, aOutRegistry);
            }
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        void __readAsset(
                xmlNodePtr             const &aAsset,
                xml::CXMLDocument      const &aFile,
                CAssetRegistry<SAsset>       &aOutRegistry)
        {
            std::string const aid  = CString::format("%0", (unsigned char*)xmlGetProp(aAsset, (const xmlChar *)"aid"));
            std::string const type = CString::format("%0", (unsigned char*)xmlGetProp(aAsset, (const xmlChar *)"type"));
            std::string const uri  = CString::format("%0", (unsigned char*)xmlGetProp(aAsset, (const xmlChar *)"uri"));

            SAsset a = {};
            a.id     = from_string<AssetId_t>(aid);
            a.type   = from_string<EAssetType>(type);
            a.URI    = uri;

            aOutRegistry.addAsset(a.id, a);
        }
        //<-----------------------------------------------------------------------------
    }
}
