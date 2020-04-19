/*!
 * @file      sr_variant.h
 * @author    Marc-Anton Boehm-von Thenen
 * @date      27/06/2018
 * @copyright SmartRay GmbH (www.smartray.com)
 */

#pragma once

#include <variant>

#include "core/base/types/smartptr/sr_cppstd_ptr.h"

/*!
 * Casts a shared pointer to a variant to a shared pointer of it's current variant type.
 *
 * @tparam TVariantViewType     Expected current variant type stored in the variant.
 * @tparam TVariantType         Type of the containing variant.
 * @param  [in] aVariantPointer Shared-Pointer to variant instance.
 * @returns                     A shared-pointer to the expected stored type in the variant.
 */
template <typename TVariantViewType, typename TVariantType>
CStdSharedPtr_t<TVariantViewType> static_variant_pointer_cast(CStdSharedPtr_t<TVariantType> aVariantPointer)
{
    TVariantViewType                 *extractedValuePointer = &std::get<TVariantViewType>(*aVariantPointer);
    std::shared_ptr<TVariantViewType> output                =  std::shared_ptr<TVariantViewType>(aVariantPointer, extractedValuePointer);
    return output;
}

/*!
 * The CVariantResolver class provides a set of operator()'s from a list of resolver
 * functions, to be used within std::visit.
 *
 * @tparam TResolverFunctionTypes... List of functions/lambdas to be invoked by std::visit, taking one parameter of a
 *                                   supported type of a given std::variant.
 */
template<typename... TResolverFunctionTypes>
struct SVariantResolver
        : TResolverFunctionTypes...
{
    using TResolverFunctionTypes::operator()...;
};

/*!
 * User-Defined Deduction Guide: Accepting a list of resolver functions, which are mapped
 * over to a CVariantResolver-specialization of these function types.
 *
 * @tparam TResolverFunctionTypes... List of functions/lambdas to be invoked by std::visit, taking one parameter of a
 *                                   supported type of a given std::variant.
 */
template<typename... TResolverFunctionTypes>
SVariantResolver(TResolverFunctionTypes...) -> SVariantResolver<TResolverFunctionTypes...>;
