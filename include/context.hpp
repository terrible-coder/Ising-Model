#include <cstdint>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <functional>

#pragma once

enum BoundaryCondition {
	PERIODIC = 'p',
	FREE = 'f'
};

/**
 * @brief Transition rate. Support for the Boltzmann form and the Suzuki-Kubo
 * form.
 */
enum TransProb {
	BOLTZMANN = 'b',
	SUZU_KUBO = 's'
};

/**
 * @brief The type of kinetics followed by the simulation. It can either be
 * flipping (non-conserved) or exchange (conserved) kinetics.
 * 
 */
enum Dynamics {
	FLIP = false,
	EXCHANGE = true
};

/**
 * @brief Stores a list of 3 values.
 * 
 * @tparam T 
 */
template<typename T>
struct vec3 {
	T x, y, z;
};

typedef vec3<uIndx> pos;

// template<typename T>

struct Context {
	uIndx Ensemble_Size;		// Number of ensembles to consider.
	vec3<uIndx> size;				// The dimensions of the lattice.
	uIndx Concentration;		// Concentration in units of 1/WORD_SIZE
	vec3<BoundaryCondition> boundary;	// Boundary conditions
	std::vector<double> Temperature;	// The temperature range of the simulation.
	double BoltzConstant;		// The numerical value of the Boltzmann factor.
	std::function<double(pos& i, pos& j)> Coupling;	// The coupling constant.
	double Field;						// The external field applied.
	TransProb Transition;		// The form of the transition rate to use.
	Dynamics SpinKinetics;	// The kind of kinetics to be used.
	std::string saveDir;	// Directory to save all data to.
};

/**
 * @brief Initialise the simulation. The context settings are read from a file
 * containing key-value pairs representing the values to be used.
 * 
 * @param filename 
 * @param ctx 
 */
void init_system(std::string filename, Context* ctx);
