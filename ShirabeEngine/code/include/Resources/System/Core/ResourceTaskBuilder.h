#ifndef __SHIRABE_RESOURCE_TASK_H__
#define __SHIRABE_RESOURCE_TASK_H__

#include <type_traits>

#include "Core/EngineTypeHelper.h"

#include "Resources/System/Core/ResourceDomainTransfer.h"
#include "Resources/System/Core/ResourceTask.h"
#include "Resources/Subsystems/GFXAPI/GFXAPIResourceBackend.h"

namespace Engine {
	namespace Resources {
    using namespace Engine::GFXAPI;

		enum class EResourceTaskType {
			Creation    = 1,
			Update      = 2,
			Destruction = 4,
			Query       = 8 // Needed?
		};

		class Task {
			
		};

    // This additional hierarchy creation structure is created in order to
    // ensure consistent access using SFINAE and a replaceable implementation...

		template <typename GfxApiType>
		class ITaskBuilderImplementationBase 
      : public IGFXAPIResourceTaskBackend<GfxApiType>
    { };		

		template <template <typename> typename TBuilderImplementation, typename GfxApiType>
		using is_task_builder_implementation_t
			= typename std::enable_if<
				std::is_base_of<
					ITaskBuilderImplementationBase<GfxApiType>, 
					TBuilderImplementation<GfxApiType>
				>::value, 
				TBuilderImplementation<GfxApiType>
			>::type;

		/**********************************************************************************************//**
		 * \class	GfxApiTaskBuilder
		 *
		 * \brief	Task builder composition of type list of size N > 1
		 *
		 * \tparam	GfxApiFirstType	Type of the graphics API first type.
		 * \tparam	GfxApiMoreTypes	Type of the graphics API more types.
		 **************************************************************************************************/
		template <template <typename> typename TBuilderImplementation, typename GfxApiFirstType, typename... GfxApiMoreTypes>
		class GfxApiTaskBuilder
			: public is_task_builder_implementation_t<TBuilderImplementation, GfxApiFirstType>
			, public GfxApiTaskBuilder<TBuilderImplementation, GfxApiMoreTypes...>
		{};

		/**********************************************************************************************//**
		 * \class	GfxApiTaskBuilder<GfxApiType>
		 *
		 * \brief	Task builder composition of type list of size N == 1
		 *
		 * \tparam	GfxApiType	Type of the graphics API type.
		 **************************************************************************************************/
		template <template <typename> typename TBuilderImplementation, typename GfxApiType>
		class GfxApiTaskBuilder<TBuilderImplementation, GfxApiType>
			: public is_task_builder_implementation_t<TBuilderImplementation, GfxApiType>
		{};
		
		template <template <typename> typename TBuilderImplementation, typename... GfxApiTypes>
		class GenericTaskBuilder
			: public GfxApiTaskBuilder<TBuilderImplementation, GfxApiTypes...>
		{
		public:
			template <typename GfxApiType> 
			EEngineStatus creationTask(
        const typename GfxApiType::CreationRequest &desc,
        ResourceTaskFn_t                           &task) 
      {
        return (*this).TBuilderImplementation<GfxApiType>::creationTask(desc, task);
			}

			template <typename GfxApiType>
      EEngineStatus updateTask(
        const typename GfxApiType::UpdateRequest &desc,
        ResourceTaskFn_t                         &task) 
      {
        return (*this).TBuilderImplementation<GfxApiType>::updateTask(desc, task);
			}

			template <typename GfxApiType>
			Ptr<Task> destructionTask(
        const typename GfxApiType::DestructionRequest &desc,
        ResourceTaskFn_t                              &task) 
      {
        return (*this).TBuilderImplementation<GfxApiType>::destructionTask(desc, task);
			}

			template <typename GfxApiType>
			Ptr<Task> queryTask(
        const typename GfxApiType::Query &desc,
        ResourceTaskFn_t                 &task) 
      {
        return (*this).TBuilderImplementation<GfxApiType>::queryTask(desc, task);
			}
		};
	}
}

#endif