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
      PassUID_t                const&passId,
      Ptr<Random::RandomState>       resourceIdGenerator)
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
        FrameGraphTexture const&desc)
    {
      // if(!desc.validate())
      //   throw std::exception("Invalid texture descriptor.");

      // Basic abstract descriptor of resources being used.
      FrameGraphResource resource={};
      resource.resourceId = m_resourceIdGenerator->next();

      FrameGraphResourcePrivateData privateData={};
      privateData.parentResourceId     = FrameGraphResourceId_t{ };
      privateData.type                 = FrameGraphResourceType::Texture;
      privateData.flags.requiredFormat = desc.format;
      privateData.usage                = FrameGraphResourceUsage::Undefined;

      m_textures[resource.resourceId]             = desc;
      m_resourcesPrivateData[resource.resourceId] = privateData;

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
        FrameGraphResource          const&subjacentTargetResource,
        FrameGraphWriteTextureFlags const&flags,
        Range                       const&arraySliceRange,
        Range                       const&mipSliceRange)
    {
      // if(!isResourceRegistered(subjacentTargetResource))
      //   throw std::exception("Resource to be bound as rendertarget is not registered.");

      FrameGraphTexture             const&subjacentResource            = m_textures[subjacentTargetResource.resourceId];
      FrameGraphResourcePrivateData      &subjacentResourcePrivateData = m_resourcesPrivateData[subjacentTargetResource.resourceId];

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

      FrameGraphResourceUsage usage={};
      switch(flags.writeTarget) {
      case FrameGraphWriteTarget::Color:
        usage = FrameGraphResourceUsage::RenderTarget;
        break;
      case FrameGraphWriteTarget::Depth:
        usage = FrameGraphResourceUsage::DepthTarget;
        break;
      default:
        usage = FrameGraphResourceUsage::Undefined;
        break;
      }

      FrameGraphResource resource ={};
      resource.resourceId = m_resourceIdGenerator->next();

      FrameGraphResourceFlags resourceViewFlags={};
      resourceViewFlags.requiredFormat = flags.requiredFormat;

      FrameGraphResourcePrivateData privateData={};
      privateData.parentResourceId = subjacentTargetResource.resourceId;
      privateData.type             = FrameGraphResourceType::TextureView;
      privateData.flags            = resourceViewFlags;
      privateData.usage            = usage;

      FrameGraphTextureView view={};
      view.readableName    = String::format("%1 View - Write#%2", subjacentResource.readableName, subjacentTargetResource.resourceId);
      view.arraySliceRange = arraySliceRange;
      view.mipSliceRange   = mipSliceRange;
      view.format          = subjacentResource.format;
      // if(flags.requiredFormat != FrameGraphFormat::Automatic) {
      //   if(validateFormatCompatibility(subjacentResource.format, flags.requiredFormat)) {
      //     view.format = flags.requiredFormat;
      //   }
      //   else
      //     throw std::exception(
      //       String::format("Incompatible formats for texture view detected [Subjacent: %0, View: %1].",
      //         subjacentResource.format,
      //         flags.requiredFormat).c_str());
      // }
      view.mode.set(FrameGraphViewAccessMode::Write);

      subjacentResourcePrivateData.resourceViews.push_back(resource.resourceId);

      m_textureViews[resource.resourceId]         = view;
      m_resourcesPrivateData[resource.resourceId] = privateData;

      return resource;
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
        FrameGraphResource         const&subjacentTargetResource,
        FrameGraphReadTextureFlags const&flags,
        Range                      const&arraySliceRange,
        Range                      const&mipSliceRange)
    {
      if(!isResourceRegistered(subjacentTargetResource))
        throw std::exception("Resource to be bound as rendertarget is not registered.");

      FrameGraphTexture             const&subjacentResource            = m_textures[subjacentTargetResource.resourceId];
      FrameGraphResourcePrivateData      &subjacentResourcePrivateData = m_resourcesPrivateData[subjacentTargetResource.resourceId];

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

      FrameGraphResource resource ={ };
      resource.resourceId = m_resourceIdGenerator->next();

      FrameGraphResourceFlags resourceViewFlags={ };
      resourceViewFlags.requiredFormat = flags.requiredFormat;

      FrameGraphResourcePrivateData privateData={ };
      privateData.parentResourceId = subjacentTargetResource.resourceId;
      privateData.type             = FrameGraphResourceType::TextureView;
      privateData.flags            = resourceViewFlags;
      privateData.usage            = FrameGraphResourceUsage::ImageResource;

      FrameGraphTextureView view={ };
      view.readableName    = String::format("%1 View - Read#%2", subjacentResource.readableName, subjacentTargetResource.resourceId);
      view.arraySliceRange = arraySliceRange;
      view.mipSliceRange   = mipSliceRange;
      view.format          = subjacentResource.format;
      // if(flags.requiredFormat != FrameGraphFormat::Automatic) {
      //   if(validateFormatCompatibility(subjacentResource.format, flags.requiredFormat)) {
      //     view.format = flags.requiredFormat;
      //   }
      //   else
      //     throw std::exception(
      //       String::format("Incompatible formats for texture view detected [Subjacent: %0, View: %1].",
      //         subjacentResource.format,
      //         flags.requiredFormat).c_str());
      // }
      view.mode.set(FrameGraphViewAccessMode::Read);

      subjacentResourcePrivateData.resourceViews.push_back(resource.resourceId);

      m_textureViews[resource.resourceId]         = view;
      m_resourcesPrivateData[resource.resourceId] = privateData;

      return resource;
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
     * \fn  template <typename TPassImplementation> bool PassLinker<TPassImplementation>::isResourceRegistered(FrameGraphResource const&resourceId) const
     *
     * \brief Queries if a resource is registered
     *
     * \tparam  TPassImplementation Type of the pass implementation.
     * \param resourceId  Identifier for the resource.
     *
     * \return  True if the resource is implementation>, false if not.
     **************************************************************************************************/
    bool
      PassBuilder::isResourceRegistered(
        FrameGraphResource const&subjacentTargetResource) const
    {
      return (m_resourcesPrivateData.find(subjacentTargetResource.resourceId) != m_resourcesPrivateData.end());
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
      // This test method can check for overlapping regions for array slices and mip slices
      //  given a specific set of options to test against.
      // If "Write" is checked, this is usually done due to read operations to take place.
      // If a "Read" and "Write" of two subresources does not overlap, both operations 
      //  as such are valid.
      // The whole operation setup is based on first come first serve.
      for(FrameGraphResourceId_t const&id : resourceViews) {
        FrameGraphResourcePrivateData const&p = m_resourcesPrivateData[id];
        FrameGraphTextureView         const&v = m_textureViews[id];

        if(!(p.type == FrameGraphResourceType::TextureView
          && p.usage.check(FrameGraphResourceUsage::ImageResource)))
          continue;

        if(v.arraySliceRange.overlapsWith(arraySliceRange)
          || v.mipSliceRange.overlapsWith(mipSliceRange))
          return true;
      }

      return false;
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
      // This test method can check for overlapping regions for array slices and mip slices
      //  given a specific set of options to test against.
      // If "Write" is checked, this is usually done due to read operations to take place.
      // If a "Read" and "Write" of two subresources does not overlap, both operations 
      //  as such are valid.
      // The whole operation setup is based on first come first serve.
      for(FrameGraphResourceId_t const&id : resourceViews) {
        FrameGraphResourcePrivateData const&p = m_resourcesPrivateData[id];
        FrameGraphTextureView         const&v = m_textureViews[id];

        if(!(p.type == FrameGraphResourceType::TextureView
          && (p.usage.check(FrameGraphResourceUsage::RenderTarget | FrameGraphResourceUsage::DepthTarget))))
          continue;

        if(v.arraySliceRange.overlapsWith(arraySliceRange)
          || v.mipSliceRange.overlapsWith(mipSliceRange))
          return true;
      }

      return false;
    }
  }
}