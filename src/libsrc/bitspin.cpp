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

/**
 * @brief Change the value of a bit at given position.
 * 
 * @param number The number in which the bit is to be modified.
 * @param a The position of the bit from the beginning (0 indexing).
 * @param bit The value of the updated bit.
 */
void changeBit(uWord_t* number, uint a, bool bit) {
	uWord_t mask = ((uWord_t)1) << (WORD_SIZE - a - 1);
	*number = (*number & ~mask) | (mask * bit);
}
