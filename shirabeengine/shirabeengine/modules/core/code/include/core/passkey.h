#ifndef __SHIRABE_DESIGN_PASSKEY_H__
#define __SHIRABE_DESIGN_PASSKEY_H__

#include <base/declaration.h>

namespace engine
{
    /**
     * Declares the Pass-Key idiom to avoid friend class access to
     * everything, but permit case-by-case access.
     *
     * @tparam The class which wants access to private data.
     */
    template <typename TAccessor>
    class CPassKey
    {
    private_typedefs:
        friend TAccessor;

    private_constructors:
        /**
         * Construct an empty pass key.
         */
        CPassKey() = default;
        /**
         * Copy one pass key to another.
         */
        CPassKey(CPassKey const&) = default;

    private_operators:
        /**
         * No assignment possible...
         *
         * @return Self-Ref.
         */
        CPassKey& operator=(CPassKey const&) = delete;
    };

}

#endif
