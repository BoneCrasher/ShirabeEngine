#pragma once

/*!
 * The EPropertySystemError enum defines property system error codes
 * for any specific error case.
 */
enum class EPropertySystemError
{
    // Error
    PropertyIdIsEmpty             = -1,
    PropertyAlreadyAdded          = -2,
    PropertyNotFound              = -3,
    IncompatiblePropertyType      = -4,
    PropertyValueIndexOutOfBounds = -5,
    PropertyPointerIsNull         = -6,
    // Neutral
    Undefined                     =  0,
    // Success
    Ok                            =  1
};
