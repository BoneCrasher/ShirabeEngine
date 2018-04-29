#include "Renderer/FrameGraph/GraphBuilder.h"

namespace Engine {
  namespace FrameGraph {

    GraphBuilder::GraphBuilder()
      : m_uidGenerator(std::make_shared<Random::RandomState>())
      , m_frameGraph(nullptr)
      , m_importedResources()
    {}

    Ptr<Random::RandomState>
      GraphBuilder::resourceUIDGenerator()
    {
      return m_uidGenerator;
    }

    FrameGraphResourceId_t
      GraphBuilder::generatePassUID()
    {
      return m_uidGenerator->next();
    }

    /**********************************************************************************************//**
     * \fn  bool GraphBuilder::initialize()
     *
     * \brief Initializes this GraphBuilder
     *
     * \return  True if it succeeds, false if it fails.
     **************************************************************************************************/
    bool
      GraphBuilder::initialize(
        Ptr<ApplicationEnvironment> const&environment)
    {
      assert(environment != nullptr);
      m_applicationEnvironment = environment;

      graph() = MakeUniquePointerType<FrameGraph>();

      return true;
    }

    /**********************************************************************************************//**
     * \fn  bool GraphBuilder::deinitialize()
     *
     * \brief De-initialises this GraphBuilder and frees any resources it is using
     *
     * \return  True if it succeeds, false if it fails.
     **************************************************************************************************/
    bool
      GraphBuilder::deinitialize()
    {
      if(graph())
        graph() = nullptr;

      return true;
    }

    Ptr<ApplicationEnvironment>
      GraphBuilder::getApplicationEnvironment()
    {
      return m_applicationEnvironment;
    }


    /**********************************************************************************************//**
     * \fn  bool GraphBuilder::importPersistentResource( std::string const&id, PublicResourceId_t const&resourceId)
     *
     * \brief Import persistent resource
     *
     * \param id          The identifier.
     * \param resourceId  Identifier for the resource.
     *
     * \return  True if it succeeds, false if it fails.
     **************************************************************************************************/
    bool
      GraphBuilder::importPersistentResource(
        std::string        const&id,
        PublicResourceId_t const&resourceId)
    {
      try {
        bool alreadyAvailableForId = !(importedResources().find(id) == importedResources().end());

        if(!alreadyAvailableForId)
          importedResources()[id] = resourceId;

        return (!alreadyAvailableForId);
      }
      catch(std::exception) {
        return false;
      }
      catch(...) {
        return false;
      }
    }

    FrameGraphTexture const&
      GraphBuilder::getTextureData(FrameGraphResource const&resource) const
    {
      bool isTexture     = isResourceTexture(m_resourcesPrivateData, resource);
      bool isTextureView = isResourceTextureView(m_resourcesPrivateData, resource);

      if(!(isTexture || isTextureView))
        throw std::runtime_error("Texture or TextureView not found!");

      try {
        FrameGraphResourceData        const&data            = m_resources.at(resource.resourceId);
        FrameGraphResourcePrivateData const&viewPrivateData = m_resourcesPrivateData.at(resource.resourceId);

        if(isTexture)
          return std::get<FrameGraphTexture>(data);

        if(isTextureView) {
          FrameGraphResource parent{};
          parent.resourceId = viewPrivateData.parentResourceId;
          return getTextureData(parent);
        }          
      }
      catch(std::bad_variant_access const&bve) {
        Log::Error(logTag(),
          "Inconsistent graph builder state."
          "Tried to access texture or textureview as defined by private data, "
          "but the data stored in the resource map is no texture or textureview.");
        throw;
      }
    }

    FrameGraphTextureView const&
      GraphBuilder::getTextureViewData(FrameGraphResource const&resource) const
    {
      bool isTextureView = isResourceTextureView(m_resourcesPrivateData, resource);
      if(!isTextureView)
        throw std::runtime_error("TextureView not found!");

      try {
        FrameGraphResourceData const&data = m_resources.at(resource.resourceId);
        return std::get<FrameGraphTextureView>(data);
      }
      catch(std::bad_variant_access const&bve) {
        Log::Error(logTag(),
          "Inconsistent graph builder state."
          "Tried to access texture view as defined by private data, but the "
          "data stored in the resource map is no texture or textureview.");
        throw;
      }
    }


    /**********************************************************************************************//**
     * \fn  UniquePtr<FrameGraph> GraphBuilder::compile()
     *
     * \brief Gets the compile
     *
     * \return  An UniquePtr&lt;FrameGraph&gt;
     **************************************************************************************************/
    UniquePtr<FrameGraph>
      GraphBuilder::compile()
    {
      // Compilation is extremely tricky, as we possibly end up with 
      // multiple graph root nodes.


      return nullptr;
    }

    UniquePtr<FrameGraph>&
      GraphBuilder::graph()
    {
      return m_frameGraph;
    }

    Map<std::string, PublicResourceId_t>&
      GraphBuilder::importedResources()
    {
      return m_importedResources;
    }

    bool
      GraphBuilder::collectPass(PassBuilder const&passBuilder)
    {
      FrameGraphResourceDataMap        const&resources   = passBuilder.m_resources;
      FrameGraphResourcePrivateDataMap const&privateData = passBuilder.m_resourcesPrivateData;

      m_resources.insert(resources.begin(), resources.end());
      m_resourcesPrivateData.insert(privateData.begin(), privateData.end());



      return true;
    }
  }
}