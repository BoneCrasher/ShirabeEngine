#ifndef __SHIRABE_WINDOWS_ERROR_H__
#define __SHIRABE_WINDOWS_ERROR_H__

#include <string>
#include <stdint.h>
#include <Windows.h>

#include "Core/String.h"
#include "Core/EngineStatus.h"

namespace Platform {
  namespace Windows {
    using namespace Engine;

    class WindowsException {
    public:
      WindowsException(
        int64_t             code,
        EEngineStatus const&status,
        std::string   const&message)
        : m_errorCode(code)
        , m_engineStatus(status)
        , m_message(message)
      {}

      int64_t       const& errorCode()    const { return m_errorCode;    }
      EEngineStatus const& engineStatus() const { return m_engineStatus; }
      std::string   const& message()      const { return m_message;      }

    private:
      int64_t       m_errorCode;
      EEngineStatus m_engineStatus;
      std::string   m_message;
    };
    
    static void HandleWindowsError(HRESULT hres, EEngineStatus const&engineStatus, std::string const&message) {
      if(FAILED(hres)) {
        std::string wrappedMessage
          = Engine::String::format("Windows/WINAPI error occurred (HRESULT: %0, EngineStatus: %1): %2", hres, engineStatus, message);
      
        throw WindowsException(static_cast<int64_t>(hres), engineStatus, message);
      }
    }

    static void HandleWindowsError(DWORD win32Error, EEngineStatus const&engineStatus, std::string const&message) {
      HandleWindowsError(HRESULT_FROM_WIN32(win32Error), engineStatus, message);
    }

  }
}

#endif