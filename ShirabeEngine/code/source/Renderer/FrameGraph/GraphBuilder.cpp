#include "Renderer/FrameGraph/GraphBuilder.h"

namespace Engine {
  namespace FrameGraph {

    GraphBuilder::GraphBuilder()
      : m_uidGenerator()
      , m_frameGraph(nullptr)
      , m_importedResources()
    {}

    FrameGraphResourceId_t
      GraphBuilder::generatePassUID()
    {
      return m_uidGenerator.next();
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
  }
}