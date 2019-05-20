#include <util/crc32.h>

#include <sstream>
#include <iomanip>

#include <cryptopp/crc.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>

namespace engine
{
    namespace util
    {
        uint32_t crc32FromString(std::string const &aInput)
        {
            std::string const source  = aInput;
            std::string       hashStr = "";
            uint32_t          hash    = 0u;

            CryptoPP::CRC32 crc {};

            // auto const sink    = new CryptoPP::ArraySink(reinterpret_cast<CryptoPP::byte*>(&hash), sizeof(hash));
            auto const sink    = new CryptoPP::StringSink(hashStr);
            auto const encoder = new CryptoPP::HexEncoder(sink);
            auto const filter  = new CryptoPP::HashFilter(crc, encoder);

            CryptoPP::StringSource(source, true, filter);

            std::stringstream converter;
            converter << std::hex << hashStr;
            converter >> hash;

            return hash;
        }
    }
}
