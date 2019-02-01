#!/bin/bash

ADDRESS_MODEL=$1
CONFIGURATION=$2

INCLUDE_PATHS="
--include-path=./__build/linux${ADDRESS_MODEL}/${CONFIGURATION}/intermediate/gen/export_headers
--include-path=./shirabeengine/code/include
--include-path=./shirabeengine/modules/asset/code/include
--include-path=./shirabeengine/modules/base/code/include
--include-path=./shirabeengine/modules/core/code/include
--include-path=./shirabeengine/modules/directx/code/include
--include-path=./shirabeengine/modules/graphicsapi/code/include
--include-path=./shirabeengine/modules/log/code/include
--include-path=./shirabeengine/modules/material/code/include
--include-path=./shirabeengine/modules/math/code/include
--include-path=./shirabeengine/modules/os/code/include
--include-path=./shirabeengine/modules/platform/code/include
--include-path=./shirabeengine/modules/rendering/code/include
--include-path=./shirabeengine/modules/resources/code/include
--include-path=./shirabeengine/modules/utility/code/include
--include-path=./shirabeengine/modules/vulkan_integration/code/include
--include-path=./shirabeengine/modules/wsi/code/include
"

DATE=$(date +%F-%T)

COMMAND="cppclean ${INCLUDE_PATHS} . &> cppclean_dump_${DATE}.md"

eval ${COMMAND}
