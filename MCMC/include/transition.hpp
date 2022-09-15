#include <cmath>

/**
 * @brief The Boltzmann probability distribution.
 * 
 * @param dE The energy change.
 * @param BETA 1/(kT)
 * @return double 
 */
double Boltzmann(double dE, double BETA);

/**
 * @brief The Suzuki-Kubo probability distribution.
 * 
 * @param dE The energy change.
 * @param BETA 1/(kT)
 * @return double 
 */
double Suzuki_Kubo(double dE, double BETA);
