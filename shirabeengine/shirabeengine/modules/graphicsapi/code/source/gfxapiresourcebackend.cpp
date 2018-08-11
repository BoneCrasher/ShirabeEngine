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
            if(!mResourceThread.initialize()) {
                throw EngineException(EEngineStatus::Error, "Cannot initialize resource backend thread.");
            }

            if(!mResourceThread.run()) {
                throw EngineException(EEngineStatus::Error, "Cannot run resource backend thread.");
            }

            return true;
        }

        bool
        CGFXAPIResourceBackend
        ::deinitialize()
        {
            if(mResourceThread.running()) {
                mResourceThread.abortAndJoin();
            }

            return mResourceThread.deinitialize();
        }

        EEngineStatus
        CGFXAPIResourceBackend::registerResource(
                PublicResourceId_t const&id,
                CStdSharedPtr_t<void>                resource)
        {
            if(m_storage.find(id) != m_storage.end())
                return EEngineStatus::Error;

            m_storage[id] = resource;
            return EEngineStatus::Ok;
        }

        /**********************************************************************************************//**
     * \fn	template <typename TResource> EEngineStatus GFXAPIResourceBackend::enqueue( const ResourceTaskFn_t &inTask, std::future<GFXAPIResourceHandle_t> &outSharedFuture)
     *
     * \brief	Adds an object onto the end of this queue
     *
     * \tparam	TResource	Type of the resource.
     * \param 		  	inTask		   	The in task.
     * \param [in,out]	outSharedFuture	The out shared future.
     *
     * \return	The EEngineStatus.
     **************************************************************************************************/
        EEngineStatus
        CGFXAPIResourceBackend
        ::enqueue(
                ResourceTaskFn_t                             &inTask,
                std::future<ResourceTaskFn_t::result_type>   &outSharedFuture)
        {
            using namespace Threading;

            std::future<ResourceTaskFn_t::result_type>   looperTaskFuture;
            ILooper<ResourceTaskFn_t::result_type>::Task looperTask;
            looperTask.setPriority(Priority::Normal);

            looperTaskFuture = looperTask.bind(inTask);
            outSharedFuture  = std::move(looperTaskFuture);

            EEngineStatus status = EEngineStatus::Ok;

            bool enqueued = mResourceThreadHandler.post(std::move(looperTask));
            return (enqueued ? EEngineStatus::Ok : EEngineStatus::GFXAPI_SubsystemThreadEnqueueFailed);
        }

        void
        CGFXAPIResourceBackend
        ::setResourceTaskBackend(ResourceTaskBackendPtr const& backend)
        {
            assert(backend != nullptr);

            m_resourceTaskBackend = backend;
        }
    }
}
