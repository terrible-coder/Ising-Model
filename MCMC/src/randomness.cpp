#include "randomness.hpp"

/**
 * @brief Seed value for generation of random values in range [0, 1).
 */
const int pSEED = 20;

/**
 * @brief Uniform probability distribution to sample from for accept-reject purposes.
 */
static std::uniform_real_distribution<double> pDist(0, 1);
/**
 * @brief Random number generator which generates values in range [0, 1).
 */
static std::default_random_engine pRNG(pSEED);

/**
 * @brief Uniform distribution to sample from for selecting random indices in
 * lattice.
 */
static std::uniform_int_distribution<int> iDist;

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
 * @brief Keep track of last width of lattice. This guards accidental
 * re-initialisation (with same seed value).
 */
static int _last_w = -1;
/**
 * @brief Keep track of last height of lattice. This guards accidental
 * re-initialisation (with same seed value).
 */
static int _last_h = -1;

void init_iRNG(int w, int h) {
	iDist = std::uniform_int_distribution<int>(0, w*h - 1);
	iRNG = std::default_random_engine(w * h);
	_last_w = w;
	_last_h = h;
}

double rProbability() {
	return pDist(pRNG);
}

int rIndex(int w, int h) {
	if (w != _last_w || h != _last_h)
		init_iRNG(w, h);
	return iDist(iRNG);
}
