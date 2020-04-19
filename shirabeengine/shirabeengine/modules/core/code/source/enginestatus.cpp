#include "core/enginestatus.h"

namespace engine
{
    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    bool CheckEngineError(EEngineStatus const &aStatus)
    {
        return (static_cast<std::underlying_type_t<EEngineStatus>>(aStatus) < 0); /* All errors are values less than zero. */
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    std::ostream &operator<<(std::ostream& stream, EEngineStatus const &status)
    {
        return (stream << static_cast<std::underlying_type_t<EEngineStatus>>(status));
    }
    //<-----------------------------------------------------------------------------
}
