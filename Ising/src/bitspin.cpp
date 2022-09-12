#include "bitspin.hpp"

double bool2spin(bool s) {
	return 2.*s - 1.;
}
double bool2spin(uint S, int n) {
	/* This comes from applying the single bool2spin function on a sum of spins
	 * (2s_1 - 1) + 2(s_2 - 1) + ... + 2(s_n - 1) = 2(s_1 + s_2 + ... + s_n) - n
	 */
	return 2.*S - n;
}

bool bitFromEnd(uWord_t number, uint a) {
	return (bool) (number >> (a - 1)) & 1;
}

bool bitFromBeg(uWord_t number, uint a) {
	return (bool) (number >> (WORD_SIZE - a - 1)) & 1;
}

void flipBit(uWord_t* number, uint a) {
	/* 
	 * A XOR 1 = A'
	 * A XOR 0 = A
	 */
	uWord_t mask = ((uWord_t)1) << (WORD_SIZE - a - 1);
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

uint randIntP(uint density, uint seed) {
	// we are working with fraction (in binary), trailing zeroes don't matter
	uint n = trimTrailingZeros(&density);
	std::default_random_engine rng(seed);
	std::uniform_int_distribution<uWord_t> dist(0, ~((uWord_t)0));

	/* Assumption: In a random number each bit is set (1) with probability 0.5
	 * X1 AND X2 : each bit is set with probability p1.p2
	 * X1 OR X2  : each bit is set with probability 1 - (1-p1)(1-p2)
	 * Starting from LSB of target density, we perform OR for every `1` in density
	 * and perform AND for every `0` in density.
	 */
	uWord_t yk = dist(rng);
	uWord_t xk;
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
