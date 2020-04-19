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
#include "core/result.h"
#include "log/log.h"

namespace engine
{
    /**
     * @brief The EEngineStatus enum
     */
    enum class EEngineStatus
            : int32_t
    {
        Ok                                                              =       0,
        Resource_Initializing                                           =  100100,
        Resource_Transferring                                           =  100101,
        Resource_Deinitializing                                         =  100102,
        Resource_NotReady                                               =  100103,
        Error                                                           =      -1,  // Non-specific issues
        NotImplemented                                                  =     -10,
        NullPointer                                                     =     -26,  // General parameter issues
        OutOfBounds                                                     =     -51,  // Array/Vector
        CollectionOperationException                                    =     -70,  // Collection issues
        CollectionInsertException                                       =     -71,
        CollectionEraseException                                        =     -72,
        ObjectAlreadyAddedToCollection                                  =     -80,
        ObjectNotAddedToCollection                                      =     -81,
        FileNotFound                                                    =    -101, // Filesystem and handles
        CreationError                                                   =    -201,
        InitializationError                                             =    -202, // Lifecycle
        UpdateError                                                     =    -203,
        DeinitializationError                                           =    -204,
        DestructionError                                                =    -205,
        ResourceError_CreationFailed                                    =    -251,
        ResourceError_AlreadyCreated                                    =    -252,
        ResourceError_NotFound                                          =    -253,
        ResourceError_LoadFailed                                        =    -254,
        ResourceError_UnloadFailed                                      =    -255,
        ResourceError_DestructionFailed                                 =    -256,
        ResourceError_ResourceInvalid                                   =    -257,
        ResourceError_DependencyNotFound                                =    -258,
        WindowCreationError                                             =    -351, // Window issues
        WindowMessageHandlerError                                       =    -352,
        WindowEventError                                                =    -353,
        ResourceManager_Generic                                         =   -1000, // ResourceManager
        ResourceManager_Setup_Failed                                    =   -1001,
        ResourceManager_ResourceAlreadyCreated                          =   -1101,
        ResourceManager_ResourceNotFound                                =   -1102,
        ResourceManager_ProxyNotFound                                   =   -1103,
        ResourceManager_ProxyCreationFailed                             =   -1110,
        ResourceManager_RootProxyFetchFailed                            =   -1150,
        ResourceManager_BaseProxyCastFailed                             =   -1151, // ResourceManager::Proxy
        ResourceManager_DependencyLoadFailed						    =   -1152,
        ResourceManager_ProxyLoadFailed                                 =   -1153,
        ResourceManager_ProxyUnloadFailed                               =   -1154,
        ResourceManager_ResourceLoadFailed							    =   -1160,
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
        Timer_PlatformTimeInstanceInitFailed                            = -300051,
        Timer_PlatformTimeConversionConstantFetchFailed                 = -300052,
        Timer_PlatformTimestampFetchFailed                              = -300053,
        Time_Win32__QueryPerformanceFrequencyFailed                     = -300101,
        Time_Win32__QueryPerformanceCounterFailed                       = -300102,
        FrameGraph_Generic                                              = -500001,
        FrameGraph_PassBuilder_TextureIsBeingRead                       = -500101,
        FrameGraph_PassBuilder_TextureIsBeingWritten                    = -500102,
        FrameGraph_PassBuilder_DuplicateTextureViewId                   = -500103,
        FrameGraph_PassBuilder_ForwardResourceFailed                    = -500104,
        FrameGraph_PassBuilder_AcceptResourceFailed                     = -500105,
        FrameGraph_PassBuilder_ReadResourceFailed                       = -500106,
        FrameGraph_PassBuilder_WriteResourceFailed                      = -500107,
        FrameGraph_PassBuilder_ImportRenderablesFailed                  = -500151,
        FrameGraph_RenderContext_AttachmentDimensionsInvalid            = -500501,
    };

    /**
     * @brief CheckEngineError
     * @param status
     * @return
     */
    bool CheckEngineError(EEngineStatus const &aStatus);

    SHIRABE_INLINE static void EngineStatusPrintOnError(EEngineStatus const &aStatus, std::string const &aLogTag, std::string const &aMessage)
    {
        bool const error = CheckEngineError(aStatus);
        if(error)
        {
            CLog::Error(aLogTag, aMessage);
        }
    }

    template <typename TResult>
    static bool IsEngineError(TResult const &aPotentialErrorValue)
    {
        if constexpr(std::is_same_v<TResult, EEngineStatus>)
        {
            return CheckEngineError(aPotentialErrorValue);
        }
        else if constexpr(std::is_same_v<TResult, bool>)
        {
            return aPotentialErrorValue;
        }
        else
        {
            return true;
        }
    }

