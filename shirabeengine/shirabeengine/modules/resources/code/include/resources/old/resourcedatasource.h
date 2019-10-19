#ifndef __SHIRABE_RESOURCES_DATASOURCE_H__
#define __SHIRABE_RESOURCES_DATASOURCE_H__

#include <memory>
#include <functional>

#include <core/databuffer.h>

namespace engine
{
    namespace resources
    {
        /**
         * Raw data source for the resource manager to use.
         * The data source does not keep the data in memory, but provides
         * dynamic access to the data referred to calling getData().
         */
        class CResourceDataSource
        {
        public_typedefs:
            using ResourceAccessFn_t = std::function<ByteBuffer()>;

        public_constructors:
            /**
             * Create an empty data source.
             */
            CResourceDataSource();

            /**
             * Create a new data source from a specific accessor function,
             * which knows how to load the raw data from it's respective source.
             *
             * @param aFunction Accessor function being able to create a ByteBuffer for it's
             *                  referred resource.
             */
            CResourceDataSource(ResourceAccessFn_t const &aFunction);

        public_methods:
            /**
             * Load and provide the referred resource data on demand.
             *
             * @return See brief.
             */
            ByteBuffer getData();

        private_members:
            ResourceAccessFn_t mAccessorFunction;
        };

    }
}

#endif
