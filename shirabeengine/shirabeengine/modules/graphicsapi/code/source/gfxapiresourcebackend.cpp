#include "graphicsapi/resources/gfxapiresourcebackend.h"

namespace engine
{
    namespace gfxapi
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CGFXAPIResourceBackend::CGFXAPIResourceBackend()
            : mResourceThread()
            , mResourceThreadHandler(mResourceThread.getHandler())
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGFXAPIResourceBackend::initialize()
        {
            // Do not catch those as it will prevent all iteration.
            bool const initialized = mResourceThread.initialize();
            if(not initialized)
            {
                EngineStatusPrintOnError(EEngineStatus::Error, logTag(), "Cannot initialize resource backend thread.");
                return { EEngineStatus::Error };
            }

            bool const ran = mResourceThread.run();
            if(not ran)
            {
                EngineStatusPrintOnError(EEngineStatus::Error, logTag(), "Cannot run resource backend thread.");
                return { EEngineStatus::Error };
            }

            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGFXAPIResourceBackend::deinitialize()
        {
            if(mResourceThread.running())
            {
                mResourceThread.abortAndJoin();
            }

            bool const deinitialized = mResourceThread.deinitialize();
            return ( deinitialized ? EEngineStatus::Ok : EEngineStatus::Error );
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGFXAPIResourceBackend::registerResource(
                PublicResourceId_t    const &aId,
                CStdSharedPtr_t<void> const &aResource,
                EImportStorageMode    const &aImportStorageMode)
        {
            bool const alreadyRegistered = (mStorage.end() != mStorage.find(aId));
            if(alreadyRegistered && (EImportStorageMode::NoOverwrite == aImportStorageMode))
            {
                return { EEngineStatus::Error };
            }

            mStorage[aId] = aResource;
            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGFXAPIResourceBackend::enqueue(
                ResourceTaskFn_t                           &aTask,
                std::future<ResourceTaskFn_t::result_type> &aOutSharedFuture)
        {
            using namespace threading;

            std::future<ResourceTaskFn_t::result_type>    looperTaskFuture{};
            ILooper<ResourceTaskFn_t::result_type>::CTask looperTask{};
            looperTask.setPriority(ETaskPriority::Normal);

            looperTaskFuture = looperTask.bind(aTask);
            aOutSharedFuture = std::move(looperTaskFuture);

            bool          const enqueued = mResourceThreadHandler.post(std::move(looperTask));
            EEngineStatus const status   = (enqueued ? EEngineStatus::Ok : EEngineStatus::GFXAPI_SubsystemThreadEnqueueFailed);

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGFXAPIResourceBackend::setResourceTaskBackend(CStdSharedPtr_t<ResourceTaskBackend_t> const &aBackend)
        {
            if(nullptr == aBackend)
            {
                return EEngineStatus::Error;
            }
            else
            {
                mResourceTaskBackend = aBackend;
                return EEngineStatus::Ok;
            }
        }
        //<-----------------------------------------------------------------------------
    }
}
