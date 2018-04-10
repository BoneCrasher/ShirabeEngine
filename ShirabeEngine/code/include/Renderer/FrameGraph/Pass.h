#ifndef __SHIRABE_FRAMEGRAPH_PASS_H__
#define __SHIRABE_FRAMEGRAPH_PASS_H__

#include <string>
#include <functional>

#include "Core/EngineTypeHelper.h"
#include "Platform/ApplicationEnvironment.h"
#include "Resources/Core/ResourceDTO.h"
#include "Renderer/IRenderer.h"

#include "FrameGraphSerialization.h"

#include "PassLinker.h"

namespace Engine {
	namespace FrameGraph {
		using namespace Renderer;
    using namespace Serialization;

		class GraphBuilder;

		class PassBase 
      : public ISerializable<IFrameGraphSerializer, IFrameGraphDeserializer>
    {
		public:
			virtual bool execute(Ptr<IRenderContext>&) = 0;

      inline void acceptSerializer(Ptr<IFrameGraphSerializer>&s)
      {
        s->serializePass(GetNonDeletingSelfPtrType(this));
      }

      inline void acceptDeserializer(Ptr<IFrameGraphDeserializer> const&d)
      {
        d->deserializePass(GetNonDeletingSelfPtrType(this));
      }
		};

		DeclareSharedPointerType(PassBase);
		DeclareListType(Ptr<PassBase>, PassBase);

		template <typename TPassImplementation>
		class Pass
			: public PassBase
		{
		public:
			using PassImplementation_t = TPassImplementation;
			using InputData_t          = typename PassImplementation_t::InputData;
			using OutputData_t         = typename PassImplementation_t::OutputData;

			Pass(
				FrameGraphResourceId_t         const&passUID,
				UniquePtr<TPassImplementation>      &implementation)
				: m_passUID(passUID)
				, m_implementation(std::move(implementation))
				, m_inputData()
				, m_outputData()
			{
				assert(m_implementation != nullptr);
			}

			FrameGraphResourceId_t const&passUID() const { return m_passUID; }

			InputData_t  const&inputData()  const { return m_inputData;  }
			OutputData_t const&outputData() const { return m_outputData; }

			template <typename... TPassCreationArgs>
			bool setup(
        Ptr<ApplicationEnvironment> const&environment,
				GraphBuilder                     &graphBuilder,
				TPassCreationArgs            &&...args) 
			{
				PassLinker<TPassImplementation> passLinker(passUID(), graphBuilder.resourceUIDGenerator());

				InputData_t  inputData ={};
				OutputData_t outputData={};

				bool setupSuccessful = m_implementation->setup(environment, passLinker, inputData, outputData, std::forward<TPassCreationArgs>(args)...);
				if(!setupSuccessful)
					throw std::exception("Failed to setup pass.");
        
				m_inputData  = inputData;
				m_outputData = outputData;

				return true;
			}

			bool execute(
				Ptr<IRenderContext>&context)
			{
				return true; // TPassImplementation::execute(context, input, output);
			}

      std::string to_string() const {
        std::string implementation_string = (m_implementation ? m_implementation->to_string() : "None");
        std::string stringified           = String::format("Pass {\nUID: %1\nImplementation: %2\n}\n", m_passUID, implementation_string);
      }

		private:
			FrameGraphResourceId_t         m_passUID;
			UniquePtr<TPassImplementation> m_implementation;

			InputData_t  m_inputData;
			OutputData_t m_outputData;
		};
    
	}
}

#endif