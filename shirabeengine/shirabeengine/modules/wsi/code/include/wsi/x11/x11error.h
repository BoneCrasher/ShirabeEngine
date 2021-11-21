#ifndef __SHIRABE_WINDOWS_ERROR_H__
#define __SHIRABE_WINDOWS_ERROR_H__

#include <string>
#include <cstdint>

#include <X11/Xlib.h>

#ifdef Status
#undef Status
#endif

#include <core/enginestatus.h>
#include <base/string.h>

namespace engine
{
    namespace wsi
    {
        namespace x11
        {
            using namespace engine;

            /**
             * The CX11Exception class encapsulates an error message, integral error code and engine status code
             * as a std::runtime_error compatible entity.
             */
            class CX11Exception
            {
            public_constructors:
                /**
                 * Construct a CX11Exception from various information.
                 *
                 * @param aCode    X11 internal error code.
                 * @param aStatus  EEngineStatus error code.
                 * @param aMessage A text message related to the error.
                 */
                SHIRABE_INLINE CX11Exception(
                        uint8_t       const &aCode,
                        EEngineStatus const &aStatus,
                        std::string   const &aMessage)
                    : mErrorCode(aCode)
                    , mEngineStatus(aStatus)
                    , mMessage(aMessage)
                {}

            public_methods:
                /**
                 * Return the x11 error code.
                 *
                 * @return See brief.
                 */
                SHIRABE_INLINE uint8_t const &errorCode() const
                {
                    return mErrorCode;
                }

                /**
                 * Return the engine error code.
                 *
                 * @return See brief.
                 */
                SHIRABE_INLINE EEngineStatus const &engineStatus() const
                {
                    return mEngineStatus;
                }

                /**
                 * Return the attached error message.
                 *
                 * @return See brief.
                 */
                SHIRABE_INLINE std::string const &message() const
                {
                    return mMessage;
                }

            private_members:
                uint8_t       mErrorCode;
                EEngineStatus mEngineStatus;
                std::string   mMessage;
            };

            /**
             * Handle an X11 error and throw a CX11Exception.
             *
             * @param aErrorCode    The X11 error code.
             * @param aEngineSTatus The engine status code.
             * @param aMessage      The attached error message.
             */
            static void HandleX11Error(
                    uint8_t       const &aErrorCode,
                    EEngineStatus const &aEngineStatus,
                    std::string   const &aMessage)
            {
                if(aErrorCode)
                {
                    std::string const wrappedMessage
                            = engine::StaticStringHelpers::format("X11 error occurred (code: {}, EngineStatus: {}): {}", aErrorCode, aEngineStatus, aMessage);

                    throw CX11Exception(aErrorCode, aEngineStatus, aMessage);
                }
            }
        }
    }
}

#endif
