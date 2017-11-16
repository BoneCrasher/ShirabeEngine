#ifndef __SHIRABE_RESOURCE_TASK_H__
#define __SHIRABE_RESOURCE_TASK_H__

#include <type_traits>

#include "Core/EngineTypeHelper.h"

#include "Resources/System/Core/ResourceDomainTransfer.h"
#include "Resources/System/Core/ResourceBuilder.h"

namespace Engine {
	namespace Resources {

		enum class EResourceTaskType {
			Creation    = 1,
			Update      = 2,
			Destruction = 4,
			Query       = 8 // Needed?
		};

		class Task {
			
		};

		template <typename GfxApiType>
		class ITaskBuilderImplementationBase {
		protected: // Make sure it's not publicly visible
			virtual Ptr<Task> build(const ResourceDescriptor<GfxApiType>& descriptor, const EResourceTaskType& taskType) = 0;
		};		

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
			template <typename GfxApiType>
			Ptr<Task> builderResourceTaskGeneric(const ResourceDescriptor<GfxApiType>& desc, const EResourceTaskType& taskType) {
				return (*this).TBuilderImplementation<GfxApiType>::build(desc, taskType);
			}

		public:
			template <typename GfxApiType> 
			Ptr<Task> builderResourceCreationTask(const ResourceDescriptor<GfxApiType>& desc) {
				return builderResourceTaskGeneric(desc, EResourceTaskType::Creation);
			}

			template <typename GfxApiType>
			Ptr<Task> builderResourceUpdateTask(const ResourceDescriptor<GfxApiType>& desc) {
				return builderResourceTaskGeneric(desc, EResourceTaskType::Creation);
			}

			template <typename GfxApiType>
			Ptr<Task> builderResourceDestructionTask(const ResourceDescriptor<GfxApiType>& desc) {
				return builderResourceTaskGeneric(desc, EResourceTaskType::Creation);
			}

			template <typename GfxApiType>
			Ptr<Task> builderResourceQueryTask(const ResourceDescriptor<GfxApiType>& desc) {
				return builderResourceTaskGeneric(desc, EResourceTaskType::Creation);
			}
		};
	}
}

#endif