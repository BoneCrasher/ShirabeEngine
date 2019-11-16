#ifndef __SHIRABE_RANDOM_H__
#define __SHIRABE_RANDOM_H__

#include <random>

#include <base/declaration.h>
#include "core/uid.h"

namespace engine
{
    namespace random
    {
        using namespace engine::core;

        /**
         * The random state class is a wrapper around the mt19937_64 merseinne twister
         * integral distribution.
         */
        class RandomState
                : public IUIDGenerator<std::mt19937_64::result_type>
        {
        public:
            using RNGType_t          = std::mt19937_64;
            using DistributionType_t = std::uniform_int_distribution<RNGType_t::result_type>;

            /**
             * Create a random state and seed using std::random_device();
             */
            SHIRABE_INLINE RandomState()
            {
                mRandomNumberGenerator.seed(std::random_device()());
            }

            /**
             * Fetch the next number from the random distribution.
             *
             * @return
             */
            SHIRABE_INLINE RNGType_t::result_type generate()
            {
                RNGType_t::result_type result = mDistribution(mRandomNumberGenerator);
                return result;
            }

        private:
            RNGType_t          mRandomNumberGenerator;
            DistributionType_t mDistribution;
        };

    }
}

#endif
