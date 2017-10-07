#ifndef __SHIRABE_RENDERTARGET_H__
#define __SHIRABE_RENDERTARGET_H__

#include "Resources/System/Core/EResourceType.h"
#include "Resources/System/Core/IResource.h"
#include "Resources/System/Core/Handle.h"

#include "GFXAPI/Definitions.h"
#include "Resources/System/GFXAPI/GFXAPI.h"

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

		template <EResourceType type, EResourceSubType subtype>
		class ResourceDescriptorAdapterBase {
		public:
			typedef
				typename ResourceDescriptor<type, subtype>::type
				descriptor_type;

			inline ResourceDescriptorAdapterBase(
				const descriptor_type& descriptor
			) : _descriptor(descriptor)
			{}

			inline const descriptor_type&
				descriptor() const { return _descriptor; }


		private:
			descriptor_type _descriptor;
		};

		/**********************************************************************************************//**
		 * \class	SwapChainDescriptorAdapterBase
		 *
		 * \brief	Descriptor 
		 **************************************************************************************************/
		class SwapChainDescriptorAdapterBase 
			: public ResourceDescriptorAdapterBase<EResourceType::GAPI_COMPONENT, EResourceSubType::SWAP_CHAIN>
		{		
		public:
			

		};

		/**********************************************************************************************//**
		 * \class	GAPISwapChain
		 *
		 * \brief	A gapi swap chain.
		 **************************************************************************************************/
		class GAPISwapChain
			: public SwapChainDescriptorAdapterBase
			, public GFXAPIResourceAdapter
		{
		public:
			using my_type = GAPISwapChain;

			GAPISwapChain(
				const descriptor_type          &descriptor,
				const GFXAPIResourceHandle_t &platformResourceHandle)
				: SwapChainDescriptorAdapterBase(descriptor)
				, GFXAPIResourceAdapter(platformResourceHandle)
				, _currentBackBufferIndex(0)
				, _backBufferRenderTargets()
			{}


			//
			// GAPISwapChain<TGAPIResource> implementation
			// 
			EEngineStatus bind(...);

			EEngineStatus present(bool verticallySynchronized = true);

		private:
			std::size_t        _currentBackBufferIndex;
			ResourceHandleList _backBufferRenderTargets;
		};

		DeclareSharedPointerType(GAPISwapChain);
	}
}

#endif