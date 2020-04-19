#ifndef __SHIRABE_MATH_GEOMETRIC_RECT_H__
#define __SHIRABE_MATH_GEOMETRIC_RECT_H__

#include <cstdint>
#include "math/vector.h"

namespace engine
{
    namespace math
    {
        /**
         * CRect defines a simple rectangle structure.
         */
        struct SHIRABE_LIBRARY_EXPORT CRect
        {
            CVector2D<uint16_t> position;
            CVector2D<uint16_t> size;

            CRect();
            CRect(
                    uint16_t const&aX,
                    uint16_t const&aY,
                    uint16_t const&aWidth,
                    uint16_t const&aHeight);

            explicit CRect(
                    CVector2D<uint16_t> const& aPosition,
                    CVector2D<uint16_t> const& aSize);
        };

    }
}

#endif
