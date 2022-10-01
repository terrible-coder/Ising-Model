#pragma once

#include <random>
#include <filesystem>
#include <fstream>
#include <bit>

#include "bitspin.hpp"
#include "boundary.hpp"
#include "context.hpp"

class Ising {
private:
	static std::function<double(const pos& i, const pos& j)> J; // Coupling constant
	static double H;		// The external magnetic field.

	bool is_generated;	// Guard for generation of initial configuration.
	vec3<uIndx> L;			// The size of the physical lattice.
	uSize N;						// Total number of spins in the system.

	uIndx conc;					// Concentration of "up" spins, scaled by `WORD_SIZE`.
	vec3<uIndx> raw;		// The size of the array in memory which stores the spins.
	uIndx rawN;					// Total number of values needed to represent all spins.
	double T;						// The temperature of the ensemble.
	uWord* initial;			// The initial condition. Must create before simulation.
	uWord* lattice;			// The current configuration of the lattice.
	vec3<BoundaryCondition> boundary; // The boundary conditions.

public:

	Ising(vec3<uIndx>& size, uIndx conc,
				double temperature,
				const vec3<BoundaryCondition>& b);
	~Ising();

	static void setCoupling(std::function<double(const pos& i, const pos& j)>& coupling);
	static double getNNCoup(const pos& i, const pos& j);
	static void setField(double field);
	static double getField();

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
	std::uint32_t getSize();
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
	void flip(uint x, uint y, uint z);
	/**
	 * @brief Flip the spin at the given position.
	 * 
	 * @param p 
	 */
	void flip(pos p);

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
	void exchange(uIndx x1, uIndx y1, uIndx z1, uIndx x2, uIndx y2, uIndx z2);
	/**
	 * @brief Exchange the spins at the given positions.
	 * 
	 * @param p1 
	 * @param p2 
	 */
	void exchange(pos p1, pos p2);

	/**
	 * @brief Computes the Ising hamiltonian for a given configuration.
	 * 
	 * @return double 
	 */
	double Hamiltonian();

	/**
	 * @brief Calculate the magnetisation of the configuration.
	 * 
	 * @return double 
	 */
	double Magnetisation();
};
