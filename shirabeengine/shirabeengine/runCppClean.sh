#!/bin/bash

INCLUDE_PATHS="
--include-path=./code/include
--include-path=./modules/asset/code/include
--include-path=./modules/base/code/include
--include-path=./modules/core/code/include
--include-path=./modules/directx/code/include
--include-path=./modules/graphicsapi/code/include
--include-path=./modules/log/code/include
--include-path=./modules/material/code/include
--include-path=./modules/math/code/include
--include-path=./modules/os/code/include
--include-path=./modules/platform/code/include
--include-path=./modules/rendering/code/include
--include-path=./modules/resources/code/include
--include-path=./modules/utility/code/include
--include-path=./modules/vulkan_integration/code/include
--include-path=./modules/wsi/code/include
"

DATE=$(date +%F-%T)

COMMAND="cppclean ${INCLUDE_PATHS} . &> cppclean_dump_${DATE}.md"

eval ${COMMAND}
