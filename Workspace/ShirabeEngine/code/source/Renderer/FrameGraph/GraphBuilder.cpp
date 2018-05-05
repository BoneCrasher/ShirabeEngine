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

      graph() = MakeUniquePointerType<Graph>();

      // Add dummy pass with UID#0
      // m_passes[0] = spawnPass<PassBase>("Pre-Pass");

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
     * \fn  bool GraphBuilder::importExternalResource( std::string const&resourceName, PublicResourceId_t const&resourceId)
     *
     * \brief Import persistent resource
     *
     * \param resourceName          The identifier.
     * \param resourceId  Identifier for the resource.
     *
     * \return  True if it succeeds, false if it fails.
     **************************************************************************************************/
    FrameGraphResource
      GraphBuilder::registerTexture(
        std::string       const&readableName,
        FrameGraphTexture const&texture)
    {
      try {

        FrameGraphResource resource{};
        resource.assignedPassUID    = 0; // Pre-Pass
        resource.resourceId         = m_resourceUIDGenerator->generate();
        resource.parentResource     = 0; // No internal tree, has to be resolved differently.
        resource.type               = FrameGraphResourceType::Texture;
        resource.readableName       = readableName;
        resource.data               = texture;
        resource.isExternalResource = true;

        m_resources[resource.resourceId] = resource;

        return resource;
      }
      catch(std::exception) {
        return FrameGraphResource{};
      }
      catch(...) {
        return FrameGraphResource{};
      }
    }

    /**********************************************************************************************//**
     * \fn  FrameGraphTexture const& GraphBuilder::getTextureData(FrameGraphResource const&resource) const
     *
     * \brief Gets texture data
     *
     * \exception std::runtime_error      Raised when a runtime error condition occurs.
     * \exception std::bad_variant_access Thrown when a bad variant access error condition occurs.
     *
     * \param resource  The resource.
     *
     * \return  The texture data.
     **************************************************************************************************/
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
          FrameGraphResource parent = m_resources.at(resource.parentResource);
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

    /**********************************************************************************************//**
     * \fn  FrameGraphTextureView const& GraphBuilder::getTextureViewData(FrameGraphResource const&resource) const
     *
     * \brief Gets texture view data
     *
     * \exception std::runtime_error      Raised when a runtime error condition occurs.
     * \exception std::bad_variant_access Thrown when a bad variant access error condition occurs.
     *
     * \param resource  The resource.
     *
     * \return  The texture view data.
     **************************************************************************************************/
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
     * \fn  UniquePtr<Graph> GraphBuilder::compile()
     *
     * \brief Gets the compile
     *
     * \return  An UniquePtr&lt;Graph&gt;
     **************************************************************************************************/
    UniquePtr<Graph>
      GraphBuilder::compile()
    {
      bool topologicalSortSuccessful = topologicalSort(graph()->m_passExecutionOrder);
      if(!topologicalSortSuccessful) {
        Log::Error(logTag(), "Failed to perform topologicalSort(...) on graph compilation.");
        return nullptr;
      }

      #if defined SHIRABE_DEBUG || defined SHIRABE_TEST 

      bool validationSuccessful = validate(graph()->m_passExecutionOrder);
      if(!validationSuccessful) {
        Log::Error(logTag(), "Failed to perform validation(...) on graph compilation.");
        return nullptr;
      }

      #endif

      // Move out the current adjacency state to the frame graph, so that it can be used for further processing.
      // It is no more needed at this point within the GraphBuilder.

      std::stack<PassUID_t> order = graph()->m_passExecutionOrder;
      while(!order.empty()) {
        PassUID_t uid = order.top();
        if(uid > 0)
          graph()->addPass(m_passes.at(uid));
        order.pop();
      }

      graph()->m_passAdjacency = std::move(this->m_passAdjacency);

      return std::move(graph());
    }

    /**********************************************************************************************//**
     * \fn  UniquePtr<Graph>& GraphBuilder::graph()
     *
     * \brief Gets the graph
     *
     * \return  A reference to an UniquePtr&lt;Graph&gt;
     **************************************************************************************************/
    UniquePtr<Graph>&
      GraphBuilder::graph()
    {
      return m_frameGraph;
    }

    /**********************************************************************************************//**
     * \fn  template <typename T> std::optional<T&> getResource(FrameGraphResourceId_t const&id)
     *
     * \brief Gets a resource
     *
     * \exception std::runtime_error  Raised when a runtime error condition occurs.
     *
     * \tparam  T Generic type parameter.
     * \param id  The identifier.
     *
     * \return  The resource.
     **************************************************************************************************/
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

    /**********************************************************************************************//**
     * \fn  FrameGraphResourceId_t GraphBuilder::findSubjacentResource( FrameGraphResourceMap const&resources, FrameGraphResource const&start)
     *
     * \brief Searches for the first subjacent resource
     *
     * \exception std::runtime_error  Raised when a runtime error condition occurs.
     *
     * \param resources The resources.
     * \param start     The start.
     *
     * \return  The found subjacent resource.
     **************************************************************************************************/
    FrameGraphResourceId_t
      GraphBuilder::findSubjacentResource(
        FrameGraphResourceMap const&resources,
        FrameGraphResource    const&start)
    {
      if(start.parentResource == 0) {
        return start.resourceId;
      }
      else {
        if(resources.find(start.parentResource) != resources.end())
          return findSubjacentResource(resources, resources.at(start.parentResource));
        else
          throw std::runtime_error("Resource not found...");
      }
    }

    /**********************************************************************************************//**
     * \fn  bool GraphBuilder::collectPass(PassBuilder&passBuilder)
     *
     * \brief Collect pass
     *
     * \param [in,out]  passBuilder The pass builder.
     *
     * \return  True if it succeeds, false if it fails.
     **************************************************************************************************/
    bool
      GraphBuilder::collectPass(PassBuilder&passBuilder)
    {
      std::function<bool(std::vector<PassUID_t> const&, PassUID_t const&)> alreadyRegisteredFn;
      alreadyRegisteredFn =
        [] (std::vector<PassUID_t> const&adjacency, PassUID_t const&possiblyAdjacent)
      {
        return std::find(adjacency.begin(), adjacency.end(), possiblyAdjacent) != adjacency.end();
      };

      FrameGraphResourceMap &resources = passBuilder.m_resources;
      m_resources.insert(resources.begin(), resources.end());

      // Derive:
      // - Resource creation requests.
      // - Edges: pass->pass and resource[view]->resource[view] for graph generation!
      // - ???
      for(FrameGraphResourceMap::value_type&resource : resources)
      {
        FrameGraphResource&r = m_resources[resource.first];

        // For each underlying OR imported resource (textures/buffers or whatever importable)
        if(r.parentResource == 0) {
          switch(r.type) {
          case FrameGraphResourceType::Imported:
            break;
          case FrameGraphResourceType::Texture:
            break;
          case FrameGraphResourceType::Buffer:
            break;
          }
        }
        // For each derived resource (views)
        else {
          FrameGraphResource const&parentResource = m_resources.at(r.parentResource);
          // Avoid internal references for passes!
          if(parentResource.assignedPassUID != r.assignedPassUID) {
            // If the edge is not added yet.
            if(!alreadyRegisteredFn(m_passAdjacency[parentResource.assignedPassUID], r.assignedPassUID)) {
              // Create edge: Parent-->Source
              m_passAdjacency[parentResource.assignedPassUID].push_back(r.assignedPassUID);
            }
          }

          FrameGraphResourceId_t  subjacentResourceId = findSubjacentResource(m_resources, r);
          FrameGraphResource     &subjacentResource   = m_resources[subjacentResourceId];

          FrameGraphTexture     &texture     = std::get<FrameGraphTexture>(subjacentResource.data);
          FrameGraphTextureView &textureView = std::get<FrameGraphTextureView>(r.data);

          switch(r.type) {
          case FrameGraphResourceType::TextureView:

            // Auto adjust format if requested
            if(textureView.format == FrameGraphFormat::Automatic)
              textureView.format = texture.format;

            // Flag required usage flags, so that the subjacent texture is properly created.
            if(textureView.mode.check(FrameGraphViewAccessMode::Read))
              texture.requestedUsage.set(FrameGraphResourceUsage::ImageResource);
            else
              texture.requestedUsage.set(FrameGraphResourceUsage::RenderTarget);

            break;
          case FrameGraphResourceType::BufferView:
            FrameGraphBuffer &buffer = std::get<FrameGraphBuffer>(subjacentResource.data);
            break;
          }
        }
      }

      Log::Verbose(logTag(), String::format("Current Adjacency State collecting pass '%0':", passBuilder.assignedPassUID()));
      for(AdjacencyListMap<PassUID_t>::value_type const&pa : m_passAdjacency) {
        Log::Verbose(logTag(), String::format("  Pass-UID: %0", pa.first));
        for(PassUID_t const&puid : pa.second) {
          Log::Verbose(logTag(), String::format("    Adjacent Pass-UID: %0", puid));
        }
      }

      return true;
    }

    /**********************************************************************************************//**
     * \fn  bool GraphBuilder::topologicalSort(std::stack<PassUID_t>&outPassOrder)
     *
     * \brief Topological sort
     *
     * \param [in,out]  outPassOrder  The out pass order.
     *
     * \return  True if it succeeds, false if it fails.
     **************************************************************************************************/
    bool
      GraphBuilder::topologicalSort(std::stack<PassUID_t>&outPassOrder)
    {
      std::function<
        void(
          AdjacencyListMap<PassUID_t> const&,
          PassUID_t const                  &,
          std::map<PassUID_t, bool>        &,
          std::stack<PassUID_t>            &)> DSFi;

      DSFi = [&](
        AdjacencyListMap<PassUID_t> const&edges,
        PassUID_t const                  &v,
        std::map<PassUID_t, bool>        &visitedEdges,
        std::stack<PassUID_t>            &passOrder) -> void
      {
        if(visitedEdges[v])
          return;

        visitedEdges[v] = true;

        // For each outgoing edge...
        if(!(edges.find(v) == edges.end())) {
          for(PassUID_t const&adjacent : edges.at(v)) {
            DSFi(edges, adjacent, visitedEdges, passOrder);
          }
        }

        passOrder.push(v);
      };

      try {
        std::map<PassUID_t, bool> visitedEdges{};
        for(AdjacencyListMap<PassUID_t>::value_type &passAdjacency : m_passAdjacency) {
          visitedEdges[passAdjacency.first] = false;
        }

        for(AdjacencyListMap<PassUID_t>::value_type &passAdjacency : m_passAdjacency) {
          DSFi(m_passAdjacency, passAdjacency.first, visitedEdges, outPassOrder);
        }
      }
      catch(std::runtime_error const&rte) {
        Log::Error(logTag(), String::format("Failed to perform topological sort: %0 ", rte.what()));
        return false;
      }
      catch(...) {
        Log::Error(logTag(), "Failed to perform topological sort. Unknown error.");
        return false;
      }

      return true;
    }

    /**********************************************************************************************//**
     * \fn  bool GraphBuilder::validate(std::stack<PassUID_t> const&passOrder)
     *
     * \brief Validates the given pass order
     *
     * \param passOrder The pass order.
     *
     * \return  True if it succeeds, false if it fails.
     **************************************************************************************************/
    bool
      GraphBuilder::validate(std::stack<PassUID_t> const&passOrder)
    {
      bool allBindingsValid = true;

      for(FrameGraphResourceMap::value_type const&resource : m_resources) {

        FrameGraphResource const&r = resource.second;

        //
        // Texture validation
        // 
        if(r.type == FrameGraphResourceType::TextureView) {

          // Adjust resource access flags in the subjacent resource to have the texture creation configure 
          // everything appropriately.
          FrameGraphResourceId_t  subjacentResourceId = findSubjacentResource(m_resources, r);
          FrameGraphResource     &subjacentResource   = m_resources[subjacentResourceId];

          if(subjacentResource.type != FrameGraphResourceType::Texture) {
            Log::Error(logTag(), "Invalid internal state. Subjacent resource of TextureView is no texture!");
            allBindingsValid = false;
          }

          FrameGraphTexture           &texture     = std::get<FrameGraphTexture>(subjacentResource.data);
          FrameGraphTextureView const &textureView = std::get<FrameGraphTextureView>(r.data);

          bool viewBindingValid = validateTextureView(texture, textureView);
          allBindingsValid &= viewBindingValid;
        }
        // Buffer validation
        else if(r.type == FrameGraphResourceType::BufferView) {

        }
        // Unsupported
        else {

        }

      } // foreach Resource

      return (allBindingsValid);
    }

    /**********************************************************************************************//**
     * \fn  bool GraphBuilder::validateTextureView(FrameGraphTexture const&texture, FrameGraphTextureView const&textureView)
     *
     * \brief Validates the texture view
     *
     * \param texture     The texture.
     * \param textureView The texture view.
     *
     * \return  True if it succeeds, false if it fails.
     **************************************************************************************************/
    bool
      GraphBuilder::validateTextureView(FrameGraphTexture const&texture, FrameGraphTextureView const&textureView)
    {
      bool usageValid             = validateTextureUsage(texture);
      bool formatValid            = validateTextureFormat(texture, textureView);
      bool subresourceAccessValid = validateTextureSubresourceAccess(texture, textureView);

      return (usageValid && formatValid && subresourceAccessValid);
    }

    /**********************************************************************************************//**
     * \fn  bool GraphBuilder::validateTextureUsage(FrameGraphTexture const&texture)
     *
     * \brief Validates the texture usage described by texture
     *
     * \param texture The texture.
     *
     * \return  True if it succeeds, false if it fails.
     **************************************************************************************************/
    bool
      GraphBuilder::validateTextureUsage(FrameGraphTexture const&texture)
    {
      // Cross both bitsets... permittedUsage should fully contain requestedUsage
      return texture.permittedUsage.check(texture.requestedUsage);
    }

    /**********************************************************************************************//**
     * \fn  bool GraphBuilder::validateTextureFormat(FrameGraphTexture const&texture, FrameGraphTextureView const&textureView)
     *
     * \brief Validates the texture format
     *
     * \param texture     The texture.
     * \param textureView The texture view.
     *
     * \return  True if it succeeds, false if it fails.
     **************************************************************************************************/
    bool
      GraphBuilder::validateTextureFormat(FrameGraphTexture const&texture, FrameGraphTextureView const&textureView)
    {
      using FrameGraphFormat_t = std::underlying_type_t<FrameGraphFormat>;

      std::function<uint64_t(FrameGraphFormat_t)> nearestPowerOf2Ceil =
        [] (FrameGraphFormat_t value) -> uint64_t
      {
        uint64_t power = 2;
        while(value >>= 1)
          power <<= 1;
        return power;
      };

      FrameGraphFormat const&source = texture.format;
      FrameGraphFormat const&target = textureView.format;

      bool formatsCompatible = false;

      switch(target) {
      case FrameGraphFormat::Undefined:
      case FrameGraphFormat::Structured:
        // Invalid!
        formatsCompatible = false;
        break;
      case FrameGraphFormat::Automatic:
        // Should never be accessed as automatic is resolved beforehand though...
        break;
      default:
        FrameGraphFormat_t sourceUID = static_cast<FrameGraphFormat_t>(source);
        FrameGraphFormat_t targetUID = static_cast<FrameGraphFormat_t>(target);
        // For now -> Simple test: Bitdepth...
        formatsCompatible = (nearestPowerOf2Ceil(sourceUID) == nearestPowerOf2Ceil(targetUID));
        break;
      }

      return formatsCompatible;
    }

    /**********************************************************************************************//**
     * \fn  bool GraphBuilder::validateTextureSubresourceAccess(FrameGraphTexture const&texture, FrameGraphTextureView const&textureView)
     *
     * \brief Validates the texture subresource access
     *
     * \param texture     The texture.
     * \param textureView The texture view.
     *
     * \return  True if it succeeds, false if it fails.
     **************************************************************************************************/
    bool
      GraphBuilder::validateTextureSubresourceAccess(FrameGraphTexture const&texture, FrameGraphTextureView const&textureView)
    {
      Range const&arraySliceRange = textureView.arraySliceRange;
      Range const&mipSliceRange   = textureView.mipSliceRange;

      bool arraySliceRangeValid = true;
      arraySliceRangeValid &= (arraySliceRange.offset < texture.arraySize);
      arraySliceRangeValid &= ((arraySliceRange.offset + arraySliceRange.length) <= texture.arraySize);

      bool mipSliceRangeValid = true;
      mipSliceRangeValid &= (mipSliceRange.offset < texture.mipLevels);
      mipSliceRangeValid &= ((mipSliceRange.offset + mipSliceRange.length) <= texture.mipLevels);

      return (arraySliceRangeValid && mipSliceRangeValid);
    }

  }
}