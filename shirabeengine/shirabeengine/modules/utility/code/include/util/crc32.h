#ifndef __SHIRABE_CRC32_H__
#define __SHIRABE_CRC32_H__

#include <string>
#include <cstdint>

namespace engine
{
    namespace util
    {
        uint32_t crc32FromString(std::string const &aInput);
    }
}

#endif // CRC32_H
