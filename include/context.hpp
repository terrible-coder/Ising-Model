#include <cstdint>
#include <iostream>
#include <fstream>
#include <string.h>

#ifndef CONTEXT_HPP
#define CONTEXT_HPP

enum TransProb {
	BOLTZMANN = 'b',
	GLAUBER		= 'g'
};

enum Dynamics {
	FLIP = false,
	EXCHANGE = true
};

struct Context {
	int ENSEMBLE_SIZE;

	std::uint16_t Lx, Ly;
	double* Temperature;
	int _t_points;
	double BoltzConstant;
	double Coupling;
	double Field;

	TransProb Transition;
	Dynamics SpinKinetics;
};

void init_system(std::string filename, Context* ctx);

#endif
