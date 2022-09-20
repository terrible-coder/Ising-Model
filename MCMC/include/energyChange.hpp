#include <list>
#include <cmath>

#include "transition.hpp"
#include "Ising.hpp"

/**
 * @brief Store data about a potential exchange. It stores the position of the
 * spins to exchange and the transition rate.
 * 
 */
struct PotEx {
	int i1, j1;
	int i2, j2;
	double delE;
	double transition;
	double normP;
};

/**
 * @brief Create a Pot Ex object.
 * 
 * @param i1 
 * @param j1 
 * @param i2 
 * @param j2 
 * @param dE 
 * @param ctx 
 * @return struct PotEx* 
 */
PotEx* createPotEx(Ising& config, int i1, int j1, int i2, int j2, Context* ctx);

/**
 * @brief Calculates the change in energy (in units of J) for flipping of
 * single spin.
 * 
 * @param c 
 * @param i 
 * @param j 
 * @return double 
 */
double flipChange(Ising& c, uint i, uint j);

/**
 * @brief Calculates the change in energy (in units of J) for exchanging two
 * neighbouring spins.
 * 
 * @param c 
 * @param i1 
 * @param j1 
 * @param i2 
 * @param j2 
 * @return double 
 */
double exchangeChange(Ising& c, int i1, int j1, int i2, int j2);
