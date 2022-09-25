#include <cstdint>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#pragma once

enum TransProb {
	BOLTZMANN = 'b',
	SUZU_KUBO = 's'
};

enum Dynamics {
	FLIP = false,
	EXCHANGE = true
};

struct Context {
	int ENSEMBLE_SIZE;

	std::uint16_t Lx, Ly;
	std::vector<double> Temperature;
	double BoltzConstant;
	double Coupling;
	double Field;

	TransProb Transition;
	Dynamics SpinKinetics;
};

void init_system(std::string filename, Context* ctx);
