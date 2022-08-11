#pragma once

#include <cstdint>
#include <iostream>
#include <string>
#include <random>
#include <filesystem>
#include <fstream>

#include "boundary.hpp"

double bool2spin(bool);
double bool2spin(int, int);

class Ising {
private:
	static double J, H;
	static bool _setJ, _setH;

	bool is_generated;
	std::uint16_t Lx, Ly;
	std::uint16_t N;
	double T;
	bool** initial;
	bool** lattice;
	BoundaryCondition boundary;

public:

	Ising(int w, int h,
				double temperature,
				BoundaryCondition b);
	~Ising();

	static void setCoupling(double coupling);
	static double getNNCoup();
	static void setField(double field);
	static double getField();

	bool operator() (int i, int j);

	std::uint16_t getHeight();
	std::uint16_t getWidth();
	std::uint16_t getSize();
	double getTemp();

	void generate();
	void reinit();

	void flip(int i, int j);
	void exchange(int i1, int j1, int i2, int j2);

	double Hamiltonian();

	double Magnetisation();
};
