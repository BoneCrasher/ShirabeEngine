#ifndef __SHIRABE_ENGINESTATUS_H__
#define __SHIRABE_ENGINESTATUS_H__

#include <ostream>

#include "Core/EngineTypeHelper.h"

namespace Engine {
	enum class EEngineStatus
		: int {
		Ok                                                              =       0,
		Error                                                           =      -1,  // Non-specific issues
		NullPointer                                                     =     -50,  // General parameter issues
		OutOfBounds                                                     =     -51,
		CollectionOperationException                                    =     -70,  // Collection issues
		CollectionInsertException                                       =     -71,
		CollectionEraseException                                        =     -72,
		ObjectAlreadyAddedToCollection                                  =     -80,
		ObjectNotAddedToCollection                                      =     -81,
		FileNotFound                                                    =    -100, // Filesystem and handles
		EngineComponentInitializationError                              =    -200, // Engine component issues
		EngineComponentUpdateError                                      =    -201,
		EngineComponentDeinitializationError                            =    -202,
		WindowCreationError                                             =    -250, // Window issues
		WindowMessageHandlerError                                       =    -251,
		WindowEventError                                                =    -252,
		ResourceManager_Generic                                         =   -1000, // ResourceManager
		ResourceManager_RootProxyFetchFailed                            =   -1050,
		ResourceManager_BaseProxyCastFailed                             =   -1051, // ResourceManager::Proxy
		ResourceManager_DependencyLoadFailed							=   -1052,
		ResourceManager_ResourceLoadFailed								=   -1053,
		GAPI_Generic                                                    =  -50000, // GAPI
		GAPI_NoPrimaryAdapter                                           =  -50001,
		GAPI_NoPrimaryAdapterOutputs                                    =  -50002,
		GAPI_NoPrimaryAdapterOutputModes                                =  -50003,
		GAPI_InvalidHandle                                              =  -50020,
		DXGI_FactoryCreationFailed                                      = -100300, // DXGI
		DXGI_FactoryGetAdapterFailed                                    = -100302, 
		DXGI_AdapterGetDescFailed                                       = -100303,
		DXGI_PrimaryAdapterGetOutputFailed                              = -100304,
		DXGI_PrimaryAdapterGetOutputModesFailed                         = -100305,
		DXGI_DeviceAndSwapChainCreationFailed                           = -100350,
		DXGI_SwapChainCreationFailed                                    = -100351,
		DXGI_SwapChainCreationFailed_BackBufferPointerAcquisition       = -100352,
		DXSwapChain_AttachSwapChainToBackBuffer_GetBackBufferPtr_Failed = -100365,
		DXDevice_Texture1DCreationFailed                                = -100370,
		DXDevice_Texture2DCreationFailed                                = -100371,
		DXDevice_Texture3DCreationFailed                                = -100372,
		DXDevice_AttachSwapChainToBackBuffer_CreateRTV_Failed           = -100400,
		DXDevice_SRVCreationFailed                                      = -100430,
		DXDevice_DepthStencilTextureCreationFailed                      = -100501,
		DXDevice_DepthStencilStateCreationFailed                        = -100502,
		DXDevice_RasterizerStateCreationFailed                          = -100550,
		Timer_NoPlatformTimeInstance                                    = -300050,
		Time_Win32__QueryPerformanceFrequencyFailed                     = -300101,
		Time_Win32__QueryPerformanceCounterFailed                       = -300102

	};
	DeclareEnumClassUnderlyingType(EEngineStatus, EngineStatusInternal_t)

	static inline bool CheckEngineError(const EEngineStatus& status) { 
		return (static_cast<EngineStatusInternal_t>(status) < 0); /* All errors are values less than zero. */ 
	}

	static inline std::ostream& operator<<(std::ostream& stream, const EEngineStatus& status) {
		return (stream << ((EngineStatusInternal_t)status));
	}
}

#endif