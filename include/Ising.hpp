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

	bool is_generated;
	std::uint16_t Lx, Ly;
	std::uint16_t N;
	uint conc;
	std::uint16_t rawX, rawY;
	std::uint16_t rawN;
	double T;
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

	void flip(uint i, uint j);
	void exchange(uint i1, uint j1, uint i2, uint j2);

	double Hamiltonian();

	double Magnetisation();
};
