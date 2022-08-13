#pragma once

#include <random>
#include <filesystem>
#include <fstream>
#include <bit>

#include "defaults.hpp"
#include "boundary.hpp"

double bool2spin(bool);
double bool2spin(uint, int);

class Ising {
private:
	static double J, H;
	static bool _setJ, _setH;

	bool is_generated;
	std::uint16_t Lx, Ly;
	std::uint16_t N;
	double T;
	uWord_t* initial;
	uWord_t* lattice;
	BoundaryCondition boundary;

public:

	Ising(uint w, uint h,
				double temperature,
				BoundaryCondition b);
	~Ising();

	static void setCoupling(double coupling);
	static double getNNCoup();
	static void setField(double field);
	static double getField();

	bool operator() (uint i, uint j);

	std::uint16_t getHeight();
	std::uint16_t getWidth();
	std::uint16_t getSize();
	double getTemp();

	void generate();
	void reinit();

	void flip(uint i, uint j);
	void exchange(uint i1, uint j1, uint i2, uint j2);

	double Hamiltonian();

	double Magnetisation();
};
