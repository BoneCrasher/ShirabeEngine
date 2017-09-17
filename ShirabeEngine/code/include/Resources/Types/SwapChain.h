#ifndef __SHIRABE_RENDERTARGET_H__
#define __SHIRABE_RENDERTARGET_H__

#include "Resources/System/Core/EResourceType.h"
#include "Resources/System/Core/IResource.h"
#include "Resources/System/Core/Handle.h"

#include "Resources/System/GFXAPI/Definitions.h"
#include "Resources/System/GFXAPI/GFXAPI.h"

#include "Resources/Types/TextureND.h"

namespace Engine {
	namespace Resources {	
		using namespace GFXAPI;

		struct SwapChainDescriptorImpl {
			std::string              _name;
			TextureDescriptorImpl<2> _texture;
			bool                     _vsyncEnabled;
			bool                     _fullscreen;
			unsigned int             _windowHandle;
			unsigned int             _backBufferCount;
			unsigned int             _refreshRateNumerator;
			unsigned int             _refreshRateDenominator;

			std::string toString() const {
				std::stringstream ss;

				ss
					<< "SwapChainDescriptor ('" << _name << "'): ";

				return ss.str();
			}
		};

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
		 * \brief	A swap chain descriptor adapter base.
		 **************************************************************************************************/
		class SwapChainDescriptorAdapterBase {		
		public:
			typedef
				typename ResourceDescriptor<EResourceType::GAPI_COMPONENT, EResourceSubType::SWAP_CHAIN>::type
				descriptor_type;

			inline SwapChainDescriptorAdapterBase(
				const descriptor_type& descriptor
			) : _descriptor(descriptor)
			{}

			inline const descriptor_type&
				descriptor() const { return _descriptor; }


		private:
			descriptor_type _descriptor;
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