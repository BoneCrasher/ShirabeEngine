#ifndef __SHIRABE_FRAMEGRAPH_PASS_H__
#define __SHIRABE_FRAMEGRAPH_PASS_H__

#include <assert.h>
#include <string>
#include <functional>
#include <cstdint>

#include <core/enginetypehelper.h>
#include <core/passkey.h>
#include <os/applicationenvironment.h>

#include "renderer/framegraph/framegraphdata.h"
#include "renderer/framegraph/framegraphcontexts.h"
#include "renderer/framegraph/framegraphserialization.h"
#include "renderer/framegraph/passbuilder.h"

namespace engine
{
    namespace framegraph
    {
        using namespace rendering;

#if defined SHIRABE_FRAMEGRAPH_ENABLE_SERIALIZATION
        using namespace serialization;
#endif

        class CGraphBuilder;
        class CGraph;

        /**
         * Pass base implementation providing UID, name and resource references.
         *
         * Any deriving pass has to define setup and execute, as these are pure virtual
         * in this base implementation.
         * As such, this pass implementation can not be instantiate, set-up or executed.
         */
        class CPassBase
        #if defined SHIRABE_FRAMEGRAPH_ENABLE_SERIALIZATION
                : public ISerializable<IFrameGraphSerializer, IFrameGraphDeserializer>
        #endif
        {
            SHIRABE_DECLARE_LOG_TAG(CPassBase);

        public_constructors:
            /**
             * Construct a base pass from UID and name.
             *
             * @param aPassUID  The pass UID assigned to this pass.
             * @param aPassName The pass name assigned to this pass.
             */
            CPassBase(
                    PassUID_t   const &aPassUID,
                    std::string const &aPassName);

        public_methods:

            /**
             * Return the UID assigned to this pass.
             *
             * @return See brief.
             */
            PassUID_t const &passUID() const;

            /**
             * Return the name assigned to this pass.
             *
             * @return See brief.
             */
            std::string const &passName() const;


#if defined SHIRABE_FRAMEGRAPH_ENABLE_SERIALIZATION
            /**
             * Double-Dispatch serialization integration to serialize this pass.
             *
             * @param aSerializer Serializer to accept for pass serialization.
             */
            virtual bool acceptSerializer(IFrameGraphSerializer &aSerializer) const;

            /**
             * Double-Dispatch serialization integration to deserialize this pass.
             *
             * @param aDeserializer Deserializer to accept for pass serialization.
             */
            virtual bool acceptDeserializer(IFrameGraphDeserializer &aDeserializer);
#endif

            FrameGraphResourceIdList const &resourceReferences() const { return mResourceReferences; }
            FrameGraphResourceIdList       &mutableResourceReferences() { return mResourceReferences; }

            /**
             * Register a resource in this pass instance.
             *
             * @param aResourceUID The resource uid of the resource to register.
             * @return             True, if successful. False otherwise.
             */
            CEngineResult<> registerResource(FrameGraphResourceId_t const &aResourceUID);

            /**
             * Interface method for all passes' setup.
             * To be implemented by specific pass classes.
             *
             * @param aPassBuilder The pass builder instance to use for setup.
             * @return             True, if successful. False otherwise.
             */
            virtual CEngineResult<> staticSetup(CPassStaticBuilder &aPassBuilder) = 0;

            virtual CEngineResult<> dynamicSetup(SFrameGraphDataSource const &aDataSource, CPassDynamicBuilder &aPassBuilder) = 0;

            /**
             * Interface method for all passes' execution.
             * To be implemented by specific pass classes.
             *
             * @param aFrameGraphResources A collection of resolved and loaded resources requested during
             *                             setup for use during exeuction.
             * @param aContext             The render context of the framegraph interfacing with all subsystems.
             * @return                     True, if successful. False otherwise.
             */
            virtual CEngineResult<> execute(
                    SFrameGraphDataSource    const &aDataSource,
                    CFrameGraphResources     const &aFrameGraphResources,
                    SFrameGraphRenderContextState  &aContextState,
                    SFrameGraphResourceContext     &aResourceContext,
                    SFrameGraphRenderContext       &aRenderContext) = 0;

        private_members:
            PassUID_t                mPassUID;
            std::string              mPassName;
            FrameGraphResourceIdList mResourceReferences;
        };

        SHIRABE_DECLARE_LIST_OF_TYPE(Shared<CPassBase>, CPassBase);
        SHIRABE_DECLARE_MAP_OF_TYPES(PassUID_t, Shared<CPassBase>, Pass);

