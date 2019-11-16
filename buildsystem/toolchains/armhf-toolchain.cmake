set(CMAKE_SYSTEM_NAME Linux)

set(CROSS_COMPILE "arm-linux-gnueabihf-")

set(CMAKE_C_COMPILER     ${CROSS_COMPILE}gcc)

set(CMAKE_CXX_COMPILER   ${CROSS_COMPILE}g++)

set(CMAKE_C_FLAGS        "-march=armv7-a -marm -mthumb-interwork -mfpu=neon -mfloat-abi=hard" CACHE STRING "" FORCE)

set(CMAKE_CXX_FLAGS      ${CMAKE_C_FLAGS} CACHE STRING "" FORCE)

set(CMAKE_AR             ${CROSS_COMPILE}ar CACHE FILEPATH "Archiver")

set(CMAKE_LINKER         ${CROSS_COMPILE}ld)
