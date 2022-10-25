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

/**
 * @brief Represents a surface at one of the edges of the lattice.
 */
struct Surface {
	Edge loc;   // location of the surface on the lattice

	float Eaa;	// interaction energy between A-A particles at surface
	float Ebb;	// interaction energy between B-B particles at surface
	float Eab;	// interaction energy between A-B particles at surface
};

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

public:

	Ising(uIndx conc, ModelParams& params, float temperature);
	~Ising();

	double getNNCoup(const pos& i, const pos& j);
	double getField(const pos& i);

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
	bool operator() (vec3<int>& p);
	bool operator() (pos& p);

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
	pos& equiv(vec3<int>& idx);

	uIndx getSizeX();
	uIndx getSizeY();
	uIndx getSizeZ();
	uSize getSize();
	double getTemp();
	uWord* getInit();
	uWord* getRaw();
	void __nXShift(uWord* shifted);
	void __nYShift(uWord* shifted);
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

	/**
	 * @brief Computes the Ising hamiltonian for a given configuration.
	 * 
	 * @return double 
	 */
	float Hamiltonian();

	/**
	 * @brief Calculate the magnetisation of the configuration.
	 * 
	 * @return double 
	 */
	float Magnetisation();
};
