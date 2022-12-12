#pragma once
#include <random>

#include "defaults.hpp"

/**
 * @brief Converts boolean representation of spin (`true` for up `false` for down)
 * to the spin representation in Ising model.
 * 
 * @param s The spin value: `true` for up and `false` for down.
 * @return float `+1.0` for up and `-1.0` for down.
 */
float bool2spin(bool s);
/**
 * @brief Converts sum of spins in boolean representation to integer representation.
 * 
 * @param S The sum of booleans.
 * @param n Number of spins that have been summed.
 * @return float 
 */
float bool2spin(uSize S, uSize n);
/**
 * @brief Converts sum of spins in boolean representation to integer representation.
 * 
 * @param J The sum of booleans multiplied by some float values.
 * @param n Number of spins that have been summed.
 * @return float 
 */
float bool2spin(float J, float n);

/**
 * @brief Gets the bit value (1 or 0) `a` positions from the end (LSB is treated
 * as position 1).
 * 
 * @param number 
 * @param a 
 * @return true 
 * @return false 
 */
bool bitFromEnd(uWord number, uint a);

/**
 * @brief Gets the bit value (1 or 0) `a` positions from the beginning (MSB is
 * treated as position 0).
 * 
 * @param number 
 * @param a 
 * @return true 
 * @return false 
 */
bool bitFromBeg(uWord number, uint a);

/**
 * @brief Change the value of a bit at given position.
 * 
 * @param number The number in which the bit is to be modified.
 * @param a The position of the bit from the beginning (0 indexing).
 */
void flipBit(uWord* number, uint a);

/**
 * @brief Generate a random number (bit string). The probability of a bit being
 * `1` is given by p/2^(n).
 * 
 * This is the finite digit algorithm as described in the paper by
 * Watanabe et. al., J. Phys. Soc. Jpn., August 15, 2019.
 * 
 * @param density Indicates the density scaled by word size (64).
 * @param seed The seed value for the random number generator.
 * @return uint 
 */
uWord randIntP(uint density, uint seed);
