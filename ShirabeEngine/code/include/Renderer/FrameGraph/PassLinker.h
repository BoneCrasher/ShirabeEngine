#ifndef __SHIRABE_RENDERER_passLinker_H__
#define __SHIRABE_RENDERER_passLinker_H__

#include <string>
#include <functional>

#include "Core/EngineTypeHelper.h"
#include "Resources/Core/ResourceDTO.h"

#include "FrameGraphData.h"

namespace Engine {
  namespace FrameGraph {

    using namespace Engine::Resources;
    
    /**********************************************************************************************//**
     * \class PassBuilder
     *
     * \brief A pass builder.
     *
     * \tparam  TPassImplementation Type of the pass implementation.
     **************************************************************************************************/
    template <typename TPassImplementation>
    class PassLinker {
    public:
      PassLinker(FrameGraphResourceId_t const&);

      FrameGraphResourceId_t const&assignedPassUID() const { return m_passUID; }

      template <typename TResource>
      FrameGraphResource
        createResource(
          typename TResource::Descriptor const&desc);

      FrameGraphResource
        bindRenderTarget(
          FrameGraphResource const&subjacentTargetResourceId,
          uint64_t                 arrayIndex,
          uint64_t                 elementCount = 1,
          uint64_t                 mipIndex     = 0);

	  FrameGraphResource
		  bindInput(
			  FrameGraphResource const&resourceId);

      FrameGraphResource
        importRenderables();

    private:
      FrameGraphResourceId_t m_passUID;
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
    PassLinker<TPassImplementation>::PassLinker(
      FrameGraphResourceId_t const&passId)
      : m_passUID(passId)
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
    FrameGraphResource
      PassLinker<TPassImplementation>::createResource(
        typename TResource::Descriptor const&desc)
    {
      // static_assert(false, LOG_FUNCTION(GraphBuilder::createResource(...) : Not implemented(GraphBuilder.cpp Line __LINE__)));
      return FrameGraphResource();
    }

    template <typename TPassImplementation>
    FrameGraphResource
      PassLinker<TPassImplementation>::bindRenderTarget(
        FrameGraphResource const&subjacentTargetResourceId,
        uint64_t                 arrayIndex,
        uint64_t                 elementCount,
        uint64_t                 mipIndex)
    {
      return FrameGraphResource();
    }

	template <typename TPassImplementation>
	FrameGraphResource
		PassLinker<TPassImplementation>::bindInput(
			FrameGraphResource const&subjacentTargetResourceId)
	{
		return FrameGraphResource();
	}

    template <typename TPassImplementation>
    FrameGraphResource
      PassLinker<TPassImplementation>::importRenderables()
    {
      return FrameGraphResource();
    }
  }
}

#endif