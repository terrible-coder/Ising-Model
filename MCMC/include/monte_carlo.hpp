#include <cmath>

#include "Ising.hpp"
#include "context.hpp"
#include "data_logger.hpp"
#include "save.hpp"
#include "transition.hpp"
#include "randomness.hpp"

/**
 * @brief Performs the accept-reject algorithm.
 * 
 * @param dE The energy change.
 * @param temperature The temperature.
 * @param ctx The system settings.
 * @return `true` if accepted, `false` if rejected.
 */
bool isAccepted(float dE, float temperature, Context *ctx);

/**
 * @brief Perform the dynamics of the system according to the spin-flip and
 * spin-exchange algorithm. The type of dynamics to perform (flip or exchange)
 * is read from the system settings.
 * 
 * @param config The Ising model configuration.
 * @param ctx The system settings.
 */
void dynamics(Ising& c, Context* ctx);

/**
 * @brief Perform the spin-flip dynamics algorithm.
 * 
 * @param config The Ising model configuration.
 * @param specs The system settings.
 */
void spin_flip(Ising& c, Context* ctx);

/**
 * @brief Perform the spin-exchange dynamics algorithm.
 * The function assumes `i1 <= i2` and `j1 <= j2`.
 * 
 * @param c The Ising model configuration.
 * @param ctx 
 */
void spin_exchange(Ising& c, Context* ctx);

/**
 * @brief The driver code for monte carlo simulations. The monte carlo runs
 * are defined here. This method is also responsible for passing the programme
 * control to the appropriate functions to handle the dynamics (conserved or
 * non-conserved) of the system.
 * 
 * @param c 
 * @param ctx 
 * @param en 
 */
void MonteCarlo(Ising& c, Context* ctx, uIndx en);
