/*!
 * @file      sr_platform.cpp
 * @author    andras.acsai
 * @date      2018/05/23
 * @copyright SmartRay GmbH (www.smartray.com)
 */


#include "sr_pch.h"
#include <core/platform/sr_platform.h>
#include <core/concurrency/sr_aconcurrency_manager.h>
#include <core/network/sr_anetwork_manager.h>


CPlatform*      CPlatform::sInstance = nullptr;

//<---------------------------------------------
CPlatform::CPlatform( const CPlatform::EPlatformType aPlatformType )
    : mType              ( aPlatformType )
    , mDiagnosticsManager( nullptr       )
    , mNetworkManager    ( nullptr       )
    , mConcurrencyManager( nullptr       )
    , mTimeManager       ( nullptr       )
    , mFileManager       ( nullptr       )
{
}

//<---------------------------------------------
CPlatform::~CPlatform()
{
    SR_SAFE_DELETE( mNetworkManager     );
    SR_SAFE_DELETE( mConcurrencyManager );
    SR_SAFE_DELETE( mTimeManager        );
    SR_SAFE_DELETE( mFileManager        );

    //!< destroy the diagnostics manager last
    SR_SAFE_DELETE( mDiagnosticsManager );
}

//<---------------------------------------------
void CPlatform::quit( void )
{
    const CMessage quitMsg( CMessage::EMessageType::Quit );

    pushMessage( quitMsg );
}

//<---------------------------------------------
void CPlatform::pushMessage( const CMessage& aMsg )
{
    CSpinLockGuard messageQueueLockGuard( mMessageQueueLock );

    mMessageQueue.push( aMsg );
}

//<---------------------------------------------------------
bool CPlatform::getMessage( CMessage& aMsg )
{
    CSpinLockGuard MessageQueueLockGuard( mMessageQueueLock );

    if ( mMessageQueue.empty() )
    {
        return false;
    }
    else
    {
        aMsg = mMessageQueue.front();
        mMessageQueue.pop();
        return true;
    }
}


