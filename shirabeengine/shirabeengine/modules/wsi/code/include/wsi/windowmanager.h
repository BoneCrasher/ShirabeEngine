#ifndef __SHIRABE_WINDOWMANAGER_H__
#define __SHIRABE_WINDOWMANAGER_H__

#include <log/log.h>
#include <core/enginetypehelper.h>
#include <os/applicationenvironment.h>
#include "wsi/iwindowfactory.h"
#include "wsi/display.h"
#include "wsi/windowhandlewrapper.h"

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
             * @param aFactory                The window factory attached to the manager.
             * @return                        EWindowManagerError::Ok if successful. An error code otherwise.
             */
            EWindowManagerError initialize(
                    os::SApplicationEnvironment const &aApplicationEnvironment,
                    Shared<IWindowFactory>      const &aFactory);

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
            Shared<IWindow> createWindow(std::string const &aName, CRect const &aInitialBounds);

            /**
             * Try to find a window by name.
             *
             * @param aName The name of the window to find.
             * @return      A pointer to the window if successful. nullptr otherwise.
             */
            Shared<IWindow> getWindowByName(std::string const &aName);

            /**
             * Try to find a window by handle.
             *
             * @param aHandle The handle of the window to find.
             * @return        A pointer to the window if successful. nullptr otherwise.
             */
            Shared<IWindow> getWindowByHandle(wsi::CWindowHandleWrapper::Handle_t const &aHandle);

        private_members:
            IWindowList                     mWindows;
            Shared<CWSIDisplay>    mDisplay;
            Shared<IWindowFactory> mWindowFactory;
        };

        #define CheckWindowManagerError(status) \
            (static_cast<std::underlying_type_t<CWindowManager::EWindowManagerError>>(status) < 0)
    }
}

#endif
