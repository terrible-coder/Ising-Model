#pragma once

#include <random>
#include <filesystem>
#include <fstream>
#include <bit>
#include <functional>
#include <vector>

#include "bitspin.hpp"
#include "boundary.hpp"
#include "context.hpp"

/**
 * @brief Enumerates the edges of the lattice.
 */
enum Edge {
	NONE = 0,
	X_BEG = 1, X_END = 2,
	Y_BEG = 4, Y_END = 8,
	Z_BEG = 16, Z_END = 32
};

float coupling(float aa, float bb, float ab);
float field(float aa, float bb);

/**
 * @brief Find the edge on which the lattice point is on.
 * 
 * @param i The lattice point to check.
 * @param s The dimensions of the lattice.
 * @return Edge 
 */
Edge onEdge(pos const& i, vec3<uIndx>& s);

/*
At some later point the Surface struct maybe modified to allow non-constant
values at the edge. That would require inheritance. 
*/

/**
 * @brief Represents a surface at one of the edges of the lattice.
 */
struct Surface {
	Edge loc;   // location of the surface on the lattice

	float Eaa;	// interaction energy between A-A particles at surface
	float Ebb;	// interaction energy between B-B particles at surface
	float Eab;	// interaction energy between A-B particles at surface
	bool __static_surf;
};

/*
Along with non-constant values at edges, the bulk values would also be allowed
to take non-constant functions.
*/

/**
 * @brief Deals with the parameters of the simulation system.
 */
class ModelParams {
public:
	vec3<uIndx> L;                     // Dimensions of the system.
	uSize N;                           // Total number of lattice points.
	uIndx q;                           // Coordination of lattice.
	vec3<BoundaryCondition> boundary;  // The boundary conditions.

	float Eaa;	// "bulk" interaction energy between A-A particles
	float Ebb;	// "bulk" interaction energy between B-B particles
	float Eab;	// "bulk" interaction energy between A-B particles
	bool __static_bulk;

	std::vector<Surface> surfaces;

	ModelParams();
	/**
	 * @brief Construct a new Model Params object.
	 * 
	 * @param size 
	 * @param bc 
	 */
	ModelParams(vec3<uIndx>& size, vec3<BoundaryCondition>& bc);

	/**
	 * @brief Set the interaction energy values.
	 * 
	 * @param aa 
	 * @param bb 
	 * @param ab 
	 */
	void setInteractions(float aa, float bb, float ab);

	/**
	 * @brief Introduce a surface at one of the edges.
	 */
	void create_surface(Surface& s);
	void create_surface(Edge loc, float aa, float bb, float ab);

	/**
	 * @brief Check if a given edge has a surface.
	 * 
	 * @param e 
	 * @return true 
	 * @return false 
	 */
	bool isSurface(Edge e);

	std::vector<Surface>::iterator whichSurface(Edge e);

	/**
	 * @brief The interaction energy between two lattice points.
	 * 
	 * @param i Lattice point index.
	 * @param j Lattice point index.
	 * @return float 
	 */
	float J(pos& i, pos& j);

	/**
	 * @brief The "bulk" magnetic field energy.
	 * 
	 * @param i Lattice point index.
	 * @return float 
	 */
	float H(pos& i);
};

class Ising {
private:
	bool is_generated;	// Guard for generation of initial configuration.

	uIndx conc;         // Concentration of "up" spins, scaled by `WORD_SIZE`.
	ModelParams p;      // Parameters of the problem.
	float T;            // Temperature of the system.
	uWord* initial;     // The initial condition. Must create before simulation.
	uWord* lattice;     // The current configuration of the lattice.
	vec3<uIndx> raw;    // The size of the array in memory which stores the spins.
	uIndx rawN;         // Total number of values needed to represent all spins.

	float __M;    // Cheat for constant magnetisation.

	float partialEnergy(uWord* shifted, uSize beg, vec3<int> off);
	/**
	 * @brief Calculate sum of the neighbours.
	 * 
	 * @param i The lattice point whose neighbours to consider.
	 * @param off The offset indicating the direction to sum in.
	 * @param e The edge on which the lattice point `i` is on.
	 * @param P Flag to indicate if sum is only in positive direction.
	 * @param n The number of spins which have been summed over.
	 * @return uIndx 
	 */
	uIndx __sumDir(pos const& i, vec3<uIndx> const& off, Edge e, bool P, uSize* n);
	uIndx sumNeighboursP(pos const& i, vec3<uIndx> const& dir, uSize* n);

public:

	Ising(uIndx conc, ModelParams& params, float temperature);
	~Ising();

