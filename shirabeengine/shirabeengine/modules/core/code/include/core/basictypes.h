#ifndef __SHIRABE_BASICTYPES_H__
#define __SHIRABE_BASICTYPES_H__

#include <array>
#include <cmath>
#include <cstdint>

#include <platform/platform.h>
#include <math/geometric/rect.h>

namespace engine
{
    using namespace engine::math;

    #ifdef min
    #undef min
    #endif

    #ifdef max
    #undef max
    #endif

    #define D_RANGE_REMAINING -1

    /**
     * @brief The CRange struct
     */
    struct SHIRABE_LIBRARY_EXPORT CRange
    {
        CRange();

        CRange(uint32_t const &aOffset,
               int32_t  const &aLength);

        bool overlapsWith(CRange const &aOther) const;

        uint32_t
          offset;
        int32_t
          length;
    };

    /**
     * @brief operator ==
     * @param aLHS
     * @param aRHS
     * @return
     */
    static bool operator==(
            CRange const &aLHS,
            CRange const &aRHS)
    {
        return
            ((aLHS.offset == aRHS.offset) &&
             (aLHS.length == aRHS.length));
    }

    /**
     * @brief from_string
     * @param aInput
     * @return
     */
    template <typename T>
    T from_string(std::string const &aInput)
    {
        T output{ };

        std::stringstream ss;
        ss << aInput;
        ss >> output;

        return output;
    }

    /**
     * @brief convert_to_string
     * @param aInput
     * @return
     */
    template <typename T>
    std::string convert_to_string(T const &aInput)
    {
        std::string output{ };

        std::stringstream ss;
        ss << aInput;
        output = ss.str();

        return output;
    }

    /**
     * @brief convert_to_string<CRange>
     * @param aRange
     * @return
     */
    template <>
    std::string convert_to_string<CRange>(CRange const&aRange);
}

#endif
