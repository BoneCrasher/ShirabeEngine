#ifndef __SHIRABE_SWAPCHAIN_H__
#define __SHIRABE_SWAPCHAIN_H__

#include "Resources/System/Core/EResourceType.h"
#include "Resources/System/Core/Handle.h"
#include "Resources/System/Core/IResource.h"
#include "Resources/System/Core/ResourceDomainTransfer.h"
#include "Resources/System/Core/ResourceTraits.h"

#include "GFXAPI/Definitions.h"
#include "GFXAPI/Types/TextureND.h"
#include "GFXAPI/Types/RenderTargetView.h"

#include "Resources/Subsystems/GFXAPI/GFXAPI.h"

namespace Engine {
	namespace GFXAPI {	
		using namespace Resources;
    
    class SwapChain;

		/**********************************************************************************************//**
		 * \struct	SwapChainDescriptorImpl
		 *
		 * \brief	Implementation of the SwapChainDescriptor wrapper.
		 **************************************************************************************************/
		struct SwapChainDescriptorImpl 
			: public DescriptorImplBase<EResourceType::GAPI_COMPONENT, EResourceSubType::SWAP_CHAIN>
		{
			std::string            name;
			TextureNDDescriptor<2> texture;
			bool                   vsyncEnabled;
			bool                   fullscreen;
			unsigned int           windowHandle;
			unsigned int           backBufferCount;
			unsigned int           refreshRateNumerator;
			unsigned int           refreshRateDenominator;

			std::string toString() const {
				std::stringstream ss;

				ss
					<< "SwapChainDescriptor ('" << name << "'): ";

				return ss.str();
			}
		};

		/**********************************************************************************************//**
		 * \struct	SwapChainCreationRequestImpl
		 *
		 * \brief	A swap chain creation request implementation.
		 **************************************************************************************************/
		struct SwapChainCreationRequestImpl {
		public:
      const struct ResourceDescriptor<SwapChain> resourceDescriptor() const; //  const { return ResourceDescriptor<SwapChain>(_resourceDescriptor); }

			std::string toString() const {
				std::stringstream ss;

				ss
					<< "SwapChainCreationRequest: \n"
					<< "[\n"
					<< _resourceDescriptor.toString() << "\n"
					<< "]"
					<< std::endl;

				return ss.str();
			}
		private:
			SwapChainDescriptorImpl _resourceDescriptor;
		};

		struct SwapChainDestructionRequestImpl {

		};

		struct SwapChainQueryRequestImpl {

		};

		struct SwapChainResourceBinding {
			ResourceHandle swapChainHandle;
			std::vector<RenderTargetViewResourceBinding> backBufferRenderTargetBindings;
		};
    
    DeclareResourceTraits(SwapChain,
                          SwapChain,
                          EResourceType::GAPI_COMPONENT,
                          EResourceSubType::SWAP_CHAIN,
                          SwapChainResourceBinding,
                          SwapChainDescriptorImpl,
                          SwapChainCreationRequestImpl,
                          void,
                          SwapChainQueryRequestImpl,
                          SwapChainDestructionRequestImpl);

    DefineTraitsPublicTypes(SwapChain, SwapChainTraits);


		/**********************************************************************************************//**
		 * \class	GAPISwapChain
		 *
		 * \brief	A gapi swap chain.
		 **************************************************************************************************/
		class SwapChain
			: public SwapChainTraits
      , public ResourceDescriptorAdapter<SwapChainTraits>
			, public ResourceBindingAdapter<SwapChainTraits>
		{
		public:
			using my_type = SwapChain;

			//
			// GAPISwapChain<TGAPIResource> implementation
			// 
			// EEngineStatus bind(...);
      // 
			// EEngineStatus present(bool verticallySynchronized = true);

			inline static Ptr<SwapChain> create(
				const SwapChainDescriptor &desc,
				const SwapChainBinding    &binding) {
				return Ptr<SwapChain>(new SwapChain(desc, binding));
			}

		private:
			inline SwapChain(
				const SwapChainDescriptor &desc, // Forward declaration issue?!
				const SwapChainBinding    &binding)
				: SwapChainTraits()
        , ResourceDescriptorAdapter<SwapChainTraits>(desc)
				, ResourceBindingAdapter<SwapChainTraits>(binding)
				, _currentBackBufferIndex(0)
			{}

			std::size_t  _currentBackBufferIndex;
		};
		DeclareSharedPointerType(SwapChain);


    const ResourceDescriptor<SwapChain> SwapChainCreationRequestImpl
      ::resourceDescriptor() const 
    { 
      return ResourceDescriptor<SwapChain>(_resourceDescriptor); 
    }
	}
}

#endif