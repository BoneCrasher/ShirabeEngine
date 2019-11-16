/*!
 * @file      sr_platform.h
 * @author    andras.acsai
 * @date      2018/05/23
 * @copyright SmartRay GmbH (www.smartray.com)
 */

#pragma once


#include "core/message/sr_message.h"

class CApplicationBase;

class ANetworkManager;
class AConcurrencyManager;
class CTimeManager;
class CDiagnosticsManager;
class CFileManager;


/*!
 * @class CPlatformBase
 * @brief The base class of a platform object that provides functions, manager objects that 
 *        could contain platform dependent solutions
 */
class CPlatform
{
public_enums:
    /*!
     * @enum EPlatformType
     * @brief The list of the supported platforms
     */
    enum class EPlatformType
    {
        Windows                 = 1
    ,   Linux
    };

    /*!
     * @enum ESignalType
     * @brief The list of handled signals
     */
    enum class ESignalType
    {
        Interrupt
    };

public_destructors:
    /*!
     * Destructor
     */
    virtual ~CPlatform();


public_static_functions:
    /*!
     * Gets the single instance
     * 
     * @return CPlatformBase*   Pointer to the platform object
     */
    static SR_INLINE CPlatform* getInstance();

public_methods:
    /*!
     * Forces the application to quit
     */
    void quit( void );

    /*!
     * Gets the type of the current platform
     * 
     * @return EPlatformType    The type of the current platform
     */
    SR_INLINE EPlatformType getType() const;

    /*!
     * Gets the network manager
     * 
     * @return CNetworkManager* Pointer to the network manager object
     */
    SR_INLINE ANetworkManager* getNetworkManager() const;

    /*!
     * Gets the concurrency manager object
     * 
     * @return CConcurrencyManager* Pointer to the concurrency manager object
     */
    SR_INLINE AConcurrencyManager* getConcurrencyManager() const;

    /*!
     * Gets the time manager object
     * 
     * @return CTimeManager*        Pointer to the time manager object
     */
    SR_INLINE CTimeManager* getTimeManager() const;

    /*!
     * Gets the diagnostics manager
     * 
     * @return CDiagnosticsManager* Pointer to the diagnostics manager object
     */
    SR_INLINE CDiagnosticsManager* getDiagnosticsManager() const;

    /*!
     * Gets the file manager
     * 
     * @return CFileManager*        Pointer to the file manager object
     */
    SR_INLINE CFileManager* getFileManager() const;

protected_constructors:
    /*!
     * Constructor
     * 
     * @param  [in] aPlatformType   The type of the current platform
     */
    CPlatform( const EPlatformType aPlatformType );

protected_methods:
    /*!
     * Adds a message to the message queue
     * 
     * @param  [in] aMsg            The new message that will be placed into the queue
     */
    void pushMessage( const CMessage& aMsg );

    /*!
     * Gets the next message to be processed
     * 
     * @param  [in] aMsg            Fills with the data of message
     * @return bool                 True if there was a message, otherwise false
     */
    bool getMessage( CMessage& aMsg );


private_constructors:
    /*!
     * Copy constructor is disabled
     */
    CPlatform( const CPlatform& ) = delete;

private_methods:
    /*!
     * Assignment operator is disabled
     */
    CPlatform& operator=( const CPlatform& ) = delete;

protected_typedefs:
    typedef std::queue<CMessage>    MessageQueue_t;             //!< Type of the message queue

protected_members:
    const EPlatformType             mType;                      //!< Contains the type of the current platform

    CDiagnosticsManager*            mDiagnosticsManager;        //!< Points to the diagnostics manager object
    ANetworkManager*                mNetworkManager;            //!< Points to the network manager object
    AConcurrencyManager*            mConcurrencyManager;        //!< Points to the concurrency manager object
    CTimeManager*                   mTimeManager;               //!< Points to the time manager object
    CFileManager*                   mFileManager;               //!< Points to the file manager object

    CSpinLock                       mMessageQueueLock;          //!< Lock to protect the message queue
    MessageQueue_t                  mMessageQueue;              //!< The message queue

protected_static_fields:
    static CPlatform*               sInstance;                  //!< Pointer to the single instance

};

//<---------------------------------------------
SR_INLINE CPlatform* CPlatform::getInstance()
{
    SR_ASSERT( nullptr != sInstance );
    return sInstance;
}

//<---------------------------------------------
SR_INLINE CPlatform::EPlatformType CPlatform::getType() const
{
    return mType;
}

//<---------------------------------------------
SR_INLINE ANetworkManager* CPlatform::getNetworkManager() const
{
    SR_ASSERT( nullptr != mNetworkManager );
    return mNetworkManager;
}

//<---------------------------------------------
SR_INLINE AConcurrencyManager* CPlatform::getConcurrencyManager() const
{
    SR_ASSERT( nullptr != mConcurrencyManager );
    return mConcurrencyManager;
}

//<---------------------------------------------
SR_INLINE CTimeManager* CPlatform::getTimeManager() const
{
    SR_ASSERT( nullptr != mTimeManager );
    return mTimeManager;
}

//<---------------------------------------------
SR_INLINE CDiagnosticsManager* CPlatform::getDiagnosticsManager() const
{
    SR_ASSERT( nullptr != mDiagnosticsManager );
    return mDiagnosticsManager;
}

//<---------------------------------------------
SR_INLINE CFileManager* CPlatform::getFileManager() const
{
    SR_ASSERT( nullptr != mFileManager );
    return mFileManager;
}


