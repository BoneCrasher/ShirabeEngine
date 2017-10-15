#ifndef __SHIRABE_RENDERTARGET_H__
#define __SHIRABE_RENDERTARGET_H__

#include "Resources/System/Core/EResourceType.h"
#include "Resources/System/Core/Handle.h"
#include "Resources/System/Core/IResource.h"
#include "Resources/System/Core/IResourceDescriptor.h"
#include "Resources/System/Core/ResourceBinding.h"

#include "GFXAPI/Definitions.h"
#include "GFXAPI/Types/TextureND.h"
#include "GFXAPI/Types/RenderTargetView.h"

#include "Resources/Subsystems/GFXAPI/GFXAPI.h"

namespace Engine {
	namespace GFXAPI {	
		using namespace Resources;

		/**********************************************************************************************//**
		 * \struct	SwapChainDescriptorImpl
		 *
		 * \brief	Implementation of the SwapChainDescriptor wrapper.
		 **************************************************************************************************/
		struct SwapChainDescriptorImpl {
			std::string              name;
			TextureDescriptorImpl<2> texture;
			bool                     vsyncEnabled;
			bool                     fullscreen;
			unsigned int             windowHandle;
			unsigned int             backBufferCount;
			unsigned int             refreshRateNumerator;
			unsigned int             refreshRateDenominator;

			std::string toString() const {
				std::stringstream ss;

				ss
					<< "SwapChainDescriptor ('" << name << "'): ";

				return ss.str();
			}
		};

		struct SwapChainResourceBinding {
			ResourceHandle swapChainHandle;
			std::vector<RenderTargetViewResourceBinding> backBufferRenderTargetBindings;
		};

		struct SwapChainTraits {
			static const constexpr EResourceType    resource_type    = EResourceType::GAPI_COMPONENT;
			static const constexpr EResourceSubType resource_subtype = EResourceSubType::SWAP_CHAIN;

			using descriptor_impl_type = SwapChainDescriptorImpl;
			using binding_type         = SwapChainResourceBinding;
		};

		/**********************************************************************************************//**
		 * \class	GAPISwapChain
		 *
		 * \brief	A gapi swap chain.
		 **************************************************************************************************/
		class SwapChain
			: public ResourceDescriptorAdapter<SwapChainTraits>
			, public ResourceBindingAdapter<SwapChainTraits>
		{
		public:
			using my_type = SwapChain;

			static const constexpr EResourceType    resource_type    = SwapChainTraits::resource_type;
			static const constexpr EResourceSubType resource_subtype = SwapChainTraits::resource_subtype;
			using descriptor_impl_type = SwapChainTraits::descriptor_impl_type;
			//
			// GAPISwapChain<TGAPIResource> implementation
			// 
			EEngineStatus bind(...);

			EEngineStatus present(bool verticallySynchronized = true);

			inline static Ptr<SwapChain> create(
				const descriptor_type          &desc,
				const SwapChainResourceBinding &binding) {
				return Ptr<SwapChain>(new SwapChain(desc, binding));
			}

		private:
			inline SwapChain(
				const descriptor_type &desc,
				const binding_type    &binding)
				: ResourceDescriptorAdapter<SwapChainTraits>(desc)
				, ResourceBindingAdapter<SwapChainTraits>(binding)
				, _currentBackBufferIndex(0)
			{}

			std::size_t  _currentBackBufferIndex;
		};
		DeclareSharedPointerType(SwapChain);

		typedef ResourceDescriptor<SwapChain> SwapChainDescriptor;

	}
}

#endif