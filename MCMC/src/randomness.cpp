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
 * @brief Keep track of last x-length of lattice. This guards accidental
 * re-initialisation (with same seed value).
 */
static int _last_Lx = -1;
/**
 * @brief Keep track of last y-length of lattice. This guards accidental
 * re-initialisation (with same seed value).
 */
static int _last_Ly = -1;
/**
 * @brief Keep track of last z-length of lattice. This guards accidental
 * re-initialisation (with same seed value).
 */
static int _last_Lz = -1;

void init_iRNG(pos const& L) {
	uSize N = (uSize)L.x * L.y * L.z;
	iDist = std::uniform_int_distribution<uSize>(0, N - 1);
	iRNG = std::default_random_engine(N);
	_last_Lx = L.x;
	_last_Ly = L.y;
	_last_Lz = L.z;
}

float rProbability() {
	return pDist(pRNG);
}

uSize rIndex(vec3<uIndx> const& L) {
	if (L.x != _last_Lx || L.y != _last_Ly || L.z != _last_Lz)
		init_iRNG(L);
	return iDist(iRNG);
}
