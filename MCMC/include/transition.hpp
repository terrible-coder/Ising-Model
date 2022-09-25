#include <cmath>

#include "context.hpp"

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

/**
 * @brief The probability value for accept-reject algorithm. The type of
 * probability distribution to choose from is read from the system settings.
 * 
 * @param dE The energy change.
 * @param BETA 1/(kT)
 * @param ctx The system settings.
 * @return double 
 */
double Probability(double dE, const double BETA, Context* ctx);
