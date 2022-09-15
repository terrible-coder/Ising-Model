#include "Ising.hpp"
#include "context.hpp"
#include "transition.hpp"

/**
 * @brief The probability value for accept-reject algorithm. The type of
 * probability distribution to choose from is read from the system settings.
 * 
 * @param dE The energy change.
 * @param BETA 1/(kT)
 * @param ctx The system settings.
 * @return double 
 */
double Probability(double, double);

/**
 * @brief Calculates the change in energy (in units of J) for flipping of
 * single spin.
 * 
 * @param c 
 * @param ctx 
 * @param i 
 * @param j 
 * @return double 
 */
double singleFlipChange(Ising& c, int i, int j);

/**
 * @brief Performs the accept-reject algorithm.
 * 
 * @param dE The energy change.
 * @param temperature The temperature.
 * @param ctx The system settings.
 * @return `true` if accepted, `false` if rejected.
 */
bool isAccepted(double dE, double temperature, Context *ctx);

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
 * @param i The random row index to check.
 * @param j The random column index to check.
 * @param specs The system settings.
 */
void spin_flip(Ising& c, int i, int j, Context* ctx);

/**
 * @brief Perform the spin-exchange dynamics algorithm.
 * The function assumes `i1 <= i2` and `j1 <= j2`.
 * 
 * @param c The Ising model configuration.
 * @param i1 The random row index of spin to exchange.
 * @param j1 The random column index of spin to exchange.
 * @param i2 The random row index of spin to exchange with.
 * @param j2 The random column index of spin to exchange with.
 * @param ctx 
 * @param success
 */
void spin_exchange(Ising& c, int i1, int j1, int i2, int j2, Context* ctx, bool* success);
