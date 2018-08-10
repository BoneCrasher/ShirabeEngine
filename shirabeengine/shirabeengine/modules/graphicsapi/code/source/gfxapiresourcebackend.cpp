

#include "GraphicsAPI/Resources/GFXAPIResourceBackend.h"

namespace engine {
  namespace GFXAPI {


    /**********************************************************************************************//**
     * \fn  template <typename... TSupportedResourceTypes> GFXAPIResourceBackend<TSupportedResourceTypes...> ::GFXAPIResourceBackend() : m_resourceThread() , m_resourceThreadHandler(m_resourceThread.getHandler())
     *
     * \brief Gfxapi resource backend
     *
     * \tparam  TSupportedResourceTypes Type of the supported resource types.
     *
     * \return  A GFXAPIResourceBackend&lt;TSupportedResourceTypes...&gt;
     **************************************************************************************************/
    GFXAPIResourceBackend
      ::GFXAPIResourceBackend()
      : m_resourceThread()
      , m_resourceThreadHandler(m_resourceThread.getHandler())
    {}


    bool
      GFXAPIResourceBackend
      ::initialize()
    {
      // Do not catch those as it will prevent all iteration.
      if(!m_resourceThread.initialize()) {
        throw EngineException(EEngineStatus::Error, "Cannot initialize resource backend thread.");
      }

      if(!m_resourceThread.run()) {
        throw EngineException(EEngineStatus::Error, "Cannot run resource backend thread.");
      }

      return true;
    }

    bool
      GFXAPIResourceBackend
      ::deinitialize()
    {
      if(m_resourceThread.running()) {
        m_resourceThread.abortAndJoin();
      }

      return m_resourceThread.deinitialize();
    }

    EEngineStatus 
      GFXAPIResourceBackend::registerResource(
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
      GFXAPIResourceBackend
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

      bool enqueued = m_resourceThreadHandler.post(std::move(looperTask));
      return (enqueued ? EEngineStatus::Ok : EEngineStatus::GFXAPI_SubsystemThreadEnqueueFailed);
    }

    void
      GFXAPIResourceBackend
      ::setResourceTaskBackend(ResourceTaskBackendPtr const& backend)
    {
      assert(backend != nullptr);

      m_resourceTaskBackend = backend;
    }
  }
}
