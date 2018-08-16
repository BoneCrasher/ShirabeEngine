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
         *
         */
        template <typename TUID>
        static AlreadyRegisteredFn_t<TUID> alreadyRegisteredFn =
                [] (std::vector<TUID> const&adjacency, TUID const&possiblyAdjacent)
        {
            return std::find(adjacency.begin(), adjacency.end(), possiblyAdjacent) != adjacency.end();
        };

        /**
         * The SequenceUIDGenerator class implements IUIDGenerator so that it
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
            : mPassUIDGenerator(std::make_shared<CSequenceUIDGenerator>(0))
            , mResourceUIDGenerator(std::make_shared<CSequenceUIDGenerator>(1))
            , mFrameGraph(nullptr)
            , mImportedResources()
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
        bool CGraphBuilder::initialize(CStdSharedPtr_t<SApplicationEnvironment> const &aApplicationEnvironment)
        {
            assert(aApplicationEnvironment != nullptr);

            applicationEnvironment() = aApplicationEnvironment;
            graph()                  = makeCStdUniquePtr<CGraph>();

            auto const pseudoSetup = [] (PassBuilder const&, bool&) -> bool
            {
                return true;
            };

            auto const pseudoExec = [] (
                    bool                                      const&,
                    CFrameGraphResources                      const&,
                    CStdSharedPtr_t<IFrameGraphRenderContext>      &) -> bool
            {
                return true;
            };

            // Spawn pseudo pass to simplify algorithms and have "empty" execution blocks.
            spawnPass<CallbackPass<bool>>("Pseudo-Pass", pseudoSetup, pseudoExec);

            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CGraphBuilder::deinitialize()
        {
            graph()                  = nullptr;
            applicationEnvironment() = nullptr;

            return true;
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

                CStdUniquePtr_t<CPassBase::CMutableAccessor> accessor = mPasses.at(0)->getMutableAccessor(PassKey<CGraphBuilder>());
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
        CStdUniquePtr_t<CGraph> CGraphBuilder::compile()
        {
            CStdUniquePtr_t<CGraph::CMutableAccessor> accessor = graph()->getMutableAccessor(PassKey<CGraphBuilder>());

            for(PassMap::value_type const &assignment : graph()->passes())
                assert(true == collectPass(assignment.second));

            bool const topologicalPassSortSuccessful = topologicalSort<PassUID_t>(accessor->mutablePassExecutionOrder());
            if(!topologicalPassSortSuccessful)
            {
                CLog::Error(logTag(), "Failed to perform topologicalSort(...) for passes on graph compilation.");
                return nullptr;
            }

            bool const topologicalResourceSortSuccessful = topologicalSort<FrameGraphResourceId_t>(accessor->mutableResourceOrder());
            if(!topologicalResourceSortSuccessful)
            {
                CLog::Error(logTag(), "Failed to perform topologicalSort(...) for resources on graph compilation.");
                return nullptr;
            }

#if defined SHIRABE_DEBUG || defined SHIRABE_TEST

            bool const validationSuccessful = validate(accessor->passExecutionOrder());
            if(!validationSuccessful) {
                CLog::Error(logTag(), "Failed to perform validation(...) on graph compilation.");
                return nullptr;
            }

#endif

            // Move out the current adjacency state to the frame graph, so that it can be used for further processing.
            // It is no more needed at this point within the GraphBuilder.
            accessor->mutablePassAdjacency()           = std::move(this->mPassAdjacency);
            accessor->mutableResourceAdjacency()       = std::move(this->mResourceAdjacency);
            accessor->mutablePassToResourceAdjacency() = std::move(this->mPassToResourceAdjacency);
            accessor->mutableResources()               = std::move(this->mResources);
            accessor->mutableResourceData().mergeIn(this->mResourceData);

            return std::move(graph());
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        std::optional<T&> getResource(FrameGraphResourceId_t const &aResourceId)
        {
            if((mResources.find(aResourceId) == mResources.end()))
                throw std::runtime_error("Resource not found");

            try
            {
                std::optional<T&> optional = std::get<T>(mResources.at(aResourceId));
                return optional;
            }
            catch(std::bad_variant_access const &aBVE)
            {
                CLog::Error(logTag(), aBVE.what());
                return std::optional<T&>();
            }
            catch(std::runtime_error const &aRTE)
            {
                CLog::Error(logTag(), aRTE.what());
                return std::optional<T&>();
            }
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        FrameGraphResourceId_t CGraphBuilder::findSubjacentResource(
               SFrameGraphResourceMap const &aResources,
               SFrameGraphResource    const &aStart)
        {
            if(aStart.parentResource == 0)
            {
                return aStart.resourceId;
            }
            else {
                if(aResources.find(aStart.parentResource) != aResources.end())
                {
                    return findSubjacentResource(aResources, aResources.at(aStart.parentResource));
                }
                else
                    throw std::runtime_error("Resource not found...");
            }
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CGraphBuilder::collectPass(CStdSharedPtr_t<CPassBase> aPass)
        {
            assert(nullptr != aPass);

            CStdUniquePtr_t<CPassBase::CMutableAccessor> accessor = aPass->getMutableAccessor(PassKey<CGraphBuilder>());

            // Derive:
            // - Resource creation requests.
            // - Edges: pass->pass and resource[view]->resource[view] for graph generation!
            // - ???
            FrameGraphResourceIdList  resources = accessor->mutableResourceReferences();

            for(FrameGraphResourceId_t const&resource : resources)
            {
                SFrameGraphResource &r = *mResourceData.getMutable<SFrameGraphResource>(resource);

                // For each underlying OR imported resource (textures/buffers or whatever importable)
                if(r.parentResource == 0)
                {
                    switch(r.type)
                    {
                    case EFrameGraphResourceType::Imported:
                        break;
                    case EFrameGraphResourceType::RenderableList:
                    case EFrameGraphResourceType::Buffer:
                    case EFrameGraphResourceType::Texture:
                        // And map the resources to it's pass appropriately
                        bool const alreadyRegistered = alreadyRegisteredFn<FrameGraphResourceId_t>(mPassToResourceAdjacency[aPass->passUID()], r.resourceId);
                        if(!alreadyRegistered)
                        {
                            mPassToResourceAdjacency[aPass->passUID()].push_back(r.resourceId);
                        }
                        break;
                    }
                }
                // For each derived resource (views)
                else
                {
                    // Avoid internal references for passes!
                    // If the edge from pass k to pass k+1 was not added yet.
                    // Create edge: Parent-->Source
                    SFrameGraphResource const &parentResource = *mResourceData.get<SFrameGraphResource>(r.parentResource);
                    PassUID_t           const &passUID        = aPass->passUID();

                    if(parentResource.assignedPassUID != passUID)
                    {
                        bool const passAlreadyRegistered = alreadyRegisteredFn<PassUID_t>(mPassAdjacency[parentResource.assignedPassUID], passUID);
                        if(!passAlreadyRegistered)
                        {
                            mPassAdjacency[parentResource.assignedPassUID].push_back(passUID);
                        }
                    }

                    // Do the same for the resources!
                    bool const resourceAdjacencyAlreadyRegistered = alreadyRegisteredFn<FrameGraphResourceId_t>(mResourceAdjacency[parentResource.resourceId], r.resourceId);
                    if(!resourceAdjacencyAlreadyRegistered)
                    {
                        mResourceAdjacency[parentResource.resourceId].push_back(r.resourceId);
                    }

                    // And map the resources to it's pass appropriately
                    bool const resourceEdgeAlreadyRegistered = alreadyRegisteredFn<FrameGraphResourceId_t>(mPassToResourceAdjacency[passUID], r.resourceId);
                    if(!resourceEdgeAlreadyRegistered)
                    {
                        mPassToResourceAdjacency[passUID].push_back(r.resourceId);
                    }

                    if(r.type == EFrameGraphResourceType::TextureView)
                    {
                        SFrameGraphTexture     &texture     = *mResourceData.getMutable<SFrameGraphTexture>    (r.subjacentResource);
                        SFrameGraphTextureView &textureView = *mResourceData.getMutable<SFrameGraphTextureView>(r.resourceId);


                        // Auto adjust format if requested
                        if(textureView.format == FrameGraphFormat_t::Automatic)
                        {
                            textureView.format = texture.format;
                        }

                        // Flag required usage flags, so that the subjacent texture is properly created.
                        if(textureView.mode.check(EFrameGraphViewAccessMode::Read))
                        {
                            texture.requestedUsage.set(EFrameGraphResourceUsage::ImageResource);
                        }
                        else
                        {
                            texture.requestedUsage.set(EFrameGraphResourceUsage::RenderTarget);
                        }
                    }
                    else if(r.type == EFrameGraphResourceType::BufferView)
                    {
                        // TODO
                    }
                    else if(r.type == EFrameGraphResourceType::RenderableListView)
                    {
                        // TODO
                    }
                }
            }

            // Now that the internal resource references were adjusted and duplicates removed, confirm the index in the graph builder
            for(FrameGraphResourceId_t const&id : accessor->resourceReferences())
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

            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CGraphBuilder::validate(std::stack<PassUID_t> const &aPassOrder)
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

            return (allBindingsValid);
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
        bool
        CGraphBuilder::validateTextureUsage(SFrameGraphTexture const &aTexture)
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
