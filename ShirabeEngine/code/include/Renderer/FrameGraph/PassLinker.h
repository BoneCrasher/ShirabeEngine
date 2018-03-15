#ifndef __SHIRABE_RENDERER_passLinker_H__
#define __SHIRABE_RENDERER_passLinker_H__

#include <string>
#include <functional>

#include "Core/EngineTypeHelper.h"
#include "Core/Random.h"
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
      PassLinker(FrameGraphResourceId_t const&, Random::RandomState&);

      FrameGraphResourceId_t const&assignedPassUID() const { return m_passUID; }

      FrameGraphResource
        createTexture(
          FrameGraphTexture const&desc);

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
      Random::RandomState    &m_resourceIdGenerator;
      FrameGraphResourceId_t  m_passUID;
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
      FrameGraphResourceId_t const&passId,
      Random::RandomState         &resourceIdGenerator)
      : m_passUID(passId)
      , m_resourceIdGenerator(resourceIdGenerator)
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
    FrameGraphResource
      PassLinker<TPassImplementation>::createTexture(
        FrameGraphTexture const&desc)
    {
      if(!desc.validate())
        throw std::exception("Invalid texture descriptor.");

      FrameGraphResourceId_t          resourceId;
      FrameGraphResourceType          type;
      FrameGraphResourceUsage         usage;
      FrameGraphResourceAccessibility cpuAccessibility;
      FrameGraphResourceAccessibility gpuAccessibility;

      resourceId = m_resourceIdGenerator.next();

      if(desc.width)
        if(desc.height > 1)
          if(desc.depth > 1)
            if(desc.arraySize > 1)
              type = FrameGraphResourceType::Texture3D;
            else
              type = FrameGraphResourceType::Texture3DArray;
          else
            if(desc.arraySize > 1)
              type = FrameGraphResourceType::Texture2D;
            else
              type = FrameGraphResourceType::Texture2DArray;
        else
          if(desc.arraySize > 1)
            type = FrameGraphResourceType::Texture1D;
          else
            type = FrameGraphResourceType::Texture1DArray;
      else
        throw std::exception("Invalid texture dimensions: Width is 0.");

      usage            = FrameGraphResourceUsage::Undefined;
      cpuAccessibility = FrameGraphResourceAccessibility::None;
      gpuAccessibility = FrameGraphResourceAccessibility::None;

      FrameGraphResource resource(resourceId, type, usage, cpuAccessibility, gpuAccessibility);

      // Store appropriately...

      return resource;
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