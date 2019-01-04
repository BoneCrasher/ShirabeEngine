#include <assert.h>
#include "renderer/framegraph/graphbuilder.h"

namespace engine
{
    namespace framegraph
    {
        /**
         * Function signature used for a check for a uid within a list of UIDs.
         */
        template <typename TUID>
        using AlreadyRegisteredFn_t = std::function<bool(std::vector<TUID> const&, TUID const&)>;

        /**
         * Functor usable as a predicate to determine, whether adjacency info was already stored for a
         * node UID.
         */
        template <typename TUID>
        static AlreadyRegisteredFn_t<TUID> alreadyRegisteredFn =
                [] (std::vector<TUID> const &aAdjacency,
                    TUID              const &aPossiblyAdjacent)
        {
            return (aAdjacency.end() != std::find(aAdjacency.begin(), aAdjacency.end(), aPossiblyAdjacent));
        };

        /**
         * The SequenceUIDGene rator class implements IUIDGenerator so that it
         * increments a FrameGraphResourceId_t linearly on each call to 'generate',
         * starting at 'aInitialID' provided to the constructor.
         */
        class CSequenceUIDGenerator
                : public IUIDGenerator<FrameGraphResourceId_t>
        {
        public_constructors:
            /**
             * Default-Construct the generator with an initial Id to start incrementing from.
             *
             * @param aInitialID See brief.
             */
            CSequenceUIDGenerator(FrameGraphResourceId_t const &aInitialID = 1)
                : mId(aInitialID)
            {}

        public_methods:
            /**
             * Increment the counter and return it as a new id.
             *
             * @return See brief.
             */
            FrameGraphResourceId_t generate()
            {
                return (mId++);
            }

        private_members:
            FrameGraphResourceId_t mId;
        };
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CGraphBuilder::CGraphBuilder()
            : mApplicationEnvironment (nullptr                                   )
            , mDisplay                (nullptr                                   )
            , mPassUIDGenerator       (std::make_shared<CSequenceUIDGenerator>(0))
            , mResourceUIDGenerator   (std::make_shared<CSequenceUIDGenerator>(1))
            , mImportedResources      (                                          )
            , mPasses                 (                                          )
            , mResources              (                                          )
            , mResourceData           (                                          )
            , mPassAdjacency          (                                          )
            , mFrameGraph             (nullptr                                   )
    #if defined SHIRABE_FRAMEGRAPH_ENABLE_SERIALIZATION
            , mResourceAdjacency      (                                          )
            , mPassToResourceAdjacency(                                          )
    #endif
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CStdSharedPtr_t<IUIDGenerator<FrameGraphResourceId_t>> CGraphBuilder::resourceUIDGenerator()
        {
            return mResourceUIDGenerator;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        FrameGraphResourceId_t CGraphBuilder::generatePassUID()
        {
            return mPassUIDGenerator->generate();
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CStdUniquePtr_t<CGraph> &CGraphBuilder::graph()
        {
            return mFrameGraph;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CStdSharedPtr_t<SApplicationEnvironment> &CGraphBuilder::applicationEnvironment()
        {
            return mApplicationEnvironment;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CStdSharedPtr_t<wsi::CWSIDisplay> const &CGraphBuilder::display()
        {
            return mDisplay;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGraphBuilder::initialize(
                CStdSharedPtr_t<SApplicationEnvironment> const &aApplicationEnvironment,
                CStdSharedPtr_t<wsi::CWSIDisplay>        const &aDisplay)
        {
            bool const inputInvalid =
                    nullptr == aApplicationEnvironment or
                    nullptr == aDisplay;
            if(inputInvalid)
            {
                return { EEngineStatus::Error };
            }

            applicationEnvironment() = aApplicationEnvironment;
            mDisplay                 = aDisplay;
            graph()                  = makeCStdUniquePtr<CGraph>();

            auto const pseudoSetup = [] (CPassBuilder const &, bool &) -> bool
            {
                return true;
            };

            auto const pseudoExec  = [] (
                    bool                                      const &,
                    CFrameGraphResources                      const &,
                    CStdSharedPtr_t<IFrameGraphRenderContext>       &) -> bool
            {
                return true;
            };

            // Spawn pseudo pass to simplify algorithms and have "empty" execution blocks.
            spawnPass<CallbackPass<bool>>("Pseudo-Pass", pseudoSetup, pseudoExec);

            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGraphBuilder::deinitialize()
        {
            graph()                  = nullptr;
            applicationEnvironment() = nullptr;

            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGraphBuilder::createPassDependency(
                std::string const &aPassSource,
                std::string const &aPassTarget)
        {
            auto const sourcePredicate = [&] (PassMap::value_type const &aPair) -> bool
            {
                return (0 == aPassSource.compare(aPair.second->passName()));
            };

            auto const targetPredicate = [&] (PassMap::value_type const &aPair) -> bool
            {
                return (0 == aPassTarget.compare(aPair.second->passName()));
            };

            PassMap::const_iterator sourcePass = std::find_if(mPasses.begin(), mPasses.end(), sourcePredicate);
            PassMap::const_iterator targetPass = std::find_if(mPasses.begin(), mPasses.end(), targetPredicate);

            bool const containsSourcePass = (mPasses.end() != sourcePass);
            bool const containsTargetPass = (mPasses.end() != targetPass);

            if(!(containsSourcePass && containsTargetPass))
            {
                CLog::Error(logTag(), CString::format("Cannot create pass dependency from %0 to %1. At least one pass was not found.", aPassSource, aPassTarget));
                return { EEngineStatus::Error };
            }

            PassUID_t const &sourcePassUID = sourcePass->second->passUID();
            PassUID_t const &targetPassUID = targetPass->second->passUID();

            createPassDependencyByUID(sourcePassUID, targetPassUID);

            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGraphBuilder::createPassDependencyByUID(
                PassUID_t const &aPassSource,
                PassUID_t const &aPassTarget)
        {
            bool const passAlreadyRegistered = alreadyRegisteredFn<PassUID_t>(mPassAdjacency[aPassSource], aPassTarget);
            if(!passAlreadyRegistered)
            {
                mPassAdjacency[aPassSource].push_back(aPassTarget);
            }

            return { passAlreadyRegistered ? EEngineStatus::Error : EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        SFrameGraphResource CGraphBuilder::registerTexture(
                std::string        const &aReadableName,
                SFrameGraphTexture const &aTexture)
        {
            try
            {
                SFrameGraphTexture &resource = mResourceData.spawnResource<SFrameGraphTexture>();
                resource.assignedPassUID    = 0; // Pre-Pass
                resource.parentResource     = 0; // No internal tree, has to be resolved differently.
                resource.type               = EFrameGraphResourceType::Texture;
                resource.readableName       = aReadableName;
                resource.isExternalResource = true;
                resource.assignTextureParameters(aTexture);

                mResources.push_back(resource.resourceId);

                CStdUniquePtr_t<CPassBase::CMutableAccessor> accessor = mPasses.at(0)->getMutableAccessor(CPassKey<CGraphBuilder>());
                accessor->mutableResourceReferences().push_back(resource.resourceId);

                return resource;
            }
            catch(std::exception)
            {
                CLog::Error(logTag(), CString::format("Failed to register texture %0", aReadableName));
                throw;
            }
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        SFrameGraphResource CGraphBuilder::registerRenderables(
                std::string               const &aReadableIdentifier,
                rendering::RenderableList const &aRenderables)
        {
            try
            {
                SFrameGraphRenderableList &resource = mResourceData.spawnResource<SFrameGraphRenderableList>();
                resource.assignedPassUID    = 0; // Pre-Pass
                resource.parentResource     = 0; // No internal tree, has to be resolved differently.
                resource.type               = EFrameGraphResourceType::RenderableList;
                resource.readableName       = aReadableIdentifier;
                resource.isExternalResource = true;
                resource.renderableList     = aRenderables;

                mResources.push_back(resource.resourceId);

                return resource;
            }
            catch(std::exception)
            {
                CLog::Error(logTag(), CString::format("Failed to register renderable list %0", aReadableIdentifier));
                throw;
            }
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<CStdUniquePtr_t<CGraph>> CGraphBuilder::compile()
        {
            CStdUniquePtr_t<CGraph::CMutableAccessor> accessor = graph()->getMutableAccessor(CPassKey<CGraphBuilder>());

            // First (No-Op, automatically performed on call to 'setPassDependency(...)':
            //   Evaluate explicit pass dependencies without resource flow.

            // Second: Evaluate all pass resources and their implicit relations to passes and other resources.
            for(PassMap::value_type const &assignment : graph()->passes())
            {
                CEngineResult<> const collection = collectPass(assignment.second);
                if(not collection.successful())
                {
                    CLog::Error(logTag(), "Failed to perform pass collection.");
                    return { EEngineStatus::Error };
                }
            }

            // Third: Sort the passes by their relationships and dependencies.
            CEngineResult<> const topologicalPassSort = topologicalSort<PassUID_t>(accessor->mutablePassExecutionOrder());
            if(not topologicalPassSort.successful())
            {
                CLog::Error(logTag(), "Failed to perform topologicalSort(...) for passes on graph compilation.");
                return { EEngineStatus::Error };
            }

#if defined SHIRABE_FRAMEGRAPH_ENABLE_SERIALIZATION

            // Fourth: Sort the resources by their relationships and dependencies.
            bool const topologicalResourceSortSuccessful = topologicalSort<FrameGraphResourceId_t>(accessor->mutableResourceOrder());
            if(!topologicalResourceSortSuccessful)
            {
                CLog::Error(logTag(), "Failed to perform topologicalSort(...) for resources on graph compilation.");
                return nullptr;
            }
#endif

#if defined SHIRABE_DEBUG || defined SHIRABE_TEST

            CEngineResult<> const validation = validate(accessor->passExecutionOrder());
            if(not validation.successful())
            {
                CLog::Error(logTag(), "Failed to perform validation(...) on graph compilation.");
                return { EEngineStatus::Error };
            }

#endif

            // Move out the current adjacency state to the frame graph, so that it can be used for further processing.
            // It is no more needed at this point within the GraphBuilder.
            accessor->mutablePassAdjacency() = std::move(this->mPassAdjacency);
            accessor->mutableResources()     = std::move(this->mResources);
            accessor->mutableResourceData().mergeIn(this->mResourceData);

#if defined SHIRABE_FRAMEGRAPH_ENABLE_SERIALIZATION
            accessor->mutableResourceAdjacency()       = std::move(this->mResourceAdjacency);
            accessor->mutablePassToResourceAdjacency() = std::move(this->mPassToResourceAdjacency);
#endif

            return { EEngineStatus::Ok, std::move(graph()) };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<FrameGraphResourceId_t> CGraphBuilder::findSubjacentResource(
               SFrameGraphResourceMap const &aResources,
               SFrameGraphResource    const &aStart)
        {
            if(aStart.parentResource == 0)
            {
                return { EEngineStatus::Ok, aStart.resourceId };
            }
            else
            {
                if(aResources.find(aStart.parentResource) != aResources.end())
                {
                    return findSubjacentResource(aResources, aResources.at(aStart.parentResource));
                }
                else
                {
                    return { EEngineStatus::Error };
                }
            }
        }        
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGraphBuilder::collectPass(CStdSharedPtr_t<CPassBase> aPass)
        {
            if(nullptr == aPass)
            {
                return { EEngineStatus::Error };
            }

            CStdUniquePtr_t<CPassBase::CMutableAccessor> accessor = aPass->getMutableAccessor(CPassKey<CGraphBuilder>());

            FrameGraphResourceIdList const resources = accessor->mutableResourceReferences();
            for(FrameGraphResourceId_t const &resourceId : resources)
            {
                SFrameGraphResource &resource = *mResourceData.getMutable<SFrameGraphResource>(resourceId);

                // For each underlying OR imported resource (textures/buffers or whatever importable)
                if(0 == resource.parentResource)
                {
                    if(EFrameGraphResourceType::Texture == resource.type)
                    {
#if defined SHIRABE_FRAMEGRAPH_ENABLE_SERIALIZATION
                        // Map the resources to it's pass appropriately
                        bool const alreadyRegistered = alreadyRegisteredFn<FrameGraphResourceId_t>(mPassToResourceAdjacency[aPass->passUID()], resource.resourceId);
                        if(!alreadyRegistered)
                        {
                            mPassToResourceAdjacency[aPass->passUID()].push_back(resource.resourceId);
                        }
#endif
                        continue;
                    }
                }
                // For each derived resource (views)
                else
                {
                    // Avoid internal references for passes!
                    // If the edge from pass k to pass k+1 was not added yet.
                    // Create edge: Parent-->Source
                    SFrameGraphResource const &parentResource = *mResourceData.get<SFrameGraphResource>(resource.parentResource);
                    PassUID_t           const &passUID        = aPass->passUID();

                    // Create a pass dependency used for topologically sorting the graph
                    // to derive the pass execution order.
                    if(parentResource.assignedPassUID != passUID)
                    {
                        createPassDependencyByUID(parentResource.assignedPassUID, passUID);
                    }

#if defined SHIRABE_FRAMEGRAPH_ENABLE_SERIALIZATION

                    // Do the same for the resources!
                    bool const resourceAdjacencyAlreadyRegistered = alreadyRegisteredFn<FrameGraphResourceId_t>(mResourceAdjacency[parentResource.resourceId], resource.resourceId);
                    if(!resourceAdjacencyAlreadyRegistered)
                    {
                        mResourceAdjacency[parentResource.resourceId].push_back(resource.resourceId);
                    }

                    // And map the resources to it's pass appropriately
                    bool const resourceEdgeAlreadyRegistered = alreadyRegisteredFn<FrameGraphResourceId_t>(mPassToResourceAdjacency[passUID], resource.resourceId);
                    if(!resourceEdgeAlreadyRegistered)
                    {
                        mPassToResourceAdjacency[passUID].push_back(resource.resourceId);
                    }

#endif

                    if(EFrameGraphResourceType::TextureView == resource.type)
                    {
                        SFrameGraphTexture     &texture     = *mResourceData.getMutable<SFrameGraphTexture>    (resource.subjacentResource);
                        SFrameGraphTextureView &textureView = *mResourceData.getMutable<SFrameGraphTextureView>(resource.resourceId);

                        // Auto adjust format if requested
                        if(FrameGraphFormat_t::Automatic == textureView.format)
                        {
                            textureView.format = texture.format;
                        }

                        SFrameGraphAttachmentCollection &attachments = mResourceData.getAttachments();

                        // Set required usage flags, so that the subjacent texture is properly created.
                        // This will also derive all attachements of the to be created render pass.
                        // Since we avoided simultaneous R/W, setting just one of the attachment flags
                        // per texture view will work out.
                        if(textureView.mode.check(EFrameGraphViewAccessMode::Read))
                        {
                            texture.requestedUsage.set(EFrameGraphResourceUsage::InputAttachment);
                            attachments.addInputAttachment(passUID, resource.resourceId);
                        }
                        else if(textureView.mode.check(EFrameGraphViewAccessMode::Write))
                        {
                            if(EFrameGraphViewSource::Color == textureView.source)
                            {
                                texture.requestedUsage.set(EFrameGraphResourceUsage::ColorAttachment);
                                attachments.addColorAttachment(passUID, resource.resourceId);
                            }
                            else if(EFrameGraphViewSource::Depth == textureView.source)
                            {
                                texture.requestedUsage.set(EFrameGraphResourceUsage::DepthAttachment);
                                attachments.addDepthAttachment(passUID, resource.resourceId);
                            }
                        }
                        else
                        {
                            texture.requestedUsage.set(EFrameGraphResourceUsage::Unused);
                        }
                    }
                    else if(resource.type == EFrameGraphResourceType::BufferView)
                    {
                        // TODO
                    }
                    else if(resource.type == EFrameGraphResourceType::RenderableListView)
                    {
                        // TODO
                    }
                }
            }

            // Now that the internal resource references were adjusted and duplicates removed, confirm the index in the graph builder
            for(FrameGraphResourceId_t const &id : accessor->resourceReferences())
            {
                mResources.push_back(id);
            }

#ifdef SHIRABE_DEBUG
            CLog::Verbose(logTag(), CString::format("Current Adjacency State collecting pass '%0':", aPass->passUID()));
            for(auto const &[uid, adjacentUIDs] : mPassAdjacency)
            {
                CLog::Verbose(logTag(), CString::format("  Pass-UID: %0", uid));

                for(PassUID_t const &adjacentUID : adjacentUIDs)
                {
                    CLog::Verbose(logTag(), CString::format("    Adjacent Pass-UID: %0", adjacentUID));
                }
            }
#endif

            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGraphBuilder::validate(std::stack<PassUID_t> const &aPassOrder)
        {
            bool allBindingsValid = true;

            for(RefIndex_t::value_type const &textureViewRef : mResourceData.textureViews())
            {
                SFrameGraphTextureView const &textureView = *mResourceData.get<SFrameGraphTextureView>(textureViewRef);

                // Adjust resource access flags in the subjacent resource to have the texture creation configure
                // everything appropriately.
                FrameGraphResourceId_t const  subjacentResourceId =  textureView.subjacentResource;
                SFrameGraphTexture     const &texture             = *mResourceData.get<SFrameGraphTexture>(subjacentResourceId);

                bool viewBindingValid = validateTextureView(texture, textureView);
                allBindingsValid &= viewBindingValid;

            } // foreach TextureView

            return { allBindingsValid ? EEngineStatus::Ok : EEngineStatus::Error };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CGraphBuilder::validateTextureView(
                SFrameGraphTexture     const& aTexture,
                SFrameGraphTextureView const& atextureView)
        {
            bool const usageValid             = validateTextureUsage(aTexture);
            bool const formatValid            = validateTextureFormat(aTexture, atextureView);
            bool const subresourceAccessValid = validateTextureSubresourceAccess(aTexture, atextureView);

            return (usageValid && formatValid && subresourceAccessValid);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CGraphBuilder::validateTextureUsage(SFrameGraphTexture const &aTexture)
        {
            // Cross both bitsets... permittedUsage should fully contain requestedUsage
            return aTexture.permittedUsage.check(aTexture.requestedUsage);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CGraphBuilder::validateTextureFormat(
                SFrameGraphTexture     const &aTexture,
                SFrameGraphTextureView const &aTextureView)
        {
            using FormatValue_t = std::underlying_type_t<FrameGraphFormat_t>;

            auto const nearestPowerOf2Ceil = [] (FormatValue_t value) -> uint64_t
            {
                uint64_t power = 2;
                while(value >>= 1)
                    power <<= 1;
                return power;
            };

            FrameGraphFormat_t const &source = aTexture.format;
            FrameGraphFormat_t const &target = aTextureView.format;

            bool formatsCompatible = false;

            switch(target) {
            case FrameGraphFormat_t::Undefined:
            case FrameGraphFormat_t::Structured:
                // Invalid!
                formatsCompatible = false;
                break;
            case FrameGraphFormat_t::Automatic:
                // Should never be accessed as automatic is resolved beforehand though...
                break;
            default:
                FormatValue_t sourceUID = static_cast<FormatValue_t>(source);
                FormatValue_t targetUID = static_cast<FormatValue_t>(target);
                // For now -> Simple test: Bitdepth...
                formatsCompatible = (nearestPowerOf2Ceil(sourceUID) == nearestPowerOf2Ceil(targetUID));
                break;
            }

            return formatsCompatible;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CGraphBuilder::validateTextureSubresourceAccess(
                SFrameGraphTexture     const &aTexture,
                SFrameGraphTextureView const &aTextureView)
        {
            CRange const &arraySliceRange = aTextureView.arraySliceRange;
            CRange const &mipSliceRange   = aTextureView.mipSliceRange;

            bool arraySliceRangeValid = true;
            arraySliceRangeValid &= (arraySliceRange.offset < aTexture.arraySize);
            arraySliceRangeValid &= ((arraySliceRange.offset + arraySliceRange.length) <= aTexture.arraySize);

            bool mipSliceRangeValid = true;
            mipSliceRangeValid &= (mipSliceRange.offset < aTexture.mipLevels);
            mipSliceRangeValid &= ((mipSliceRange.offset + mipSliceRange.length) <= aTexture.mipLevels);

            bool const valid = (arraySliceRangeValid && mipSliceRangeValid);
            return valid;
        }
        //<-----------------------------------------------------------------------------
    }
}