    template <typename TResult>
    static void PrintEngineError(TResult     const &aPotentialErrorValue,
                                 std::string const &aLogTag,
                                 std::string const &aFormat)
    {
        if constexpr(std::is_same_v<TResult, EEngineStatus>)
        {
            EngineStatusPrintOnError(aPotentialErrorValue, aLogTag, aFormat);
        }
        else if constexpr(std::is_same_v<TResult, bool>)
        {
            if(aPotentialErrorValue)
            {
                EngineStatusPrintOnError(EEngineStatus::Error, aLogTag, aFormat);
            }
        }
    }

    template <typename TResult, typename... TArgs>
    static void PrintEngineError(TResult     const &aPotentialErrorValue,
                                 std::string const &aLogTag,
                                 std::string const &aFormat,
                                 TArgs         &&...aArgs)
    {
        if constexpr(std::is_same_v<TResult, EEngineStatus>)
        {
            EngineStatusPrintOnError(aPotentialErrorValue, aLogTag, CString::format(aFormat, std::forward<TArgs>(aArgs)...));
        }
        else if constexpr(std::is_same_v<TResult, bool>)
        {
            if(aPotentialErrorValue)
            {
                EngineStatusPrintOnError(EEngineStatus::Error, aLogTag, CString::format(aFormat, std::forward<TArgs>(aArgs)...));
            }
        }
    }

    template <typename TResult>
    static EEngineStatus ConvertError(TResult const &aPotentialErrorValue)
    {
        if constexpr(std::is_same_v<TResult, EEngineStatus>)
        {
            return aPotentialErrorValue;
        }
        else
        {
            return EEngineStatus::Error;
        }
    }

    #define SHIRABE_RETURN_RESULT_ON_ERROR(status) \
            if(IsEngineError(status))              \
            {                                      \
                                                   \
                return ConvertError(status);       \
            }

    #define SHIRABE_RETURN_VALUE_ON_ERROR(status, value) \
            if(IsEngineError(status))                    \
            {                                            \
                                                         \
                return value;                            \
            }

    /**
     * The CEngineResult class is a SResult implementation to return result tuples of
     * result type EEngineStatus.
     */
    template <typename TData = void>
    class [[nodiscard]] CEngineResult
        : public AResult<EEngineStatus, TData>
    {
    public_constructors:
        // Reuse the base class constructors.
        using AResult<EEngineStatus, TData>::AResult;
        using AResult<EEngineStatus, TData>::operator=;
        using AResult<EEngineStatus, TData>::get;

    public_methods:
        /**
         * Evaluate, whether the result contains a successful result code.
         *
         * @return See brief.
         */
        SHIRABE_INLINE bool successful() const
        {
            bool const success = not CheckEngineError(AResult<EEngineStatus, TData>::result());
            return success;
        }

        /**
         * Check, whether the stored result equals the result being provided as the argument.
         *
         * @param aOtherStatus
         * @return
         */
        SHIRABE_INLINE bool resultEquals(EEngineStatus const &aOtherStatus) const
        {
            bool const equal = (AResult<EEngineStatus, TData>::result() == aOtherStatus);
            return equal;
        }

    private_members:
        template <typename TOtherData> friend class CEngineResult;
    };

    /**
     * Print out an EEngineStatus value to an ostream.
     *
     * @param aStream
     * @param aStatus
     * @return
     */
    std::ostream& operator<<(std::ostream &aStream, EEngineStatus const &aStatus);
}

// Make CEngineResult decomposable...
namespace std
{
    template <typename TData>
    struct tuple_size<engine::CEngineResult<TData>>
        : std::integral_constant<std::size_t, 2>
    {};

    template <>
    struct tuple_size<engine::CEngineResult<void>>
        : std::integral_constant<std::size_t, 1>
    {};

    template <typename TData>
    struct tuple_element<0, engine::CEngineResult<TData>>
    {
        using type = engine::EEngineStatus;
    };

    template <typename TData>
    struct tuple_element<1, engine::CEngineResult<TData>>
    {
        using type = TData;
    };

    template <>
    struct tuple_element<0, engine::CEngineResult<void>>
    {
        using type = engine::EEngineStatus;
    };

    // template <std::size_t N, typename TResult, typename TData>
    // struct tuple_element<N, engine::AResult<TResult, TData>>
    // {
    //     using type = decltype(std::declval<engine::AResult<TResult, TData>>().getResource<N>());
    // };
    //
    // template <std::size_t N, typename TResult>
    // struct tuple_element<N, engine::AResult<TResult, void>>
    // {
    //     using type = decltype(std::declval<engine::AResult<TResult, void>>().getResource<N>());
    // };
}

#endif
