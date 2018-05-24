#include "Renderer/FrameGraph/GraphBuilder.h"

namespace Engine {
  namespace FrameGraph {

    template <typename TUID>
    static std::function<bool(std::vector<TUID> const&, TUID const&)> alreadyRegisteredFn =
      [] (std::vector<TUID> const&adjacency, TUID const&possiblyAdjacent)
    {
      return std::find(adjacency.begin(), adjacency.end(), possiblyAdjacent) != adjacency.end();
    };

    class SequenceUIDGenerator
      : public IUIDGenerator<FrameGraphResourceId_t>
    {
    public:
      SequenceUIDGenerator(FrameGraphResourceId_t const&initialID = 1)
        : m_id(initialID)
      {};

      inline FrameGraphResourceId_t generate() {
        return (m_id++);
      }

    private:
      FrameGraphResourceId_t m_id;
    };

    GraphBuilder::GraphBuilder()
      : m_passUIDGenerator(std::make_shared<SequenceUIDGenerator>(0))
      , m_resourceUIDGenerator(std::make_shared<SequenceUIDGenerator>(1))
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
      env()   = environment;
      graph() = MakeUniquePointerType<Graph>();

      // Spawn pseudo pass to simplify algorithms and have "empty" execution blocks.
      spawnPass<CallbackPass<bool>>(
        "Pseudo-Pass",
        [] (PassBuilder const&, bool&)                                               -> bool { return true; },
        [] (bool const&, FrameGraphResources const&, Ptr<IFrameGraphRenderContext>&) -> bool { return true; });

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
      graph() = nullptr;
      env()   = nullptr;

      return true;
    }

    Ptr<ApplicationEnvironment>&
      GraphBuilder::env()
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
        FrameGraphTexture &resource = m_resourceData.spawnResource<FrameGraphTexture>();
        resource.assignedPassUID    = 0; // Pre-Pass
        resource.parentResource     = 0; // No internal tree, has to be resolved differently.
        resource.type               = FrameGraphResourceType::Texture;
        resource.readableName       = readableName;
        resource.isExternalResource = true;
        resource.assignTextureParameters(texture);

        m_resources.push_back(resource.resourceId);

