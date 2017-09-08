#ifndef __SHIRABE_ENGINESTATUS_H__
#define __SHIRABE_ENGINESTATUS_H__

#include <ostream>

#include "Core/EngineTypeHelper.h"

namespace Engine {
	enum class EEngineStatus
		: int {
		Ok                                                              =    0,
		Error                                                           =   -1,   // Non-specific issues
		NullPointer                                                     =  -50,  // General parameter issues
		OutOfBounds                                                     =  -51,
		CollectionOperationException                                    =  -70,  // Collection issues
		CollectionInsertException                                       =  -71,
		CollectionEraseException                                        =  -72,
		ObjectAlreadyAddedToCollection                                  =  -80,
		ObjectNotAddedToCollection                                      =  -81,
		FileNotFound                                                    = -100, // Filesystem and handles
		EngineComponentInitializationError                              = -200, // Engine component issues
		EngineComponentUpdateError                                      = -201,
		EngineComponentDeinitializationError                            = -202,
		WindowCreationError                                             = -250, // Window issues
		WindowMessageHandlerError                                       = -251,
		WindowEventError                                                = -252,
		GAPI_NoPrimaryAdapter                                           = -300,
		GAPI_NoPrimaryAdapterOutputs                                    = -301,
		GAPI_NoPrimaryAdapterOutputModes                                = -302,
		DXGI_FactoryCreationFailed                                      = -100300, // DXGI
		DXGI_FactoryGetAdapterFailed                                    = -100302, 
		DXGI_AdapterGetDescFailed                                       = -100303,
		DXGI_PrimaryAdapterGetOutputFailed                              = -100304,
		DXGI_PrimaryAdapterGetOutputModesFailed                         = -100305,
		DXGI_DeviceAndSwapChainCreationFailed                           = -100350,
		DXSwapChain_AttachSwapChainToBackBuffer_GetBackBufferPtr_Failed = -100365,
		DXDevice_AttachSwapChainToBackBuffer_CreateRTV_Failed           = -100370,
		DXDevice_DepthStencilTextureCreationFailed                      = -100371,
		DXDevice_DepthStencilStateCreationFailed                        = -100372,
		DXDevice_RasterizerStateCreationFailed                          = -100380,
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