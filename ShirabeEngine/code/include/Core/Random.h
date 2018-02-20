#ifndef __SHIRABE_RANDOM_H__
#define __SHIRABE_RANDOM_H__

#include <random>


namespace Engine {
	namespace Random {

		class RandomState {
		public:
			typedef std::mt19937_64                                      rng_type;
			typedef std::uniform_int_distribution<rng_type::result_type> dist_type;

			inline RandomState() {
				m_randomNumberGenerator.seed(std::random_device()());
			}

			inline rng_type::result_type next() { return m_distribution(m_randomNumberGenerator); }

		private:
			rng_type  m_randomNumberGenerator;
			dist_type m_distribution;
		};

	}
}

#endif