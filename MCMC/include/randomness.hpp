#include <random>

#include "context.hpp"

/**
 * @brief Seed the random number generator for the indices.
 * 
 * @param N Size of the lattice to index.
 */
void init_iRNG(vec3<uIndx> const& L);

/**
 * @brief A random number in range [0, 1) for probability purposes.
 * 
 * @return float 
 */
float rProbability();

/**
 * @brief A random integer in range `[0, N)` for indexing purposes.
 * 
 * @param L The dimensions of the lattice to index.
 * @return int 
 */
uSize rIndex(vec3<uIndx> const& L);
