#ifndef __SHIRABE_RENDERER_passLinker_H__
#define __SHIRABE_RENDERER_passLinker_H__

#include <string>
#include <functional>

#include "Core/BasicTypes.h"
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
      using ResourceMap_t = std::map<FrameGraphResource, FrameGraphUsageInfo>;
    public:
      PassLinker(FrameGraphResourceId_t const&, Random::RandomState&);

      FrameGraphResourceId_t const&assignedPassUID() const { return m_passUID; }

      FrameGraphResource
        createTexture(
          FrameGraphTexture const&desc);

      // createBuffer
      // create~?

      FrameGraphResource
        bindRenderTarget(
          FrameGraphResource const&subjacentTargetResourceId,
          Range              const&arraySliceRange = Range(0, 1),
          Range              const&mipSliceRange   = Range(0, 1));

      FrameGraphResource
        bindInput(
          FrameGraphResource const&subjacentTargetResourceId,
          Range              const&arraySliceRange = Range(0, 1),
          Range              const&mipSliceRange   = Range(0, 1));

      FrameGraphResource
        importRenderables();

    private:
      bool isResourceRegistered(FrameGraphResource const&resourceId) const;
      bool checkReadWriteOverlap(
        FrameGraphUsageInfo                 const&usageInfo,
        Range                               const&arraySliceRange,
        Range                               const&mipSliceRange,
        BitField<FrameGraphUsageInfo::Type> const checkedOps = (FrameGraphUsageInfo::Type::Read | FrameGraphUsageInfo::Type::Write) );

      Random::RandomState    &m_resourceIdGenerator;
      FrameGraphResourceId_t  m_passUID;
      ResourceMap_t           m_resources;
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

      // Basic abstract descriptor of resources being used.
      FrameGraphResource resource(resourceId, type, usage, cpuAccessibility, gpuAccessibility);

      // The usage info will be used to keep track of how resources are gonna be used (Mode, Duration, Parallel Access).
      FrameGraphUsageInfo info;
      info.resourceId = resource.resourceId;
      info.usageTypes      = FrameGraphUsageInfo::Type::None;
      // Store

      m_resources[resource] = info;

      return resource;
    }

    /**********************************************************************************************//**
     * \fn  template <typename TPassImplementation> FrameGraphResource PassLinker<TPassImplementation>::bindRenderTarget( FrameGraphResource const&subjacentTargetResourceId, Range const&arraySliceRange, Range const&mipSliceRange)
     *
     * \brief Bind render target
     *
     * \exception std::exception  Thrown when an exception error condition occurs.
     *
     * \tparam  TPassImplementation Type of the pass implementation.
     * \param subjacentTargetResourceId Identifier for the subjacent target resource.
     * \param arraySliceRange           The array slice range.
     * \param mipSliceRange             The mip slice range.
     *
     * \return  A FrameGraphResource.
     **************************************************************************************************/
    template <typename TPassImplementation>
    FrameGraphResource
      PassLinker<TPassImplementation>::bindRenderTarget(
        FrameGraphResource const&subjacentTargetResourceId,
        Range              const&arraySliceRange,
        Range              const&mipSliceRange)
    {
      if(!isResourceRegistered(subjacentTargetResourceId))
        throw std::exception("Resource to be bound as rendertarget is not registered.");

      FrameGraphUsageInfo &info = m_resources[subjacentTargetResourceId];
      if(info.usageTypes.check(FrameGraphUsageInfo::Type::Read))
        throw std::exception("Resource is already being read by this pass and cannot be simultaneously written.");

      if(checkReadWriteOverlap(info, arraySliceRange, mipSliceRange))
        throw std::exception(
          String::format(
            "Resource is already being read or written at the specified ranges (Array: %0[%1]; Mip: %2[%3])",
            arraySliceRange.offset, arraySliceRange.length, mipSliceRange.offset, mipSliceRange.length).c_str());

      info.usageTypes.set(FrameGraphUsageInfo::Type::Write);
      info.resourceAccessors[{ arraySliceRange, mipSliceRange}] = FrameGraphUsageInfo::Type::Write;

      return subjacentTargetResourceId;
    }

    /**********************************************************************************************//**
     * \fn  template <typename TPassImplementation> FrameGraphResource PassLinker<TPassImplementation>::bindInput( FrameGraphResource const&subjacentTargetResourceId, Range const&arraySliceRange, Range const&mipSliceRange)
     *
     * \brief Bind input
     *
     * \exception std::exception  Thrown when an exception error condition occurs.
     *
     * \tparam  TPassImplementation Type of the pass implementation.
     * \param subjacentTargetResourceId Identifier for the subjacent target resource.
     * \param arraySliceRange           The array slice range.
     * \param mipSliceRange             The mip slice range.
     *
     * \return  A FrameGraphResource.
     **************************************************************************************************/
    template <typename TPassImplementation>
    FrameGraphResource
      PassLinker<TPassImplementation>::bindInput(
        FrameGraphResource const&subjacentTargetResourceId,
        Range              const&arraySliceRange,
        Range              const&mipSliceRange)
    {
      if(!isResourceRegistered(subjacentTargetResourceId))
        throw std::exception("Resource to be bound as shader input is not registered.");

      FrameGraphUsageInfo &info = m_resources[subjacentTargetResourceId];
      if(info.usageTypes.check(FrameGraphUsageInfo::Type::Write))
        throw std::exception("Resource is being written by this pass and cannot be simultaneously read.");

      // Only check against write... we can read as much as we want.
      // If existing reads are stored, they all passed before.
      if(checkReadWriteOverlap(info, arraySliceRange, mipSliceRange, FrameGraphUsageInfo::Type::Write))
        throw std::exception(
          String::format(
            "Resource is already being written at the specified ranges (Array: %0[%1]; Mip: %2[%3])",
            arraySliceRange.offset, arraySliceRange.length, mipSliceRange.offset, mipSliceRange.length).c_str());

      info.usageTypes.set(FrameGraphUsageInfo::Type::Read);
      info.resourceAccessors[{ arraySliceRange, mipSliceRange}] = FrameGraphUsageInfo::Type::Read;

      return subjacentTargetResourceId;
    }

    /**********************************************************************************************//**
     * \fn  template <typename TPassImplementation> FrameGraphResource PassLinker<TPassImplementation>::importRenderables()
     *
     * \brief Import renderables
     *
     * \tparam  TPassImplementation Type of the pass implementation.
     *
     * \return  A FrameGraphResource.
     **************************************************************************************************/
    template <typename TPassImplementation>
    FrameGraphResource
      PassLinker<TPassImplementation>::importRenderables()
    {
      return FrameGraphResource();
    }

    /**********************************************************************************************//**
     * \fn  template <typename TPassImplementation> bool PassLinker<TPassImplementation>::isResourceRegistered(FrameGraphResource const&resourceId) const
     *
     * \brief Queries if a resource is registered
     *
     * \tparam  TPassImplementation Type of the pass implementation.
     * \param resourceId  Identifier for the resource.
     *
     * \return  True if the resource is implementation>, false if not.
     **************************************************************************************************/
    template <typename TPassImplementation>
    bool
      PassLinker<TPassImplementation>::isResourceRegistered(
        FrameGraphResource const&subjacentTargetResourceId) const
    {
      return (m_resources.find(subjacentTargetResourceId) == m_resources.end());
    }

    /**********************************************************************************************//**
     * \fn  template <typename TPassImplementation> bool PassLinker<TPassImplementation>::checkReadWriteOverlap( FrameGraphUsageInfo const&usageInfo, Range const&arraySliceRange, Range const&mipSliceRange, BitField<FrameGraphUsageInfo::Type> const&checkedOps = (FrameGraphUsageInfo::Type::Read | FrameGraphUsageInfo::Type::Write))
     *
     * \brief Check read write overlap
     *
     * \tparam  TPassImplementation Type of the pass implementation.
     * \param usageInfo       Information describing the usage.
     * \param arraySliceRange The array slice range.
     * \param mipSliceRange   The mip slice range.
     * \param checkedOps      (Optional) The checked ops.
     *
     * \return  True if it succeeds, false if it fails.
     **************************************************************************************************/
    template <typename TPassImplementation>
    bool 
      PassLinker<TPassImplementation>::checkReadWriteOverlap(
        FrameGraphUsageInfo                 const&usageInfo,
        Range                               const&arraySliceRange,
        Range                               const&mipSliceRange,
        BitField<FrameGraphUsageInfo::Type> const checkedOps)
    {
      // This test method can check for overlapping regions for array slices and mip slices
      //  given a specific set of options to test against.
      // If "Write" is checked, this is usually done due to read operations to take place.
      // If a "Read" and "Write" of two subresources does not overlap, both operations 
      //  as such are valid.
      // The whole operation setup is based on first come first serve.
      bool overlap = false;
      if(checkedOps.check(FrameGraphUsageInfo::Type::Write)) {
        for(std::map<FrameGraphResourceIndex, FrameGraphUsageInfo::Type>::value_type const&targetBinding : usageInfo.resourceAccessors) {
          if(targetBinding.second != FrameGraphUsageInfo::Type::Write)
            continue;

          if((overlap = targetBinding.first.arraySliceRange.overlapsWith(arraySliceRange)))
            break;
          if((overlap = targetBinding.first.mipSliceRange.overlapsWith(mipSliceRange)))
            break;
        }
      }

      if(overlap)
        return true;
      
      if(checkedOps.check(FrameGraphUsageInfo::Type::Read)) {
        for(std::map<FrameGraphResourceIndex, FrameGraphUsageInfo::Type>::value_type const&inputBinding : usageInfo.resourceAccessors) {
          if(inputBinding.second != FrameGraphUsageInfo::Type::Read)
            continue;

          if((overlap = inputBinding.first.arraySliceRange.overlapsWith(arraySliceRange)))
            break;
          if((overlap = inputBinding.first.mipSliceRange.overlapsWith(mipSliceRange)))
            break;
        }
      }

      if(overlap)
        return true;

      return false;
    }
  }
}

#endif