#include "graphicsapi/resources/gfxapiresourcebackend.h"

namespace engine
{
    namespace gfxapi
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CGFXAPIResourceBackend
        ::CGFXAPIResourceBackend()
            : mResourceThread()
            , mResourceThreadHandler(mResourceThread.getHandler())
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CGFXAPIResourceBackend
        ::initialize()
        {
            // Do not catch those as it will prevent all iteration.
            bool const initialized = mResourceThread.initialize();
            if(!initialized)
            {
                throw CEngineException(EEngineStatus::Error, "Cannot initialize resource backend thread.");
            }

            bool const ran = mResourceThread.run();
            if(!ran)
            {
                throw CEngineException(EEngineStatus::Error, "Cannot run resource backend thread.");
            }

            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CGFXAPIResourceBackend::deinitialize()
        {
            if(mResourceThread.running())
            {
                mResourceThread.abortAndJoin();
            }

            bool const deinitialized = mResourceThread.deinitialize();
            return deinitialized;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CGFXAPIResourceBackend::registerResource(
                PublicResourceId_t    const &aId,
                CStdSharedPtr_t<void> const &aResource)
        {
            if(mStorage.find(aId) != mStorage.end())
                return EEngineStatus::Error;

            mStorage[aId] = aResource;
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CGFXAPIResourceBackend::enqueue(
                ResourceTaskFn_t                           &aTask,
                std::future<ResourceTaskFn_t::result_type> &aOutSharedFuture)
        {
            using namespace threading;

            std::future<ResourceTaskFn_t::result_type>    looperTaskFuture{};
            ILooper<ResourceTaskFn_t::result_type>::CTask looperTask{};
            looperTask.setPriority(ETaskPriority::Normal);

            looperTaskFuture = looperTask.bind(aTask);
            aOutSharedFuture  = std::move(looperTaskFuture);

            bool          const enqueued = mResourceThreadHandler.post(std::move(looperTask));
            EEngineStatus const status   = (enqueued ? EEngineStatus::Ok : EEngineStatus::GFXAPI_SubsystemThreadEnqueueFailed);
            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        void CGFXAPIResourceBackend::setResourceTaskBackend(CStdSharedPtr_t<ResourceTaskBackend_t> const &aBackend)
        {
            assert(aBackend != nullptr);

            mResourceTaskBackend = aBackend;
        }
        //<-----------------------------------------------------------------------------
    }
}
