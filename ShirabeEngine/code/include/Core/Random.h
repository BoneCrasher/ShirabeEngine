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
				_randomNumberGenerator.seed(std::random_device()());
			}

			inline rng_type::result_type next() { return _distribution(_randomNumberGenerator); }

		private:
			rng_type  _randomNumberGenerator;
			dist_type _distribution;
		};

	}
}

#endif