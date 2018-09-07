#ifndef __SHIRABE_COMPONENT_H__
#define __SHIRABE_COMPONENT_H__

#include <core/enginetypehelper.h>
#include <core/enginestatus.h>
#include <core/benchmarking/timer/timer.h>

namespace engine
{
    /**
     * The IComponent interface describes the basic capabilitites of all system components.
     */
    class IComponent
    {
        SHIRABE_DECLARE_INTERFACE(IComponent);

    public_api:
        /**
         * Return the name of the component.
         *
         * @return See brief.
         */
        virtual std::string const &name() const = 0;

        /**
         * Set the component name.
         *
         * @param aName The new name of the component.
         */
        virtual void name(std::string const &aName) = 0;

        /**
         * Update the component with an optionally usable timer component.
         *
         * @param aTimer A timer providing the currently elapsed time.
         * @return       Return EEngineStatus::Ok, if successful. An error code otherwise.
         */
        virtual EEngineStatus update(CTimer const &aTimer) = 0;
    };

    SHIRABE_DECLARE_LIST_OF_TYPE(CStdSharedPtr_t<IComponent>, IComponent);

}

#endif
