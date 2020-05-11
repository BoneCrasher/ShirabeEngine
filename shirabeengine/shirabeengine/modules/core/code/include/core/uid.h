#ifndef __SHIRABE_CORE_UID_H__
#define __SHIRABE_CORE_UID_H__

#include "core/enginetypehelper.h"

namespace engine
{
    namespace core
    {
        /**
         * Generic interface for any kind of random generator to be used.
         */
        template <typename TUIDUnderlyingType>
        class IUIDGenerator
        {
            SHIRABE_DECLARE_INTERFACE(IUIDGenerator);

        public_api:
            /**
             * Fetch the next random number from the underlying generator.
             *
             * @return
             */
            virtual TUIDUnderlyingType generate() = 0;
        };

        /**
         * The SequenceUIDGene rator class implements IUIDGenerator so that it
         * increments a RenderGraphResourceId_t linearly on each call to 'generate',
         * starting at 'aInitialID' provided to the constructor.
         */
        template <typename TID>
        class CSequenceUIDGenerator
                : public IUIDGenerator<TID>
        {
        public_constructors:
            /**
             * Default-Construct the generator with an initial Id to start incrementing from.
             *
             * @param aInitialID See brief.
             */

            explicit CSequenceUIDGenerator(TID const &aInitialID = 1)
                    : mId(aInitialID)
            {}

        public_methods:
            /**
             * Increment the counter and return it as a new id.
             *
             * @return See brief.
             */
            TID generate()
            {
                return (mId++);
            }

        private_members:
            TID mId;
        };
    }
}

#endif
