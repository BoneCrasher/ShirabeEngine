#pragma once

#include <property_system/serialization/sr_property_json_de_serializer.h>

// TBDone: Put the property system unit test read/write file/buffer functions into core_base...

/*!
 * Read a file from disk into a string buffer.
 *
 * @param [in]  aFilename The filename of the file to read.
 * @param [out] aOutData  Placeholder for the data to be read.
 * @return
 */
bool readFile(std::string const &aFilename, std::string &aOutData);

/*!
 * Fwd declaration... ( I know this is the most reasonable comment ever... )
 */
class CHandlerFactory;

/*!
 * The CState class is a wrapper around a loaded property-system file providing
 * immutable access to a root CMetaObject and a PathMap indexing all available
 * properties by their property-path.
 */
class CState
{
public_constructors:
    /*!
     * Construct a new state accepting an external handler factory.
     *
     * @param [in] aFactory Handlerfactory used to create replication handlers for properties.
     */
    CState(CStdSharedPtr_t<CHandlerFactory> aFactory);

public_methods:
    /*!
     * Initialize the state, effectively causing it to load a specific file.
     *
     * @return True, if successful. False otherwise.
     */
    bool initialize();

    /*!
     * Serialize the currently stored state to string.
     *
     * @param [out] aOutSerialized Contains the text representation of the current tree
     *                             if successful.
     * @return                     True, if successful. False, otherwise.
     */
    bool getSerializedState(std::string &aOutSerialized);

private:
    CStdSharedPtr_t<CHandlerFactory> mHandlerFactory;
    CStdSharedPtr_t<CMetaObject>     mRoot;
    CPropertyDeserializer::PathMap_t mPathMap;
};
