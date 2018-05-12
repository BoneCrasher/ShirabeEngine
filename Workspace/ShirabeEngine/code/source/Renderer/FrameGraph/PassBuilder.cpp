#include "Renderer/FrameGraph/PassBuilder.h"
#include "Core/String.h"

namespace Engine {
  namespace FrameGraph {
    /**********************************************************************************************//**
     * \fn  template <typename TPassImplementation> PassBuilder<TPassImplementation>::PassBuilder( Ptr<Pass<TPassImplementation>> &pass)
     *
     * \brief Constructor
     *
     * \tparam  TPassImplementation Type of the pass implementation.
     * \param [in,out]  pass  The pass.
     **************************************************************************************************/
    PassBuilder::PassBuilder(
      PassUID_t                                  const&passId,
      Ptr<IUIDGenerator<FrameGraphResourceId_t>>       resourceIdGenerator)
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
    FrameGraphResource
      PassBuilder::createTexture(
        std::string       const&name,
        FrameGraphTexture const&desc)
    {
      // if(!desc.validate())
      //   throw std::exception("Invalid texture descriptor.");

      // Basic abstract descriptor of resources being used.
      FrameGraphResource resource={};
      resource.assignedPassUID = m_passUID;
      resource.resourceId      = m_resourceIdGenerator->generate();
      resource.parentResource  = 0;
      resource.readableName    = name;
      resource.type            = FrameGraphResourceType::Texture;
      resource.data            = desc;

      m_resources[resource.resourceId] = resource;

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
    FrameGraphResource
      PassBuilder::writeTexture(
        FrameGraphResource          const&sourceResource,
        FrameGraphWriteTextureFlags const&flags,
        Range                       const&arraySliceRange,
        Range                       const&mipSliceRange)
    {
      // if(!isResourceRegistered(m_resourcesPrivateData, sourceResource))
      //   throw std::runtime_error("Resource to be bound as rendertarget is not registered.");

      try {
        // if(isTextureBeingReadInSubresourceRange(subjacentResourcePrivateData.resourceViews, arraySliceRange, mipSliceRange))
        //   throw std::exception(
        //     String::format(
        //       "Resource is already being read or written at the specified ranges (Array: %0[%1]; Mip: %2[%3])",
        //       arraySliceRange.offset, arraySliceRange.length, mipSliceRange.offset, mipSliceRange.length).c_str());
        // 
        // if(isTextureBeingWrittenInSubresourceRange(subjacentResourcePrivateData.resourceViews, arraySliceRange, mipSliceRange))
        //   throw std::exception(
        //     String::format(
        //       "Resource is already being read or written at the specified ranges (Array: %0[%1]; Mip: %2[%3])",
        //       arraySliceRange.offset, arraySliceRange.length, mipSliceRange.offset, mipSliceRange.length).c_str());

        FrameGraphTextureView view={ };
        view.arraySliceRange = arraySliceRange;
        view.mipSliceRange   = mipSliceRange;
        view.format          = flags.requiredFormat;
        view.mode.set(FrameGraphViewAccessMode::Write);

        FrameGraphResource resource ={};
        resource.assignedPassUID   = m_passUID;
        resource.resourceId        = m_resourceIdGenerator->generate();
        resource.parentResource    = sourceResource.resourceId;
        resource.subjacentResource =
          (sourceResource.type == FrameGraphResourceType::Texture
            ? sourceResource.resourceId
            : sourceResource.subjacentResource);
        resource.readableName      = String::format("TextureView ID %0 - Write #%1", resource.resourceId, sourceResource.resourceId);
        resource.type              = FrameGraphResourceType::TextureView;
        resource.data              = view;
        
        m_resources[resource.resourceId] = resource;

        return resource;
      }
      catch(std::bad_variant_access const&bva) {
        Log::Error(logTag(), bva.what());
        throw;
      }
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
    FrameGraphResource
      PassBuilder::readTexture(
        FrameGraphResource         const&sourceResource,
        FrameGraphReadTextureFlags const&flags,
        Range                      const&arraySliceRange,
        Range                      const&mipSliceRange)
    {
      // if(!isResourceRegistered(m_resourcesPrivateData, sourceResource))
      //   throw std::runtime_error("Resource to be bound as rendertarget is not registered.");

      try {
        // Reading overlapping subresources is no problem...
        /*if(isTextureBeingReadInSubresourceRange(subjacentResourcePrivateData.resourceViews, arraySliceRange, mipSliceRange))
          throw std::exception(
            String::format(
              "Resource is already being read or written at the specified ranges (Array: %0[%1]; Mip: %2[%3])",
              arraySliceRange.offset, arraySliceRange.length, mipSliceRange.offset, mipSliceRange.length).c_str());*/

              // if(isTextureBeingWrittenInSubresourceRange(subjacentResourcePrivateData.resourceViews, arraySliceRange, mipSliceRange))
              //   throw std::exception(
              //     String::format(
              //       "Resource is already being read or written at the specified ranges (Array: %0[%1]; Mip: %2[%3])",
              //       arraySliceRange.offset, arraySliceRange.length, mipSliceRange.offset, mipSliceRange.length).c_str());
        
        FrameGraphTextureView view={ };
        view.arraySliceRange = arraySliceRange;
        view.mipSliceRange   = mipSliceRange;
        view.format          = flags.requiredFormat;
        view.mode.set(FrameGraphViewAccessMode::Read);

        FrameGraphResource resource ={ };
        resource.assignedPassUID   = m_passUID;
        resource.resourceId        = m_resourceIdGenerator->generate();
        resource.parentResource    = sourceResource.resourceId;
        resource.subjacentResource = 
          (sourceResource.type == FrameGraphResourceType::Texture
            ? sourceResource.resourceId 
            : sourceResource.subjacentResource);
        resource.readableName      = String::format("TextureView ID %0 - Read #%1", resource.resourceId, sourceResource.resourceId);
        resource.type              = FrameGraphResourceType::TextureView;
        resource.data              = view;
        
        m_resources[resource.resourceId] = resource;

        return resource;
      }
      catch(std::bad_variant_access const&bve) {
        Log::Error(logTag(), bve.what());
        throw;
      }
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
    FrameGraphResource
      PassBuilder::importRenderables()
    {
      return FrameGraphResource();
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
    bool
      PassBuilder::isTextureBeingReadInSubresourceRange(
        std::vector<FrameGraphResourceId_t> const&resourceViews,
        Range                               const&arraySliceRange,
        Range                               const&mipSliceRange)
    {
      try {
        // This test method can check for overlapping regions for array slices and mip slices
        //  given a specific set of options to test against.
        // If "Write" is checked, this is usually done due to read operations to take place.
        // If a "Read" and "Write" of two subresources does not overlap, both operations 
        //  as such are valid.
        // The whole operation setup is based on first come first serve.
        for(FrameGraphResourceId_t const&id : resourceViews) {
          FrameGraphResource const&resource = m_resources[id];
          if(!(resource.type == FrameGraphResourceType::TextureView))
            continue;

          FrameGraphTextureView const&v = std::get<FrameGraphTextureView>(resource.data);
          if(!v.mode.check(FrameGraphViewAccessMode::Read))
            continue;

          if(v.arraySliceRange.overlapsWith(arraySliceRange)
            || v.mipSliceRange.overlapsWith(mipSliceRange))
            return true;
        }

        return false;
      }
      catch(std::bad_variant_access const&bve) {
        Log::Error(logTag(), bve.what());
        throw;
      }

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
    bool
      PassBuilder::isTextureBeingWrittenInSubresourceRange(
        std::vector<FrameGraphResourceId_t> const&resourceViews,
        Range                               const&arraySliceRange,
        Range                               const&mipSliceRange)
    {
      try {
        // This test method can check for overlapping regions for array slices and mip slices
        //  given a specific set of options to test against.
        // If "Write" is checked, this is usually done due to read operations to take place.
        // If a "Read" and "Write" of two subresources does not overlap, both operations 
        //  as such are valid.
        // The whole operation setup is based on first come first serve.
        for(FrameGraphResourceId_t const&id : resourceViews) {
          FrameGraphResource const&resource = m_resources[id];
          if(!(resource.type == FrameGraphResourceType::TextureView))
            continue;

          FrameGraphTextureView const&v = std::get<FrameGraphTextureView>(resource.data);
          if(!v.mode.check(FrameGraphViewAccessMode::Write))
            continue;

          if(v.arraySliceRange.overlapsWith(arraySliceRange)
            || v.mipSliceRange.overlapsWith(mipSliceRange))
            return true;
        }

        return false;
      }
      catch(std::bad_variant_access const&bve) {
        Log::Error(logTag(), bve.what());
        throw;
      }
    }

  }
}