#include "sr_pch.h"

#include <fstream>
#include <sstream>

#if __GNUC__ && __GNUC__ >= 8
#include <filesystem>
#elif __GNUC__ && __GNUC__ >= 7
#include <experimental/filesystem>
#endif

#include "sr_handler_factory.h"
#include "sr_state.h"

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
bool readFile(std::string const &aFilename, std::string &aOutData)
{
    SR_RETURN_IF(true == aFilename.empty(), false);

#if __GNUC__ && __GNUC__ >= 8
    bool const fileExists = std::filesystem::exists(aFilename);
#elif __GNUC__ && __GNUC__ >= 7
    bool const fileExists = std::experimental::filesystem::exists(aFilename);
#endif

    SR_RETURN_IF(false == fileExists, false);

    std::ifstream file(aFilename, std::ios::in);

    SR_RETURN_IF(true == (file.bad() || file.fail() || file.eof()), false);

    std::string data((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());

    aOutData = data;
    // File will be closed when leaving scope.
    return true;
}
//<-----------------------------------------------------------------------------

/*!
 * Creates a resolver usable within std::visit to set the property path to the correct
 * property instance of a variant.
 *
 * @tparam TType The underlying data type of the property.
 */
template <typename TType>
struct SRegisterReplicationCallbackResolver
{
    static auto getResolver(
            CStdSharedPtr_t<CMetaSystem> aMetaSystem,
            CHandlerFactory             &aHandlerFactory,
            bool                        &aInOutSuccess)
    {
        return [&] (CMetaProperty<TType> const &aResolvedInstance) -> void
        {
            aInOutSuccess = aInOutSuccess && aMetaSystem->registerReplicationCallback(aResolvedInstance, aHandlerFactory.createHandler<TType>());
        };
    }
};
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
CState::CState(CStdSharedPtr_t<CHandlerFactory> aFactory)
    : mHandlerFactory(aFactory)
{}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
bool CState::initialize()
{
    bool ok = true;

    // We register callbacks. System may not be const, as we alter the system's state.
    CStdSharedPtr_t<CMetaSystem> system = CMetaSystem::get();

    std::string demoData{};
    ok = ok && readFile("demo_machine_file.ref.txt", demoData);

    CStdSharedPtr_t<IDeserializer<CMetaObject>::IResult> deserializationResult = nullptr;

    bool const dynamicDeserializationMode = true;
    CPropertyDeserializer deserializer = { dynamicDeserializationMode };

    ok = ok && deserializer.initialize();
    ok = ok && deserializer.deserialize(demoData, deserializationResult);
    ok = ok && deserializer.deinitialize();
    ok = ok && deserializationResult->asT(mRoot);

    CStdSharedPtr_t<CPropertyDeserializer::CDeserializationResult> specificResultType = nullptr;
    specificResultType = std::static_pointer_cast<CPropertyDeserializer::CDeserializationResult>(deserializationResult);

    ok = ok && specificResultType->getPropertyPathMap(mPathMap);

    for(auto const&[path, propertyRef] : mPathMap)
    {
        MetaPropertyVariant_t const &variant = propertyRef->second;

        SR_RESOLVE(SRegisterReplicationCallbackResolver, variant, system, *mHandlerFactory, ok);
    }

    return ok;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
bool CState::getSerializedState(std::string &aOutSerialized)
{
    bool ok = true;

    CStdSharedPtr_t<ISerializer<CMetaObject>::IResult> serializationResult = nullptr;

    CPropertySerializer serializer;

    ok = ok && serializer.initialize();
    ok = ok && serializer.serialize(*mRoot, serializationResult);
    ok = ok && serializer.deinitialize();
    ok = ok && serializationResult->asString(aOutSerialized);

    return ok;
}
//<-----------------------------------------------------------------------------
