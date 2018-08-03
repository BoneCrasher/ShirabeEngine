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
    CEngineException::CEngineException(
            EEngineStatus const &aStatus,
            std::string   const &aMessage)
        : std::runtime_error(aMessage.c_str())
        , mStatus(aStatus)
    { }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    EEngineStatus const &CEngineException::status()  const
    {
        return mStatus;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    std::string   const  CEngineException::message() const
    {
        return this->what();
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    EEngineStatus HandleEngineStatusError(
            EEngineStatus const &aStatus,
            std::string   const &aMessage)
    {
        if(CheckEngineError(aStatus))
            throw CEngineException(aStatus, aMessage);

        return aStatus;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    std::ostream &operator<<(std::ostream& stream, const EEngineStatus& status)
    {
        return (stream << static_cast<std::underlying_type_t<EEngineStatus>>(status));
    }
    //<-----------------------------------------------------------------------------
}
