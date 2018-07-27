/*!
 * @file      enginestatus.h
 * @author    Marc-Anton Boehm-von Thenen
 * @date      27/07/2018
 * @copyright SmartRay GmbH (www.smartray.com)
 */

#ifndef __SHIRABE_ENGINESTATUS_H__
#define __SHIRABE_ENGINESTATUS_H__

#include <ostream>

#include "core/enginetypehelper.h"
#include "log/log.h"

namespace Engine
{
    /**
     * @brief The EEngineStatus enum
     */
    enum class EEngineStatus
            : int32_t
    {
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
        ResourceManager_ResourceAlreadyCreated                          =   -1001,
        ResourceManager_ProxyCreationFailed                             =   -1010,
        ResourceManager_RootProxyFetchFailed                            =   -1050,
        ResourceManager_BaseProxyCastFailed                             =   -1051, // ResourceManager::Proxy
        ResourceManager_DependencyLoadFailed						    =   -1052,
        ResourceManager_ProxyLoadFailed                                 =   -1053,
        ResourceManager_ProxyUnloadFailed                               =   -1054,
        ResourceManager_ResourceLoadFailed							    =   -1060,
        GAPI_Generic                                                    =  -50000, // GAPI
        GAPI_NoPrimaryAdapter                                           =  -50001,
        GAPI_NoPrimaryAdapterOutputs                                    =  -50002,
        GAPI_NoPrimaryAdapterOutputModes                                =  -50003,
        GAPI_InvalidHandle                                              =  -50020,
        GFXAPI_SubsystemThreadEnqueueFailed                             =  -50200, // GFXAPI-Subsystem
        GFXAPI_SubsystemResourceCreationFailed                          =  -50201,
        GFXAPI_SubsystemResourceDestructionFailed                       =  -50202,
        GFXAPI_LoadResourceFailed                                       =  -50210,
        GFXAPI_UnloadResourceFailed                                     =  -50211,
        ResourceTaskBackend_FunctionNotFound                            =  -50500,
        ResourceTaskBackend_FunctionTypeInvalid                         =  -50501,
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
        DXDevice_CreateRTV_Failed                                       = -100400,
        DXDevice_CreateSRV_Failed                                       = -100401,
        DXDevice_CreateDSV_Failed                                       = -100402,
        DXDevice_CreateDSS_Failed                                       = -100403,
        DXDevice_CreateRS_Failed                                        = -100404,
        DXDevice_CreateSwapChainBuffer_Failed                           = -100420,
        DXDevice_SRVCreationFailed                                      = -100430,
        DXDevice_DepthStencilTextureCreationFailed                      = -100501,
        DXDevice_DepthStencilStateCreationFailed                        = -100502,
        DXDevice_RasterizerStateCreationFailed                          = -100550,
        Timer_NoPlatformTimeInstance                                    = -300050,
        Time_Win32__QueryPerformanceFrequencyFailed                     = -300101,
        Time_Win32__QueryPerformanceCounterFailed                       = -300102

    };

    /**
     * @brief CheckEngineError
     * @param status
     * @return
     */
    static bool CheckEngineError(EEngineStatus const &aStatus);

    /**
     * @brief The EngineException class
     */
    class CEngineException
            : public std::runtime_error
    {
    public:
        /**
         * @brief CEngineException
         * @param aStatus
         * @param aMessage
         */
        CEngineException(
                EEngineStatus const &aStatus,
                std::string   const &aMessage);

        /**
         * @brief status
         * @return
         */
        EEngineStatus const &status() const;

        /**
         * @brief message
         * @return
         */
        std::string const message() const;

    private_members:
        EEngineStatus mStatus;
    };

    static inline EEngineStatus HandleEngineStatusError(
            EEngineStatus const &aStatus,
            std::string   const &aMessage);

    static inline std::ostream& operator<<(std::ostream &aStream, const EEngineStatus &aStatus);
}

#endif
