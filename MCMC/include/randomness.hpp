#include <random>

/**
 * @brief Seed the random number generator for the indices.
 * 
 * @param w The width of the lattice to index.
 * @param h The height of the lattice to index.
 */
void init_iRNG(int w, int h);

/**
 * @brief A random number in range [0, 1) for probability purposes.
 * 
 * @return double 
 */
double rProbability();

/**
 * @brief A random integer in range `[0, w*h)` for indexing purposes.
 * 
 * @param w The width of the lattice to index.
 * @param h The height of the lattice to index.
 * @return int 
 */
int rIndex(int w, int h);
