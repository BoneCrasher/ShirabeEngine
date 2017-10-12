#ifndef __SHIRABE_RENDERTARGET_H__
#define __SHIRABE_RENDERTARGET_H__

#include "Resources/System/Core/EResourceType.h"
#include "Resources/System/Core/Handle.h"
#include "Resources/System/Core/IResource.h"
#include "Resources/System/Core/IResourceDescriptor.h"

#include "GFXAPI/Definitions.h"
#include "Resources/Subsystems/GFXAPI/GFXAPI.h"

#include "Resources/Types/TextureND.h"

namespace Engine {
	namespace Resources {	
		using namespace GFXAPI;

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

		/**********************************************************************************************//**
		 * \struct	ResourceDescriptor<EResourceType::GAPI_COMPONENT,EResourceSubType::SWAP_CHAIN>
		 *
		 * \brief	Make the SwapChainDescriptorImpl accessible with the resource descriptor 
		 * 			wrappers.
		 **************************************************************************************************/
		template <>
		struct ResourceDescriptor<EResourceType::GAPI_COMPONENT, EResourceSubType::SWAP_CHAIN>
			: public SwapChainDescriptorImpl
		{
			typedef SwapChainDescriptorImpl type;
		};
		typedef ResourceDescriptor<EResourceType::GAPI_COMPONENT, EResourceSubType::SWAP_CHAIN> SwapChainDescriptor;
		
		/**********************************************************************************************//**
		 * \class	SwapChainDescriptorAdapterBase
		 *
		 * \brief	Descriptor 
		 **************************************************************************************************/
		class SwapChainDescriptorAdapterBase 
			: public ResourceDescriptorAdapterBase<EResourceType::GAPI_COMPONENT, EResourceSubType::SWAP_CHAIN>
		{		
		public:
			inline SwapChainDescriptorAdapterBase(
				const descriptor_type& descriptor
			) : ResourceDescriptorAdapterBase<EResourceType::GAPI_COMPONENT, EResourceSubType::SWAP_CHAIN>(descriptor)
			{}
		};

		/**********************************************************************************************//**
		 * \struct	SwapChainResourceBinding
		 *
		 * \brief	A swap chain resource binding.
		 **************************************************************************************************/
		struct SwapChainResourceBinding {
			ResourceHandle                           swapChainHandle;
			std::vector<RenderTargetResourceBinding> backBufferRenderTargetBindings;
		};

		/**********************************************************************************************//**
		 * \class	GAPISwapChain
		 *
		 * \brief	A gapi swap chain.
		 **************************************************************************************************/
		class SwapChain
			: public SwapChainDescriptorAdapterBase
		{
		public:
			using my_type = SwapChain;

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
				const descriptor_type          &desc,
				const SwapChainResourceBinding &binding)
				: SwapChainDescriptorAdapterBase(desc)
				, _currentBackBufferIndex(0)
				, _resourceBinding(binding)
			{}

			std::size_t              _currentBackBufferIndex;
			SwapChainResourceBinding _resourceBinding;
		};

		DeclareSharedPointerType(SwapChain);
	}
}

#endif