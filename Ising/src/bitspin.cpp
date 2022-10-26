#include "bitspin.hpp"

float bool2spin(bool s) {
	return 2.*s - 1.;
}
float bool2spin(uSize S, uSize n) {
	/* This comes from applying the single bool2spin function on a sum of spins
	 * (2s_1 - 1) + 2(s_2 - 1) + ... + 2(s_n - 1) = 2(s_1 + s_2 + ... + s_n) - n
	 */
	return 2.f * S - n;
}
float bool2spin(float J, float n) {
	return 2.f * J - n;
}

bool bitFromEnd(uWord number, uint a) {
	return (bool) (number >> (a - 1)) & 1;
}

bool bitFromBeg(uWord number, uint a) {
	return (bool) (number >> (WORD_SIZE - a - 1)) & 1;
}

void flipBit(uWord* number, uint a) {
	/* 
	 * A XOR 1 = A'
	 * A XOR 0 = A
	 */
	uWord mask = ((uWord)1) << (WORD_SIZE - a - 1);
	*number ^= mask;
}

/**
 * @brief Removes the zeros at the end of a binary number.
 * 
 * @param n 
 * @return uint Number of binary positions left
 */
uint trimTrailingZeros(uint* n) {
	uint k = 6; // This is hard coded for now, needs to be changed later
	while ((*n & 1) == 0) {
		*n >>= 1;
		k--;
	}
	return k;
}

uint _last_seed = -1;
uWord randIntP(uint density, uint seed) {
	// we are working with fraction (in binary), trailing zeroes don't matter
	uint n = trimTrailingZeros(&density);
	static std::default_random_engine rng;
	if (seed != _last_seed) {
		rng.seed(seed);
		_last_seed = seed;
	}
	static std::uniform_int_distribution<uWord> dist(0, ~((uWord)0));

	/* Assumption: In a random number each bit is set (1) with probability 0.5
	 * X1 AND X2 : each bit is set with probability p1.p2
	 * X1 OR X2  : each bit is set with probability 1 - (1-p1)(1-p2)
	 * Starting from LSB of target density, we perform OR for every `1` in density
	 * and perform AND for every `0` in density.
	 */
	uWord yk = dist(rng);
	uWord xk;
	for (uint j = 1; j < n; j++) {
		density = density >> 1;
		xk = dist(rng);
		if ((density & 1))
			yk = yk | xk;
		else
			yk = yk & xk;
	}
	return yk;
}
