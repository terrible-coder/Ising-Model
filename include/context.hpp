#include <cstdint>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <functional>

#include "defaults.hpp"

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

template<typename T>
inline bool operator==(vec3<T> const& lhs, vec3<T> const& rhs) {
	return (lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.z == rhs.z);
}

template<typename T>
inline vec3<T> operator+(vec3<T> const& lhs, vec3<T> const& rhs) {
	return {lhs.x+rhs.x, lhs.y+rhs.y, lhs.z+rhs.z};
}

template<typename T, typename U>
inline vec3<int> operator+(vec3<T> const& lhs, vec3<U> const& rhs) {
	return {lhs.x+rhs.x, lhs.y+rhs.y, lhs.z+rhs.z};
}

template<typename T, typename U>
inline vec3<int> operator-(vec3<T> const& lhs, vec3<U> const& rhs) {
	return {lhs.x-rhs.x, lhs.y-rhs.y, lhs.z-rhs.z};
}

template<typename T>
inline vec3<int> toIntVec(vec3<T> const& a) {
	return {(int) a.x, (int) a.y, (int) a.z};
}

template<typename T>
inline vec3<T> toTVec(vec3<int> const& a) {
	return {(T) a.x, (T) a.y, (T) a.z};
}

typedef vec3<uIndx> pos;

// template<typename T>

struct Context {
	std::string saveDir;    // Directory to save all data to.
	uint Run;    // The total number of monte carlo cycles to run

	float BoltzConstant;    // The numerical value of the Boltzmann factor.
	vec3<float> interact;   // Energy interaction between types A-B

	vec3<uIndx> size;				// The dimensions of the lattice.
	vec3<BoundaryCondition> boundary;	// Boundary conditions
	std::vector<std::string> surfLocs; // Location of surfaces
	std::vector<vec3<float>> surfInts; // Interaction values at surfaces

	std::vector<float> Temperature;	// The temperature range of the simulation.
	uIndx EnsembleSize;		// Number of ensembles to consider.

	uIndx Concentration;		// Concentration in units of 1/WORD_SIZE
	TransProb Transition;		// The form of the transition rate to use.
	Dynamics SpinKinetics;	// The kind of kinetics to be used.
	std::vector<std::string> DataLogs;
};
