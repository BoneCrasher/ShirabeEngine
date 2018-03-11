#ifndef __SHIRABE_RENDERER_GRAPHBUILDER_H__
#define __SHIRABE_RENDERER_GRAPHBUILDER_H__

#include <string>
#include <functional>

#include "Core/EngineTypeHelper.h"
#include "Resources/Core/ResourceDTO.h"

#include "Pass.h"
#include "FrameGraph.h"

namespace Engine {
  namespace FrameGraph {

    using namespace Engine::Resources;

    /**********************************************************************************************//**
     * \class GraphBuilder
     *
     * \brief A graph builder.
     **************************************************************************************************/
    class GraphBuilder {
    public:
      bool initialize();
      bool deinitialize();

      template <typename TPassImplementation, typename... TPassCreationArgs>
      Ptr<Pass<TPassImplementation>>
        spawnPass(
          std::string      const&id,
          TPassCreationArgs&&... args);

      bool
        importPersistentResource(
          std::string        const&id,
          PublicResourceId_t const&resourceId);

      UniquePtr<FrameGraph>
        compile();

    private:
      inline UniquePtr<FrameGraph>&                graph()             { return m_frameGraph; }
      inline Map<std::string, PublicResourceId_t>& importedResources() { return m_importedResources; }

      UniquePtr<FrameGraph>                m_frameGraph;
      Map<std::string, PublicResourceId_t> m_importedResources;
    };

    /**********************************************************************************************//**
     * \fn  template <typename TPassImplementation, typename... TPassCreationArgs> Ptr<Pass<TPassImplementation>> GraphBuilder::spawnPass( std::string const&id, TPassCreationArgs&&... args)
     *
     * \brief Spawn pass
     *
     * \tparam  TPassImplementation Type of the pass implementation.
     * \tparam  TPassCreationArgs   Type of the pass creation arguments.
     * \param id    The identifier.
     * \param args  Variable arguments providing [in,out] The arguments.
     *
     * \return  A Ptr&lt;Pass&lt;TPassImplementation&gt;&gt;
     **************************************************************************************************/
    template <typename TPassImplementation, typename... TPassCreationArgs>
    Ptr<Pass<TPassImplementation>>
      GraphBuilder::spawnPass(
        std::string      const&id,
        TPassCreationArgs&&... args)
    {
      if (!graph())
        return false;

      try {
        Ptr<Pass<TPassImplementation>>
          pass = MakeSharedPointerType<Pass<TPassImplementation>>(std::forward<TPassCreationArgs>(args)...);
        if (!pass)
          return false;

        graph()->passes().push_back(std::static_pointer_cast<PassBase>(pass));

        return pass;
      }
      catch (std::exception e) {
        return nullptr;
      }
      catch (...) {
        return nullptr;
      }
    }

    /**********************************************************************************************//**
     * \class PassBuilder
     *
     * \brief A pass builder.
     *
     * \tparam  TPassImplementation Type of the pass implementation.
     **************************************************************************************************/
    template <typename TPassImplementation>
    class PassBuilder {
    public:
      PassBuilder(Ptr<Pass<TPassImplementation>>&);

      template <typename TResource>
      FrameGraphResourceId_t
        createResource(
          typename TResource::Descriptor const&desc);


    private:
      Ptr<Pass<TPassImplementation>> m_pass;
    };

    /**********************************************************************************************//**
     * \fn  template <typename TPassImplementation> PassBuilder<TPassImplementation>::PassBuilder( Ptr<Pass<TPassImplementation>> &pass)
     *
     * \brief Constructor
     *
     * \tparam  TPassImplementation Type of the pass implementation.
     * \param [in,out]  pass  The pass.
     **************************************************************************************************/
    template <typename TPassImplementation>
    PassBuilder<TPassImplementation>::PassBuilder(
      Ptr<Pass<TPassImplementation>> &pass)
      : m_pass(pass)
    {}
    
    /**********************************************************************************************//**
     * \fn  template <typename TResource> PublicResourceId_t GraphBuilder::createResource( typename TResource::Descriptor const&desc)
     *
     * \brief Creates a resource
     *
     * \tparam  TResource Type of the resource.
     * \param desc  The description.
     *
     * \return  The new resource.
     **************************************************************************************************/
    template <typename TPassImplementation>
    template <typename TResource>
    FrameGraphResourceId_t
      PassBuilder<TPassImplementation>::createResource(
        typename TResource::Descriptor const&desc)
    {
      static_assert(false, LOG_FUNCTION(GraphBuilder::createResource(...) : Not implemented(GraphBuilder.cpp Line __LINE__)));
    }
  }
}

#endif