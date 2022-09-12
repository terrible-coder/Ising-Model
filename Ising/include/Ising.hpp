#pragma once

#include <random>
#include <filesystem>
#include <fstream>
#include <bit>

#include "bitspin.hpp"
#include "boundary.hpp"

class Ising {
private:
	static double J, H;
	static bool _setJ, _setH;

	/**
	 * @brief Guard for generation of initial configuration.
	 */
	bool is_generated;
	/**
	 * @brief The size of the physical lattice.
	 */
	std::uint16_t Lx, Ly;
	/**
	 * @brief Total number of spins in the system.
	 */
	std::uint16_t N;
	/**
	 * @brief Concentration of "up" spins, scaled by `WORD_SIZE`.
	 */
	uint conc;
	/**
	 * @brief The size of the array in memory which stores the spins.
	 */
	std::uint16_t rawX, rawY;
	/**
	 * @brief Total number of values needed to represent all spins.
	 */
	std::uint16_t rawN;
	/**
	 * @brief The temperature of the ensemble.
	 */
	double T;
	/**
	 * @brief The initial configuration. This must be created before performing
	 * any simulations.
	 */
	uWord_t* initial;
	uWord_t* lattice;
	BoundaryCondition boundary;

public:

	Ising(uint w, uint h, uint conc,
				double temperature,
				BoundaryCondition b);
	~Ising();

	static void setCoupling(double coupling);
	static double getNNCoup();
	static void setField(double field);
	static double getField();

	/**
	 * @brief Lattice point accessor. The index is of the site we "want" to look at.
	 * The function takes care of the appropriate boundary conditions and returns
	 * the spin value at the actual index in the grid. The function can return `NULL`
	 * if the spin is supposed to be interpreted as absent at `(i, j)`.
	 * 
	 * @param i The row index we "want" to look at.
	 * @param j The column index we "want" to look at.
	 * @return true 
	 * @return false 
	 */
	bool operator() (uint i, uint j);

	std::uint16_t getHeight();
	std::uint16_t getWidth();
	std::uint16_t getSize();
	double getTemp();
	uWord_t* getInit();
	uWord_t* getRaw();
	void __leftShift(uWord_t* shifted);
	void __downShift(uWord_t* shifted);

	void generate();
	void reinit();

	/**
	 * @brief Flip the spin at the given index.
	 * 
	 * @param i The row index.
	 * @param j The column index.
	 */
	void flip(uint i, uint j);

	/**
	 * @brief Exchange the spins at the given indices.
	 * 
	 * @param i1 Row index 1.
	 * @param j1 Column index 1.
	 * @param i2 Row index 2.
	 * @param j2 Column index 2.
	 */
	void exchange(uint i1, uint j1, uint i2, uint j2);

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
