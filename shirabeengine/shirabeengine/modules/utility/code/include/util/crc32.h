#ifndef __SHIRABE_CRC32_H__
#define __SHIRABE_CRC32_H__

#include <string>
#include <cstdint>

namespace engine
{
    namespace util
    {
        using CRC32Id_t = uint32_t;

        CRC32Id_t crc32FromString(std::string const &aInput);
    }
}

#endif // CRC32_H
