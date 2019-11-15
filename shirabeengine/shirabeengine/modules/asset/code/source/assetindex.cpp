#include <filesystem>

#include <base/string.h>
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
                std::filesystem::path  const &aSourceDir,
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
                std::filesystem::path  const &aSourceDir,
                xmlNodePtr             const &aAsset,
                xml::CXMLDocument      const &aFile,
                CAssetRegistry<SAsset>       &aOutRegistry);
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CAssetRegistry<SAsset> CAssetIndex::loadIndexById(std::filesystem::path const &aIndexPath)
        {
            namespace fs  = std::filesystem;

            std::filesystem::path const parent = aIndexPath.parent_path();

            CAssetRegistry<SAsset> reg = {};

            xml::CXMLDocument          file  = {};
            xml::EXMLDocumentOpenState state = xml::EXMLDocumentOpenState::FILE_OK;

            state = file.openFile(aIndexPath);
            switch(state)
            {
            case xml::EXMLDocumentOpenState::FILE_NOT_FOUND:
            case xml::EXMLDocumentOpenState::FILE_ERROR:
            case xml::EXMLDocumentOpenState::FILE_EMPTY:
                break;
            case xml::EXMLDocumentOpenState::FILE_OK:
                __readAssets(parent, file, reg);
                break;
            }

            return reg;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        void __readAssets(
                std::filesystem::path  const &aSourceDir,
                xml::CXMLDocument      const &aFile,
                CAssetRegistry<SAsset>       &aOutRegistry)
        {
            std::string   const path   = CString::format("/Index/Asset");
            xmlNodeSetPtr const assets = aFile.xmlSelect(path);

            for(uint32_t k=0; k<assets->nodeNr; ++k)
            {
                xmlNodePtr const asset = assets->nodeTab[k];
                __readAsset(aSourceDir, asset, aFile, aOutRegistry);
            }
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        void __readAsset(
                std::filesystem::path  const &aSourceDir,
                xmlNodePtr             const &aAsset,
                xml::CXMLDocument      const &aFile,
                CAssetRegistry<SAsset>       &aOutRegistry)
        {
            auto const getProp = [] (xmlNodePtr aNode, char const *aPropId) -> std::string {
                char const *p = (char const *)xmlGetProp(aNode, (xmlChar const *)aPropId);
                std::string const value(p, strlen(p));
                xmlFree((void *)p);

                return value;
            };

            std::string const aid     = CString::format("{}", getProp(aAsset, "aid"));
            std::string const parent  = CString::format("{}", getProp(aAsset, "parent_aid"));
            std::string const type    = CString::format("{}", getProp(aAsset, "type"));
            std::string const subtype = CString::format("{}", getProp(aAsset, "subtype"));
            std::string const uri     = CString::format("{}", getProp(aAsset, "uri"));

            std::filesystem::path uriPath     = std::filesystem::path(uri).lexically_normal();
            std::filesystem::path composedURI = aSourceDir.lexically_normal();
            composedURI /= uriPath;

            SAsset a = {};
            a.id      = from_string<AssetId_t>(aid);
            a.parent  = from_string<AssetId_t>(parent);
            a.type    = from_string<EAssetType>(type);
            a.subtype = from_string<EAssetSubtype>(type);
            a.uri     = composedURI;

            aOutRegistry.addAsset(a.id, a);
        }
        //<-----------------------------------------------------------------------------
    }
}