	float getNNCoup(const pos& i, const pos& j);
	float getField(const pos& i);

	/**
	 * @brief Lattice point accessor. The index is of the site we "want" to look at.
	 * The function takes care of the appropriate boundary conditions and returns
	 * the spin value at the actual index in the grid. The function can return `NULL`
	 * if the spin is supposed to be interpreted as absent at `(x, y, z)`.
	 * 
	 * @param x 
	 * @param y 
	 * @param z
	 * @return true 
	 * @return false 
	 */
	bool operator() (int x, int y, int z);
	bool operator() (vec3<int> const& i);
	bool operator() (pos const& i);

	/**
	 * @brief Convert given lattice coordinates to equivalent positive coordinates.
	 *
	 * @param x
	 * @param y
	 * @param z
	 */
	void equiv(int* x, int* y, int* z);
	/**
	 * @brief Convert given lattice coordinates to equivalent positive coordinates.
	 * 
	 * @param idx 
	 * @return pos& 
	 */
	pos& equiv(vec3<int> const& idx);

	vec3<uIndx> getVecSize();
	uIndx getSizeX();
	uIndx getSizeY();
	uIndx getSizeZ();
	uSize getSize();
	float getTemp();
	uWord* getInit();
	uWord* getRaw();

	/**
	 * @brief Returns the sum of spins of all the neighbours of the given spin.
	 * This method takes care of the boundary conditions and the dimensionality
	 * of the lattice.
	 * 
	 * @param i 
	 * @return uIndx 
	 */
	uIndx sumNeighbours(pos const& i, uSize* n);
	/**
	 * @brief Calculate the sum of spins of neighbours ONLY in the indicated
	 * directions. The direction must have unsigned integer values. This method
	 * will check in both +ve and -ve directions for the indicated axes.
	 * 
	 * @param i 
	 * @param dir 
	 * @return uIndx 
	 */
	uIndx sumNeighbours(pos const& i, vec3<uIndx> const& dir, uSize* n);
	/**
	 * @brief Calculates the sum of the spins of neighbours ONLY in the indicated
	 * directions, except the one specified. The direction must have unsigned
	 * integer values. This method takes care of the boundary conditions and the
	 * dimensionality of the lattice.
	 * 
	 * @param i 
	 * @param dir 
	 * @param except 
	 * @return uIndx 
	 */
	uIndx sumNeighbours(pos const& i, vec3<uIndx> const& dir, pos const& except, uSize* n);

	/**
	 * @brief Returns the lattice shifted towards -ve x by 1 position. Assumes
	 * PBC. Any FBC corrections must be added by user.
	 * 
	 * @param shifted 
	 */
	void __nXShift(uWord* shifted);
	/**
	 * @brief Returns the lattice shifted towards -ve y by 1 position. Assumes
	 * PBC. Any FBC corrections must be added by user.
	 * 
	 * @param shifted 
	 */
	void __nYShift(uWord* shifted);
	/**
	 * @brief Returns the lattice shifted towards -ve z by 1 position. Assumes
	 * PBC. Any FBC corrections must be added by user.
	 * 
	 * @param shifted 
	 */
	void __nZShift(uWord* shifted);

	void generate();
	void reinit();

	/**
	 * @brief Flip the spin at the given index.
	 * 
	 * @param x 
	 * @param y 
	 * @param z 
	 */
	void flip(uIndx x, uIndx y, uIndx z);
	/**
	 * @brief Flip the spin at the given position.
	 * 
	 * @param p 
	 */
	void flip(pos& p);

	/**
	 * @brief Exchange the spins at the given indices.
	 * 
	 * @param x1 
	 * @param y1 
	 * @param z1 
	 * @param x2 
	 * @param y2 
	 * @param z2 
	 */
	void exchange(int x1, int y1, int z1, int x2, int y2, int z2);
	/**
	 * @brief Exchange the spins at the given positions.
	 * 
	 * @param p1 
	 * @param p2 
	 */
	void exchange(vec3<int>& p1, vec3<int>& p2);
	/**
	 * @brief Exchange the spins at the given positions.
	 * 
	 * @param p1 
	 * @param p2 
	 */
	void exchange(pos& p1, pos& p2);

	float flipEnergyChange(pos const& i);
	float exchangeEnergyChange(pos const& i, pos const& j);

	/**
	 * @brief Computes the Ising hamiltonian for a given configuration.
	 * 
	 * @return float 
	 */
	float Hamiltonian();

	/**
	 * @brief Calculate the magnetisation of the configuration.
	 * 
	 * @return float 
	 */
	float Magnetisation();
};
