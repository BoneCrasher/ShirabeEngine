#ifndef __SHIRABE_WSI_DISPLAY_H__
#define __SHIRABE_WSI_DISPLAY_H__

#include <assert.h>
#include <core/enginetypehelper.h>
#include <core/enginestatus.h>
#include <os/osdisplay.h>

namespace engine
{
    namespace wsi
    {
        /**
         * The CWSIDisplay class is responsible to interface with a operating system window system
         * display, which is usually used as the link between window management logic and the
         * window instances.
         *
         * On Windows, the CWSIDisplay is basically empty, but will be used for consistency, as
         * we need a display for X11 variant integration.
         *
         * The core information of the display is its handle, which uniquely identifies
         * the display instance.
         *
         * This class is abstract and has to be specialized by OS aware display integrations.
         */
        class CWSIDisplay
        {
        public_destructors:
            /**
             * Destroy and run...
             */
            virtual ~CWSIDisplay() = default;

        public_methods:
            /**
             * Initialize the display, binding to the operating system mechanics for displays,
             * trying to fetch information about the available screens of the display,
             * i.e. connected and activated monitors, which are integrated in the current desktop.
             *
             * @return EEngineStatus::Ok, if successful. An error otherwise.
             */
            virtual EEngineStatus initialize()   = 0;

            /**
             * Clear resources and unbind from the operation system mechanics for displayes.
             *
             * @return EEngineStatus::Ok, if successful. An error otherwise.
             */
            virtual EEngineStatus deinitialize() = 0;

            /**
             * Return the currently attached display unique handle.
             *
             * @return A handle of value greater zero, if valid. 0 otherwise.
             */
            SHIRABE_INLINE uint64_t displayHandle() const
            {
                return mHandle;
            }

            /**
             * Return a list of screens attached to the display.
             *
             * @return See brief.
             */
            SHIRABE_INLINE Vector<os::SOSDisplayDescriptor> const &screenInfo() const
            {
                return mScreenInfo;
            }

            /**
             * Return the zero-based index of the primary screen.
             *
             * @return See brief.
             */
            SHIRABE_INLINE uint64_t primaryScreenIndex() const
            {
                return mPrimaryScreenIndex;
            }

        protected_constructors:
            /**
             * Default-Construct an empty display.
             */
            CWSIDisplay() = default;

        protected_methods:
            /**
             * Internally set the attached display handle, once available.
             *
             * @param aHandle The handle to attach to.
             */
            SHIRABE_INLINE void setDisplayHandle(uint64_t const &aHandle)
            {
                assert(0 != aHandle);

                mHandle = aHandle;
            }

            /**
             * Internally set the attached screen list for the display, once available.
             *
             * @param aInfo A Vector of screen information entries.
             */
            SHIRABE_INLINE void setScreenInfo(Vector<os::SOSDisplayDescriptor> const &aInfo)
            {
                mScreenInfo = aInfo;
            }

            /**
             * Internally set the attached screen's primary screen index, once available.
             *
             * @param aIndex The zero-based index of the primary screen.
             */
            SHIRABE_INLINE void setPrimaryScreenIndex(uint64_t const &aIndex)
            {
                mPrimaryScreenIndex = aIndex;
            }

        protected_methods:
            /**
             * Determine the screens of the operating system.
             *
             * @param aOutPrimaryDisplayIndex The primary screen handle will be stored in this variable.
             * @return                        A list of display descriptors, if any.
             */
            virtual Vector<os::SOSDisplayDescriptor> getScreenInfo(uint32_t &aOutPrimaryDisplayIndex) = 0;

        private_members:
            uint64_t                         mHandle;
            uint64_t                         mPrimaryScreenIndex;
            Vector<os::SOSDisplayDescriptor> mScreenInfo;
        };

    }
}

#endif // DISPLAY_H