        /**
         * The CallbackPass is a pass implementation, which accepts callbacks for setup and execute,
         * permitting a very elegant way to implement passes without thousands of separate classes.
         * Each CallbackPass operates on a 'TPassData' instance, i.e. the input, state and output data
         * of the pass, being externally managed and provided to the callback pass on execution.
         *
         * @tparam TPassData The data struct of the pass to externally manage and provide on execution.
         */
        template <typename TStaticPassData, typename TDynamicPassData>
        class CallbackPass
                : public CPassBase
        {
            SHIRABE_DECLARE_LOG_TAG(CallbackPass);

        public_typedefs:
            using StaticSetupCallback_t  = std::function<CEngineResult<>(CPassStaticBuilder&, TStaticPassData&)>;
            using DynamicSetupCallback_t = std::function<CEngineResult<>(CPassDynamicBuilder&, SFrameGraphDataSource const &, TDynamicPassData&)>;
            using ExecCallback_t  = std::function<CEngineResult<>(TStaticPassData const&
                                                                , TDynamicPassData const &
                                                                , SFrameGraphDataSource const &
                                                                , CFrameGraphResources const &
                                                                , SFrameGraphRenderContextState &
                                                                , SFrameGraphResourceContext &
                                                                , SFrameGraphRenderContext &)>;

        public_constructors:
            /**
             * Construct a callback pass from a UID, name, setup callback and execute callback.
             *
             * @param aPassUID  The UID assigned to this pass.
             * @param aPassName The name assigned to this pass.
             * @param aSetupCb  The setup callback for this pass, immediately invoked on construction.
             * @param aExecCb   The execution callback invoked deferred.
             */
            CallbackPass(
                    PassUID_t       const   &aPassUID,
                    std::string     const   &aPassName,
                    StaticSetupCallback_t  &&aStaticSetupCb,
                    DynamicSetupCallback_t &&aDynamicSetupCb,
                    ExecCallback_t         &&aExecCb);

            /**
             * Setup implementation, invoking the setup callback.
             *
             * @param aBuilder The pass builder to use for setup.
             * @return         True, if successful. False otherwise.
             */
            CEngineResult<> staticSetup(CPassStaticBuilder &aBuilder);

            CEngineResult<> dynamicSetup(SFrameGraphDataSource const &aDataSource, CPassDynamicBuilder &aBuilder);

            /**
             * Execute implementation, invoking the execute callback.
             *
             * @param aFrameGraphResources A collection of resolved and loaded resources requested during
             *                             setup for use during exeuction.
             * @param aContext             The render context of the framegraph interfacing with all subsystems.
             * @return                     True, if successful. False otherwise.
             */
            CEngineResult<> execute(
                    SFrameGraphDataSource    const &aDataSource,
                    CFrameGraphResources     const &aFrameGraphResources,
                    SFrameGraphRenderContextState  &aContextState,
                    SFrameGraphResourceContext     &aResourceContext,
                    SFrameGraphRenderContext       &aRenderContext);

            /**
             * Return the pass data struct associated with this callback pass.
             *
             * @return See brief.
             */
            TStaticPassData const &staticPassData() const
            {
                return mStaticPassData;
            }

            /**
             * Return the pass data struct associated with this callback pass.
             *
             * @return See brief.
             */
            TStaticPassData const &dynamicPassData() const
            {
                return mDynamicPassData;
            }

        private_members:
            StaticSetupCallback_t    mStaticSetupCallback;
            DynamicSetupCallback_t   mDynamicSetupCallback;
            ExecCallback_t           mExecCallback;
            FrameGraphResourceIdList mResources;
            TStaticPassData          mStaticPassData;
            TDynamicPassData         mDynamicPassData;
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TStaticPassData, typename TDynamicPassData>
        CallbackPass<TStaticPassData, TDynamicPassData>::CallbackPass(
                PassUID_t       const   &passUID,
                std::string     const   &passName,
                StaticSetupCallback_t  &&staticSetupCb,
                DynamicSetupCallback_t &&dynamicSetupCb,
                ExecCallback_t         &&execCb)
            : CPassBase(passUID, passName)
            , mStaticSetupCallback(staticSetupCb)
            , mDynamicSetupCallback(dynamicSetupCb)
            , mExecCallback(execCb)
            , mStaticPassData()
            , mDynamicPassData()
        {
            assert(nullptr != mStaticSetupCallback);
            assert(nullptr != mDynamicSetupCallback);
            assert(nullptr != mExecCallback );
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TStaticPassData, typename TDynamicPassData>
        CEngineResult<> CallbackPass<TStaticPassData, TDynamicPassData>::staticSetup(CPassStaticBuilder &aPassBuilder)
        {
            TStaticPassData passData{ };

            CEngineResult<> setup = mStaticSetupCallback(aPassBuilder, passData);
            if(setup.successful())
            {
                mStaticPassData = passData;
            }

            return setup;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TStaticPassData, typename TDynamicPassData>
        CEngineResult<> CallbackPass<TStaticPassData, TDynamicPassData>::dynamicSetup(SFrameGraphDataSource const &aDataSource, CPassDynamicBuilder &aPassBuilder)
        {
            TDynamicPassData passData{ };

            CEngineResult<> setup = mDynamicSetupCallback(aPassBuilder, aDataSource, passData);
            if(setup.successful())
            {
                mDynamicPassData = passData;
            }

            return setup;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TStaticPassData, typename TDynamicPassData>
        CEngineResult<> CallbackPass<TStaticPassData, TDynamicPassData>::execute(
                SFrameGraphDataSource    const &aDataSource,
                CFrameGraphResources     const &aFrameGraphResources,
                SFrameGraphRenderContextState  &aContextState,
                SFrameGraphResourceContext     &aResourceContext,
                SFrameGraphRenderContext       &aRenderContext)
        {
            try
            {
                CEngineResult<> execution = mExecCallback(mStaticPassData, mDynamicPassData, aDataSource, aFrameGraphResources, aContextState, aResourceContext, aRenderContext);
                return execution;
            }
            catch(std::runtime_error const &e)
            {
                CLog::Error(logTag(), e.what());
                return { EEngineStatus::Error };
            }
            catch(...)
            {
                CLog::Error(logTag(), "Unknown error while executing pass.");
                return { EEngineStatus::Error };
            }
        }
        //<-----------------------------------------------------------------------------
    }
}

#endif
