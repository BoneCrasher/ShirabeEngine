#ifndef __SHIRABE_RANDOM_H__
#define __SHIRABE_RANDOM_H__

#include <random>

#include "Core/UID.h"

namespace Engine {
	namespace Random {
    using namespace Engine::Core;

		class RandomState 
      : public IUIDGenerator<std::mt19937_64::result_type>
    {
		public:
			typedef std::mt19937_64                                      rng_type;
			typedef std::uniform_int_distribution<rng_type::result_type> dist_type;

			inline RandomState() {
				m_randomNumberGenerator.seed(std::random_device()());
			}

			inline rng_type::result_type generate() { return m_distribution(m_randomNumberGenerator); }

		private:
			rng_type  m_randomNumberGenerator;
			dist_type m_distribution;
		};

	}
}

#endif