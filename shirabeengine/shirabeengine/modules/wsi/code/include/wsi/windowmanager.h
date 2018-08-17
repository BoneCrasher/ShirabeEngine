#ifndef __SHIRABE_WINDOWMANAGER_H__
#define __SHIRABE_WINDOWMANAGER_H__

#include <log/log.h>
#include <core/enginetypehelper.h>
#include <os/applicationenvironment.h>
#include "wsi/iwindowfactory.h"

namespace engine
{
    namespace wsi
    {
        /**
         * The CWindowManager class is used to create, access and destroy various
         * engine window instances during runtime.
         */
        class CWindowManager
        {
            SHIRABE_DECLARE_LOG_TAG(CWindowManager);

        public_enums:
            /**
             * The EWindowManagerError enum describes error flags related
             * to window manager operations.
             */
            enum class EWindowManagerError
                    : int8_t
            {
                Ok                     =    0,
                InitializationFailed   = -100,
                UpdateFailed           = -101,
                DeinitializationFailed = -102
            };

        public_constructors:
            /**
             * Default-Construct an empty window manager.
             */
            CWindowManager();

        public_destructors:
            /**
             * Destroy and run...
             */
            ~CWindowManager();

        public_methods:
            /**
             *
             * Initialize the window manager with an application environment instance.
             *
             * @param aApplicationEnvironment The environment to initialize the manager with.
             * @return                        EWindowManagerError::Ok if successful. An error code otherwise.
             */
            EWindowManagerError initialize(os::SApplicationEnvironment const &aApplicationEnvironment);

            /**
             * Deinitialize the window manager.
             *
             * @return EWindowManagerError::Ok if successful. An error code otherwise.
             */
            EWindowManagerError deinitialize();

            /**
             * @brief update
             * @return EWindowManagerError::Ok if successful. An error code otherwise.
             */
            EWindowManagerError update();

            /**
             * Create a new window from a name and initial bounds.
             *
             * @param aName          The name of the new window.
             * @param aInitialBounds The initial bounds of the new window.
             * @return               A pointer to the new window if successful. nullptr otherwise.
             */
            CStdSharedPtr_t<IWindow> createWindow(std::string const &aName, CRect const &aInitialBounds);

            /**
             * Try to find a window by name.
             *
             * @param aName The name of the window to find.
             * @return      A pointer to the window if successful. nullptr otherwise.
             */
            CStdSharedPtr_t<IWindow> getWindowByName(std::string const &aName);

            /**
             * Try to find a window by handle.
             *
             * @param aHandle The handle of the window to find.
             * @return        A pointer to the window if successful. nullptr otherwise.
             */
            CStdSharedPtr_t<IWindow> getWindowByHandle(os::CWindowHandleWrapper::Handle_t const &aHandle);

        private_members:
            IWindowList                     mWindows;
            CStdSharedPtr_t<IWindowFactory> mWindowFactory;
        };

        #define CheckWindowManagerError(status) \
            (static_cast<std::underlying_type_t<WindowManager::EWindowManagerError>>(status) < 0)
    }
}

#endif
