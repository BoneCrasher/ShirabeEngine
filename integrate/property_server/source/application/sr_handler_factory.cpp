#include "sr_pch.h"

#include "sr_handler_factory.h"

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
CHandlerFactory::CHandlerFactory(CStdSharedPtr_t<CTCPSession> aSession)
    : mSession(aSession)
{}
//<-----------------------------------------------------------------------------
