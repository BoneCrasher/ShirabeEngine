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
      GraphBuilder                               const&graphBuilder)
      : m_passUID(passId)
      , m_resourceIdGenerator(resourceUIDGenerator)
      , m_graphBuilder(graphBuilder)
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
      FrameGraphTexture resource = desc;
      resource.assignedPassUID   = m_passUID;
      resource.resourceId        = m_resourceIdGenerator->generate();
      resource.parentResource    = 0;
      resource.subjacentResource = resource.resourceId; // This is a trick to keep algorithms consistent while supporting r/w from textures and t-views.
      resource.readableName      = name;
      resource.type              = FrameGraphResourceType::Texture;

      m_resourceData.addTexture(resource, desc);
      m_resources[resource.resourceId] = RefWrapper<FrameGraphResource>(*m_resourceData.getMutableTexture(resource.resourceId));

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
      #if defined SHIRABE_DEBUG || defined SHIRABE_TEST 

      if(isTextureBeingReadInSubresourceRange(m_resourceData.textureViews(), m_resourceData, sourceResource, arraySliceRange, mipSliceRange))
        throw std::runtime_error(
          String::format(
            "Resource is already being read or written at the specified ranges (Array: %0[%1]; Mip: %2[%3])",
            arraySliceRange.offset, arraySliceRange.length, mipSliceRange.offset, mipSliceRange.length).c_str());

      if(isTextureBeingWrittenInSubresourceRange(m_resourceData.textureViews(), m_resourceData, sourceResource, arraySliceRange, mipSliceRange))
        throw std::runtime_error(
          String::format(
            "Resource is already being read or written at the specified ranges (Array: %0[%1]; Mip: %2[%3])",
            arraySliceRange.offset, arraySliceRange.length, mipSliceRange.offset, mipSliceRange.length).c_str());

      #endif

      FrameGraphTextureView view={ };
      view.arraySliceRange = arraySliceRange;
      view.mipSliceRange   = mipSliceRange;
      view.format          = flags.requiredFormat;
      view.mode.set(FrameGraphViewAccessMode::Write);
      if(flags.writeTarget == FrameGraphWriteTarget::Color)
        view.source = FrameGraphViewSource::Color;
      if(flags.writeTarget == FrameGraphWriteTarget::Depth)
        view.source = FrameGraphViewSource::Depth;

      view.assignedPassUID    = m_passUID;
      view.resourceId         = m_resourceIdGenerator->generate();
      view.parentResource     = sourceResource.resourceId;
      view.subjacentResource  =
        (sourceResource.type == FrameGraphResourceType::Texture
          ? sourceResource.resourceId
          : sourceResource.subjacentResource);
      view.readableName       = String::format("TextureView ID %0 - Write #%1", view.resourceId, sourceResource.resourceId);
      view.type               = FrameGraphResourceType::TextureView;

      m_resourceData.addTextureView(view.resourceId, view);
      m_resources[view.resourceId] = RefWrapper<FrameGraphResource>(*m_resourceData.getMutableTextureView(view.resourceId));

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
      #if defined SHIRABE_DEBUG || defined SHIRABE_TEST 

      if(isTextureBeingWrittenInSubresourceRange(m_resourceData.textureViews(), m_resourceData, sourceResource, arraySliceRange, mipSliceRange))
        throw std::runtime_error(
          String::format(
            "Resource is already being read or written at the specified ranges (Array: %0[%1]; Mip: %2[%3])",
            arraySliceRange.offset, arraySliceRange.length, mipSliceRange.offset, mipSliceRange.length).c_str());

      #endif

      FrameGraphTextureView view={ };
      view.arraySliceRange = arraySliceRange;
      view.mipSliceRange   = mipSliceRange;
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

      m_resourceData.addTextureView(view.resourceId, view);
      m_resources[view.resourceId] = RefWrapper<FrameGraphResource>(*m_resourceData.getMutableTextureView(view.resourceId));

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
      FrameGraphResource resource{};
      resource.assignedPassUID    = m_passUID;
      resource.resourceId         = m_resourceIdGenerator->generate();
      resource.readableName       = collectionName;
      resource.parentResource     = renderableListResource.resourceId;
      resource.subjacentResource  = renderableListResource.resourceId;
      resource.type               = FrameGraphResourceType::RenderableListView;
      resource.isExternalResource = false;

      FrameGraphRenderableListView view{};

      Renderer::RenderableList const&renderableList = m_graphBuilder.getRenderableList(renderableListResource);
      for(uint64_t k=0; k<renderableList.size(); ++k)
        view.renderableRefIndices.push_back(k);

      m_resourceData.addRenderableListView(resource.resourceId, view);

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
        FrameGraphTextureViewMap const&resourceViews,
        FrameGraphResources      const&resources,
        FrameGraphResource       const&sourceResource,
        Range                    const&arraySliceRange,
        Range                    const&mipSliceRange)
    {
      // This test method can check for overlapping regions for array slices and mip slices
      //  given a specific set of options to test against.
      // If "Write" is checked, this is usually done due to read operations to take place.
      // If a "Read" and "Write" of two subresources does not overlap, both operations 
      //  as such are valid.
      // The whole operation setup is based on first come first serve.
      for(FrameGraphTextureViewMap::value_type const&viewAssignment : resourceViews) {

        FrameGraphTextureView const&view = viewAssignment.second;
        if(!view.mode.check(FrameGraphViewAccessMode::Read))
          continue;
        
        if(
          sourceResource.subjacentResource == view.subjacentResource
          && view.arraySliceRange.overlapsWith(arraySliceRange)
          && view.mipSliceRange.overlapsWith(mipSliceRange))
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
        FrameGraphTextureViewMap const&resourceViews,
        FrameGraphResources      const&resources,
        FrameGraphResource       const&sourceResource,
        Range                    const&arraySliceRange,
        Range                    const&mipSliceRange)
    {
      // This test method can check for overlapping regions for array slices and mip slices
      //  given a specific set of options to test against.
      // If "Write" is checked, this is usually done due to read operations to take place.
      // If a "Read" and "Write" of two subresources does not overlap, both operations 
      //  as such are valid.
      // The whole operation setup is based on first come first serve.
      for(FrameGraphTextureViewMap::value_type const&viewAssignment : resourceViews) {

        FrameGraphTextureView const&view = viewAssignment.second;
        if(!view.mode.check(FrameGraphViewAccessMode::Write))
          continue;

        if(
          sourceResource.subjacentResource == view.subjacentResource
          && view.arraySliceRange.overlapsWith(arraySliceRange)
          && view.mipSliceRange.overlapsWith(mipSliceRange))
          return true;
      }

      return false;
    }

  }
}