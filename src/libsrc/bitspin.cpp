#include "bitspin.hpp"

/**
 * @brief Converts boolean representation of spin (`true` for up `false` for down)
 * to the spin representation in Ising model.
 * 
 * @param s The spin value: `true` for up and `false` for down.
 * @return double `+1.0` for up and `-1.0` for down.
 */
double bool2spin(bool s) {
	return 2.*s - 1.;
}
/**
 * @brief Converts sum of spins in boolean representation to integer representation.
 * 
 * @param S The sum of booleans.
 * @param n Number of spins that have been summed.
 * @return double 
 */
double bool2spin(uint S, int n) {
	return 2.*S - n;
}

bool bitFromEnd(uWord_t number, uint a) {
	return (bool) (number >> (a - 1)) & 1;
}

bool bitFromBed(uWord_t number, uint a) {
	return (bool) (number >> (WORD_SIZE - a - 1)) & 1;
}
