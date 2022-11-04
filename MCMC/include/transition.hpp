#include <cmath>

#include "context.hpp"

/**
 * @brief The Boltzmann probability distribution.
 * 
 * @param dE The energy change.
 * @param BETA 1/(kT)
 * @return float 
 */
float Boltzmann(float dE, float BETA);

/**
 * @brief The Suzuki-Kubo probability distribution.
 * 
 * @param dE The energy change.
 * @param BETA 1/(kT)
 * @return float 
 */
float Suzuki_Kubo(float dE, float BETA);

/**
 * @brief The probability value for accept-reject algorithm. The type of
 * probability distribution to choose from is read from the system settings.
 * 
 * @param dE The energy change.
 * @param BETA 1/(kT)
 * @param ctx The system settings.
 * @return float 
 */
float Probability(float dE, const float BETA, Context* ctx);
