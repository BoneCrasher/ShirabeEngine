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
      using PassInput_t  = typename TPassImplementation::InputData;
      using PassOutput_t = typename TPassImplementation::OutputData;

      PassLinker(FrameGraphResourceId_t const&);

      FrameGraphResourceId_t const&assignedPassUID() const { return m_passUID; }

      PassInput_t  &input()  { return m_inputData;  }
      PassOutput_t &output() { return m_outputData; }

      template <typename TResource>
      FrameGraphResourceId_t
        createResource(
          typename TResource::Descriptor const&desc);

      FrameGraphResourceId_t
        bindRenderTarget(
          FrameGraphResourceId_t const&subjacentTargetResourceId,
          uint64_t                     arrayIndex,
          uint64_t                     elementCount = 1,
          uint64_t                     mipIndex     = 0);

      FrameGraphResourceId_t
        importRenderables();

    private:
      FrameGraphResourceId_t m_passUID;
      PassInput_t            m_inputData;
      PassOutput_t           m_outputData;
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
    FrameGraphResourceId_t
      PassLinker<TPassImplementation>::createResource(
        typename TResource::Descriptor const&desc)
    {
      // static_assert(false, LOG_FUNCTION(GraphBuilder::createResource(...) : Not implemented(GraphBuilder.cpp Line __LINE__)));
      return FrameGraphResourceId_t();
    }

    template <typename TPassImplementation>
    FrameGraphResourceId_t
      PassLinker<TPassImplementation>::bindRenderTarget(
        FrameGraphResourceId_t const&subjacentTargetResourceId,
        uint64_t                     arrayIndex,
        uint64_t                     elementCount,
        uint64_t                     mipIndex)
    {
      return FrameGraphResourceId_t();
    }

    template <typename TPassImplementation>
    FrameGraphResourceId_t
      PassLinker<TPassImplementation>::importRenderables()
    {
      return FrameGraphResourceId_t();
    }
  }
}

#endif