        return resource;
      }
      catch(std::exception) {
        Log::Error(logTag(), String::format("Failed to register texture %0", readableName));
        throw;
      }
    }

    FrameGraphResource
      GraphBuilder::registerRenderables(
        std::string              const&readableIdentifier,
        Renderer::RenderableList const&renderables)
    {
      try {
        FrameGraphRenderableList &resource = m_resourceData.spawnResource<FrameGraphRenderableList>();
        resource.assignedPassUID    = 0; // Pre-Pass
        resource.parentResource     = 0; // No internal tree, has to be resolved differently.
        resource.type               = FrameGraphResourceType::RenderableList;
        resource.readableName       = readableIdentifier;
        resource.isExternalResource = true;
        resource.renderableList     = renderables;

        m_resources.push_back(resource.resourceId);

        return resource;
      }
      catch(std::exception) {
        Log::Error(logTag(), String::format("Failed to register renderable list %0", readableIdentifier));
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
      bool topologicalPassSortSuccessful = topologicalSort<PassUID_t>(graph()->m_passExecutionOrder);
      if(!topologicalPassSortSuccessful) {
        Log::Error(logTag(), "Failed to perform topologicalSort(...) for passes on graph compilation.");
        return nullptr;
      }

      bool topologicalResourceSortSuccessful = topologicalSort<FrameGraphResourceId_t>(graph()->m_resourceOrder);
      if(!topologicalResourceSortSuccessful) {
        Log::Error(logTag(), "Failed to perform topologicalSort(...) for resources on graph compilation.");
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
        graph()->addPass(m_passes.at(uid));
        order.pop();
      }

      graph()->m_passAdjacency           = std::move(this->m_passAdjacency);
      graph()->m_resourceAdjacency       = std::move(this->m_resourceAdjacency);
      graph()->m_passToResourceAdjacency = std::move(this->m_passToResourceAdjacency);
      graph()->m_resources               = std::move(this->m_resources);
      graph()->m_resourceData.mergeIn(this->m_resourceData);

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
      m_resources.resize(m_resources.size() + passBuilder.m_resources.size());
      for(FrameGraphResourceId_t const&id : passBuilder.m_resources)
        m_resources.push_back(id);

      // Derive:
      // - Resource creation requests.
      // - Edges: pass->pass and resource[view]->resource[view] for graph generation!
      // - ???
      FrameGraphResourceIdList &resources = passBuilder.m_resources;
      for(FrameGraphResourceId_t const&resource : resources)
      {
        FrameGraphResource&r = *m_resourceData.getMutable<FrameGraphResource>(resource);

        // For each underlying OR imported resource (textures/buffers or whatever importable)
        if(r.parentResource == 0) {
          switch(r.type) {
          case FrameGraphResourceType::Imported:
            break;
          case FrameGraphResourceType::RenderableList:
          case FrameGraphResourceType::Buffer:
          case FrameGraphResourceType::Texture:
            // And map the resources to it's pass appropriately
            if(!alreadyRegisteredFn<FrameGraphResourceId_t>(m_passToResourceAdjacency[r.assignedPassUID], r.resourceId)) {
              m_passToResourceAdjacency[r.assignedPassUID].push_back(r.resourceId);
            }
            break;
          }
        }
        // For each derived resource (views)
        else {
          // Avoid internal references for passes!
          // If the edge from pass k to pass k+1 was not added yet.
          // Create edge: Parent-->Source
          FrameGraphResource const&parentResource = *m_resourceData.get<FrameGraphResource>(r.parentResource);
          if(parentResource.assignedPassUID != r.assignedPassUID) {
            if(!alreadyRegisteredFn<PassUID_t>(m_passAdjacency[parentResource.assignedPassUID], r.assignedPassUID)) {
              m_passAdjacency[parentResource.assignedPassUID].push_back(r.assignedPassUID);
            }
          }

          // Do the same for the resources!
          if(!alreadyRegisteredFn<FrameGraphResourceId_t>(m_resourceAdjacency[parentResource.resourceId], r.resourceId)) {
            m_resourceAdjacency[parentResource.resourceId].push_back(r.resourceId);
          }

          // And map the resources to it's pass appropriately
          if(!alreadyRegisteredFn<FrameGraphResourceId_t>(m_passToResourceAdjacency[r.assignedPassUID], r.resourceId)) {
            m_passToResourceAdjacency[r.assignedPassUID].push_back(r.resourceId);
          }

          if(r.type == FrameGraphResourceType::TextureView) {
            // Further adjustments
            FrameGraphTexture     &texture     = *m_resourceData.getMutable<FrameGraphTexture>(r.subjacentResource);
            FrameGraphTextureView &textureView = *m_resourceData.getMutable<FrameGraphTextureView>(r.resourceId);
            
            // Auto adjust format if requested
            if(textureView.format == FrameGraphFormat::Automatic)
              textureView.format = texture.format;

            // Flag required usage flags, so that the subjacent texture is properly created.
            if(textureView.mode.check(FrameGraphViewAccessMode::Read))
              texture.requestedUsage.set(FrameGraphResourceUsage::ImageResource);
            else
              texture.requestedUsage.set(FrameGraphResourceUsage::RenderTarget);
          }
          else if(r.type == FrameGraphResourceType::BufferView) {
            // TODO
          }
          else if(r.type == FrameGraphResourceType::RenderableListView) {
            // TODO
          }
        }
      }

      #ifdef SHIRABE_DEBUG
      Log::Verbose(logTag(), String::format("Current Adjacency State collecting pass '%0':", passBuilder.assignedPassUID()));
      for(AdjacencyListMap<PassUID_t>::value_type const&pa : m_passAdjacency) {
        Log::Verbose(logTag(), String::format("  Pass-UID: %0", pa.first));
        for(PassUID_t const&puid : pa.second) {
          Log::Verbose(logTag(), String::format("    Adjacent Pass-UID: %0", puid));
        }
      }
      #endif

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

      for(RefIndex::value_type const&textureViewRef : m_resourceData.textureViews()) {
        FrameGraphTextureView const&textureView = *m_resourceData.get<FrameGraphTextureView>(textureViewRef); 

        // Adjust resource access flags in the subjacent resource to have the texture creation configure 
        // everything appropriately.
        FrameGraphResourceId_t subjacentResourceId = textureView.subjacentResource;

        FrameGraphTexture const&texture = *m_resourceData.get<FrameGraphTexture>(subjacentResourceId);

        bool viewBindingValid = validateTextureView(texture, textureView);
        allBindingsValid &= viewBindingValid;

      } // foreach TextureView

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