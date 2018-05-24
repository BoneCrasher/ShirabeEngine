#include "Renderer/FrameGraph/PassBuilder.h"
#include "Core/String.h"

#include "Renderer/FrameGraph/GraphBuilder.h"

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
      Ptr<IUIDGenerator<FrameGraphResourceId_t>>       resourceUIDGenerator,
      FrameGraphMutableResources                      &resourceData)
      : m_passUID(passId)
      , m_resourceIdGenerator(resourceUIDGenerator)
      , m_resourceData(resourceData)
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
      FrameGraphTexture &resource = m_resourceData.spawnResource<FrameGraphTexture>();
      resource.assignedPassUID   = m_passUID;
      resource.parentResource    = 0;
      resource.subjacentResource = resource.resourceId; // This is a trick to keep algorithms consistent while supporting r/w from textures and t-views.
      resource.readableName      = name;
      resource.type              = FrameGraphResourceType::Texture;
      resource.assignTextureParameters(desc);

      m_resources.push_back(resource.resourceId);

      return resource;
    }

    void PassBuilder::adjustArrayAndMipSliceRanges(
      FrameGraphResources const&resourceData,
      FrameGraphResource  const&sourceResource,
      Range const&arraySliceRange,
      Range const&mipSliceRange,
      Range      &adjustedArraySliceRange,
      Range      &adjustedMipSliceRange) 
    {
      adjustedArraySliceRange = arraySliceRange;
      adjustedMipSliceRange   = mipSliceRange;
      if(sourceResource.type == FrameGraphResourceType::TextureView)
      {
        Optional<Ptr<FrameGraphTexture>> const&subjacentRef = resourceData.get<FrameGraphTexture>(sourceResource.subjacentResource);
        
        #if defined SHIRABE_DEBUG || defined SHIRABE_TEST 
        if(!subjacentRef.has_value())
          throw std::runtime_error("Subjacent resource handle not found.");

        if(!(*subjacentRef))
          throw std::runtime_error("Subjacent resource handle is empty.");
        #endif

        Optional<Ptr<FrameGraphTextureView>> const&parentRef = resourceData.get<FrameGraphTextureView>(sourceResource.resourceId);

        #if defined SHIRABE_DEBUG || defined SHIRABE_TEST 
        if(!parentRef.has_value())
          throw std::runtime_error("Parent resource handle not found.");

        if(!(*parentRef))
          throw std::runtime_error("Parent resource handle is empty.");
        #endif

        FrameGraphTexture     const&subjacent = **subjacentRef;
        FrameGraphTextureView const&parent    = **parentRef;

        adjustedArraySliceRange.offset = (parent.arraySliceRange.offset + arraySliceRange.offset);
        adjustedMipSliceRange.offset   = (parent.mipSliceRange.offset   + mipSliceRange.offset);

        #if defined SHIRABE_DEBUG || defined SHIRABE_TEST 
        bool arraySliceRangeOutOfBounds = (adjustedArraySliceRange.length > parent.arraySliceRange.length) || (adjustedArraySliceRange.offset + adjustedArraySliceRange.length) > subjacent.arraySize;
        bool mipSliceRangeOutOfBounds   = (adjustedMipSliceRange.length   > parent.mipSliceRange.length)   || (adjustedMipSliceRange.offset   + adjustedMipSliceRange.length)   > subjacent.mipLevels;

        if(arraySliceRangeOutOfBounds || mipSliceRangeOutOfBounds)
          throw std::runtime_error(String::format("Derived subresource range out of bounds (Array:%0, Mip:%1).", arraySliceRangeOutOfBounds, mipSliceRangeOutOfBounds));
        #endif

      }
      else {

        #if defined SHIRABE_DEBUG || defined SHIRABE_TEST 
        Optional<Ptr<FrameGraphTexture>> const&subjacentRef = resourceData.get<FrameGraphTexture>(sourceResource.resourceId);

        FrameGraphTexture const&subjacent = **subjacentRef;

        bool arraySliceRangeOutOfBounds = (adjustedArraySliceRange.offset + adjustedArraySliceRange.length) > subjacent.arraySize;
        bool mipSliceRangeOutOfBounds   = (adjustedMipSliceRange.offset   + adjustedMipSliceRange.length)   > subjacent.mipLevels;

        if(arraySliceRangeOutOfBounds || mipSliceRangeOutOfBounds)
          throw std::runtime_error(String::format("Subresource range out of bounds (Array:%0, Mip:%1).", arraySliceRangeOutOfBounds, mipSliceRangeOutOfBounds));
        #endif

      }
    }

    void PassBuilder::validateArrayAndMipSliceRanges(
      FrameGraphResources const&resourceData,
      FrameGraphResource  const&sourceResource,
      Range               const&arraySliceRange,
      Range               const&mipSliceRange,
      bool                      validateReads,
      bool                      validateWrites)
    {
      #if defined SHIRABE_DEBUG || defined SHIRABE_TEST 
      if(isTextureBeingReadInSubresourceRange(resourceData.textureViews(), resourceData, sourceResource, arraySliceRange, mipSliceRange))
        throw std::runtime_error(
          String::format(
            "Resource is already being read or written at the specified ranges (Array: %0[%1]; Mip: %2[%3])",
            arraySliceRange.offset, arraySliceRange.length, mipSliceRange.offset, mipSliceRange.length).c_str());

      if(isTextureBeingWrittenInSubresourceRange(resourceData.textureViews(), resourceData, sourceResource, arraySliceRange, mipSliceRange))
        throw std::runtime_error(
          String::format(
            "Resource is already being read or written at the specified ranges (Array: %0[%1]; Mip: %2[%3])",
            arraySliceRange.offset, arraySliceRange.length, mipSliceRange.offset, mipSliceRange.length).c_str());
      #endif

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
      Range
        adjustedArraySliceRange = arraySliceRange,
        adjustedMipSliceRange   = mipSliceRange;

      adjustArrayAndMipSliceRanges(m_resourceData, sourceResource, arraySliceRange, mipSliceRange, adjustedArraySliceRange, adjustedMipSliceRange);
      validateArrayAndMipSliceRanges(m_resourceData, sourceResource, adjustedArraySliceRange, adjustedMipSliceRange, true, true);

      FrameGraphTextureView &view = m_resourceData.spawnResource<FrameGraphTextureView>();
      view.arraySliceRange = adjustedArraySliceRange;
      view.mipSliceRange   = adjustedMipSliceRange;
      view.format          = flags.requiredFormat;
      view.mode.set(FrameGraphViewAccessMode::Write);
      if(flags.writeTarget == FrameGraphWriteTarget::Color)
        view.source = FrameGraphViewSource::Color;
      if(flags.writeTarget == FrameGraphWriteTarget::Depth)
        view.source = FrameGraphViewSource::Depth;

      view.assignedPassUID    = m_passUID;
      view.parentResource     = sourceResource.resourceId;
      view.subjacentResource  =
        (sourceResource.type == FrameGraphResourceType::Texture
          ? sourceResource.resourceId
          : sourceResource.subjacentResource);
      view.readableName       = String::format("TextureView ID %0 - Write #%1", view.resourceId, sourceResource.resourceId);
      view.type               = FrameGraphResourceType::TextureView;

      m_resources.push_back(view.resourceId);

      return view;

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
      Range
        adjustedArraySliceRange = arraySliceRange,
        adjustedMipSliceRange   = mipSliceRange;

      adjustArrayAndMipSliceRanges(m_resourceData, sourceResource, arraySliceRange, mipSliceRange, adjustedArraySliceRange, adjustedMipSliceRange);
      validateArrayAndMipSliceRanges(m_resourceData, sourceResource, adjustedArraySliceRange, adjustedMipSliceRange, false, true);

      FrameGraphTextureView &view = m_resourceData.spawnResource<FrameGraphTextureView>();
      view.arraySliceRange = adjustedArraySliceRange;
      view.mipSliceRange   = adjustedMipSliceRange;
      view.format          = flags.requiredFormat;
      view.mode.set(FrameGraphViewAccessMode::Read);
      if(flags.source == FrameGraphReadSource::Color)
        view.source = FrameGraphViewSource::Color;
      if(flags.source == FrameGraphReadSource::Depth)
        view.source = FrameGraphViewSource::Depth;

      view.assignedPassUID   = m_passUID;
      view.resourceId        = m_resourceIdGenerator->generate();
      view.parentResource    = sourceResource.resourceId;
      view.subjacentResource =
        (sourceResource.type == FrameGraphResourceType::Texture
          ? sourceResource.resourceId
          : sourceResource.subjacentResource);
      view.readableName      = String::format("TextureView ID %0 - Read #%1", view.resourceId, sourceResource.resourceId);
      view.type              = FrameGraphResourceType::TextureView;

      m_resources.push_back(view.resourceId);

      return view;
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
      PassBuilder::importRenderables(
        std::string        const&collectionName,
        FrameGraphResource const&renderableListResource)
    {
      FrameGraphRenderableListView&resource = m_resourceData.spawnResource<FrameGraphRenderableListView>();
      resource.assignedPassUID    = m_passUID;
      resource.readableName       = collectionName;
      resource.parentResource     = renderableListResource.resourceId;
      resource.subjacentResource  = renderableListResource.resourceId;
      resource.type               = FrameGraphResourceType::RenderableListView;
      resource.isExternalResource = false;
      
      Optional<Ptr<FrameGraphRenderableList>> const&listRef = m_resourceData.get<FrameGraphRenderableList>(renderableListResource.resourceId);

      #if defined SHIRABE_DEBUG || defined SHIRABE_TEST 

      #endif

      FrameGraphRenderableList const&list = **listRef;

      for(uint64_t k=0; k<list.renderableList.size(); ++k)
        resource.renderableRefIndices.push_back(k);

      m_resources[resource.resourceId] = resource;

      return resource;
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
        FrameGraphResources::RefIndex const&resourceViews,
        FrameGraphResources           const&resources,
        FrameGraphResource            const&sourceResource,
        Range                         const&arraySliceRange,
        Range                         const&mipSliceRange)
    {
      // This test method can check for overlapping regions for array slices and mip slices
      //  given a specific set of options to test against.
      // If "Write" is checked, this is usually done due to read operations to take place.
      // If a "Read" and "Write" of two subresources does not overlap, both operations 
      //  as such are valid.
      // The whole operation setup is based on first come first serve.
      for(FrameGraphResources::RefIndex::value_type const&viewRef : resourceViews)
      {
        FrameGraphTextureView const&view = resources.get<FrameGraphTextureView>(viewRef.second);
        bool correctMode = view.mode.check(FrameGraphViewAccessMode::Read);
        if(!correctMode)
          continue;

        bool commonPass      = sourceResource.assignedPassUID   == view.assignedPassUID;
        bool commonId        = sourceResource.resourceId        == view.resourceId;
        bool commonSubjacent = sourceResource.subjacentResource == view.subjacentResource;
        if(!(commonPass && commonId && commonSubjacent))
          continue;

        bool arraySliceOverlap = view.arraySliceRange.overlapsWith(arraySliceRange);
        if(!arraySliceOverlap)
          continue;

        bool mipSliceOverlap = view.mipSliceRange.overlapsWith(mipSliceRange);
        if(!mipSliceOverlap)
          continue;

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
        FrameGraphResources::RefIndex const&resourceViews,
        FrameGraphResources           const&resources,
        FrameGraphResource            const&sourceResource,
        Range                         const&arraySliceRange,
        Range                         const&mipSliceRange)
    {
      // This test method can check for overlapping regions for array slices and mip slices
      //  given a specific set of options to test against.
      // If "Write" is checked, this is usually done due to read operations to take place.
      // If a "Read" and "Write" of two subresources does not overlap, both operations 
      //  as such are valid.
      // The whole operation setup is based on first come first serve.
      for(FrameGraphResources::RefIndex::value_type const&viewRef : resourceViews)
      {
        FrameGraphTextureView const&view = resources.get<FrameGraphTextureView>(viewRef.second);

        bool correctMode = view.mode.check(FrameGraphViewAccessMode::Write);        
        if(!correctMode)
          continue;

        bool commonPass      = sourceResource.assignedPassUID   == view.assignedPassUID;
        bool commonId        = sourceResource.resourceId        == view.resourceId;
        bool commonSubjacent = sourceResource.subjacentResource == view.subjacentResource;
        if(!(commonPass && !commonId && commonSubjacent))
          continue;

        bool arraySliceOverlap = view.arraySliceRange.overlapsWith(arraySliceRange);
        if(!arraySliceOverlap)
          continue;

        bool mipSliceOverlap = view.mipSliceRange.overlapsWith(mipSliceRange);
        if(!mipSliceOverlap)
          continue;

        return true;
      }

      return false;
    }

  }
}