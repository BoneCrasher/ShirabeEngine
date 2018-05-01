#include "Renderer/FrameGraph/GraphBuilder.h"

namespace Engine {
  namespace FrameGraph {

    class SequenceUIDGenerator
      : public IUIDGenerator<FrameGraphResourceId_t>
    {
    public:
      SequenceUIDGenerator()
        : m_id(0)
      {};

      inline FrameGraphResourceId_t generate() {
        return (++m_id);
      }

    private:
      FrameGraphResourceId_t m_id;
    };

    GraphBuilder::GraphBuilder()
      : m_passUIDGenerator(std::make_shared<SequenceUIDGenerator>())
      , m_resourceUIDGenerator(std::make_shared<SequenceUIDGenerator>())
      , m_frameGraph(nullptr)
      , m_importedResources()
    {
    }

    Ptr<IUIDGenerator<FrameGraphResourceId_t>>
      GraphBuilder::resourceUIDGenerator()
    {
      return m_resourceUIDGenerator;
    }

    FrameGraphResourceId_t
      GraphBuilder::generatePassUID()
    {
      return m_passUIDGenerator->generate();
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
      bool isTexture     = isResourceTexture(m_resources, resource);
      bool isTextureView = isResourceTextureView(m_resources, resource);

      if(!(isTexture || isTextureView))
        throw std::runtime_error("Texture or TextureView not found!");

      try {
        FrameGraphResourceData const&data = m_resources.at(resource.resourceId).data;

        if(isTexture)
          return std::get<FrameGraphTexture>(data);

        if(isTextureView) {
          FrameGraphResource parent;
          // Crawl back to the underlying resource of the view chain
          while(parent.parentResource > 0)
            parent = m_resources.at(parent.parentResource);

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
      bool isTextureView = isResourceTextureView(m_resources, resource);
      if(!isTextureView)
        throw std::runtime_error("TextureView not found!");

      try {
        FrameGraphResourceData const&data = m_resources.at(resource.resourceId).data;
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

    template <typename T>
    std::optional<T&> getResource(FrameGraphResourceId_t const&id) {
      if((m_resources.find(id) == m_resources.end()))
        throw std::runtime_error("Resource not found");

      try {
        std::optional<T&> optional = std::get<T>(m_resources.at(id));
        return optional;
      }
      catch(std::bad_variant_access const&bve) {
        Log::Error(logTag(), bve.what());
        return std::optional<T&>();
      }
      catch(std::runtime_error const&rte) {
        Log::Error(logTag(), rte.what());
        return std::optional<T&>();
      }
    }

    bool
      GraphBuilder::collectPass(PassBuilder const&passBuilder)
    {
      FrameGraphResourceMap const&resources = passBuilder.m_resources;

      m_resources.insert(resources.begin(), resources.end());

      // Derive:
      // - Resource creation requests.
      // - Edges: pass->pass and resource[view]->resource[view] for graph generation!
      // - ???
      for(FrameGraphResourceMap::value_type const&resource : resources)
      {
        FrameGraphResource const&r = resource.second;

        // This block will only be executed, if the resource being processed is no root resource.
        if(r.parentResource > 0) {
          FrameGraphResource const&sourceResourcePd = m_resources.at(r.parentResource);
          if(sourceResourcePd.assignedPassUID != r.assignedPassUID)
            m_passAdjacency[sourceResourcePd.assignedPassUID].push_back(r.assignedPassUID);
        }
      }

      return true;
    }
  }
}