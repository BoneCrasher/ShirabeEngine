#ifndef __SHIRABE_GAPI_DEVICECAPABILITIES_H__
#define __SHIRABE_GAPI_DEVICECAPABILITIES_H__

#include <core/basictypes.h>
#include <core/enginestatus.h>
#include <core/enginetypehelper.h>
#include <log/log.h>

namespace engine
{
    namespace gfxapi
    {
        using namespace engine::resources;

        struct SGAPIOutputMode
        {
            uint32_t            modeIndex;
            Format              format;
            CVector2D<uint32_t> size;
            CVector2D<uint32_t> refreshRate;
        };
        SHIRABE_DECLARE_LIST_OF_TYPE(SGAPIOutputMode, SGAPIOutputMode);

        struct SGAPIOutput
        {
            uint32_t            outputIndex;
            SGAPIOutputModeList outputModes;
        };
        SHIRABE_DECLARE_LIST_OF_TYPE(SGAPIOutput, SGAPIOutput);

        struct SAPIAdapterDescription
        {
            std::string   name;
            unsigned long dedicatedGPUMemory;
            unsigned long dedicatedSysMemory;

        };

        struct SGAPIAdapter
        {
            uint32_t               adapterIndex;
            SAPIAdapterDescription description;
            SGAPIOutputList        outputs;
        };
        SHIRABE_DECLARE_LIST_OF_TYPE(SGAPIAdapter, SGAPIAdapter);

        struct SGAPIAdapterOutputConfig
        {
            int32_t adapterIndex;
            int32_t outputIndex;
            int32_t modeIndex;
        };

        struct SGAPIDeviceCapabilities
        {
            SGAPIAdapterList         adapters;
            SGAPIAdapterOutputConfig selectedConfig;
        };

    }
}
#endif 
