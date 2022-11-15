#include "randomness.hpp"

/**
 * @brief Seed value for generation of random values in range [0, 1).
 */
const int pSEED = 20;

/**
 * @brief Uniform probability distribution to sample from for accept-reject purposes.
 */
static std::uniform_real_distribution<float> pDist(0, 1);
/**
 * @brief Random number generator which generates values in range [0, 1).
 */
static std::default_random_engine pRNG(pSEED);

/**
 * @brief Uniform distribution to sample from for selecting random indices in
 * lattice.
 */
static std::uniform_int_distribution<uSize> iDist;

/**
 * @brief Random number generator which generates random index value.
 * 
 * Note: The generator assumes a row major storage of lattice data. The row
 * and column indices can be calculated from a single value returned by this
 * generator.
 * 
 */
static std::default_random_engine iRNG;

/**
 * @brief Keep track of last length of lattice. This guards accidental
 * re-initialisation (with same seed value).
 */
static vec3<uIndx> _lastL = {0, 0, 0};

void init_iRNG(pos const& L) {
	std::cout << "\tiRNG initiated\t";
	uSize N = (uSize)L.x * L.y * L.z;
	iDist = std::uniform_int_distribution<uSize>(0, N - 1);
	iRNG = std::default_random_engine(N);
	_lastL = L;
	std::cout << "last L: " << _lastL.x << "," << _lastL.y << "," << _lastL.z << std::endl;
}

float rProbability() {
	return pDist(pRNG);
}

uSize rIndex(vec3<uIndx> const& L, bool noInit) {
	if (!(L == _lastL) && !noInit)
		init_iRNG(L);
	if (noInit) {
		std::uniform_int_distribution<uSize> tmpDist(0, L.x-1);
		return tmpDist(iRNG);
	}
	return iDist(iRNG);
}

uSize rIndex(vec3<uIndx> const& L) {
	return rIndex(L, false);
}
