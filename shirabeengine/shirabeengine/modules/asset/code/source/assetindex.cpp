#include <experimental/filesystem>

#include "Core/String.h"

#include "Asset/AssetIndex.h"

namespace Engine {
  namespace Asset {

    namespace xml = Engine::Documents;

    void __readAssets(xml::XMLDocument const&file, AssetRegistry<Asset> &registry);
    void __readAsset(xmlNodePtr        const&assets, xml::XMLDocument const&file, AssetRegistry<Asset> &registry);

    AssetRegistry<Asset>
      AssetIndex::loadIndexById(std::string const&indexId)
    {
      namespace fs  = std::experimental::filesystem;

      AssetRegistry<Asset> reg{ };

      std::string filename = String::format("./assets/%0.assetindex.xml", indexId);

      xml::XMLDocument file{};
      xml::XMLDocumentOpenState state = xml::XMLDocumentOpenState::FILE_OK;

      state = file.openFile(filename);
      switch(state) {
      case xml::XMLDocumentOpenState::FILE_NOT_FOUND:
      case xml::XMLDocumentOpenState::FILE_ERROR:
      case xml::XMLDocumentOpenState::FILE_EMPTY:
      default:
        break;
      case xml::XMLDocumentOpenState::FILE_OK:
        __readAssets(file, reg);
        break;
      }

      return reg;
    }

    void __readAssets(
      xml::XMLDocument     const&file,
      AssetRegistry<Asset>      &registry)
    {
      std::string path = String::format("/Index/Asset");

      xmlNodeSetPtr assets = file.xmlSelect(path);
      for(uint32_t k=0; k<assets->nodeNr; ++k) {
        xmlNodePtr asset = assets->nodeTab[k];
        __readAsset(asset, file, registry);
      }
    }

    void __readAsset(
      xmlNodePtr           const&asset,
      xml::XMLDocument     const&file,
      AssetRegistry<Asset>      &registry)
    {
      std::string aid  = String::format("%0", (unsigned char*)xmlGetProp(asset, (const xmlChar *)"aid"));
      std::string type = String::format("%0", (unsigned char*)xmlGetProp(asset, (const xmlChar *)"type"));
      std::string uri  = String::format("%0", (unsigned char*)xmlGetProp(asset, (const xmlChar *)"uri"));

      Asset a{};
      a.id   = from_string<AssetId_t>(aid);
      a.type = from_string<AssetType>(type);
      a.URI  = uri;

      registry.addAsset(a.id, a);
    }
  }
}