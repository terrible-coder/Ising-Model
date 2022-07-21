#include <iostream>
#include <fstream>
#include <string.h>

enum TransProb {
	BOLTZMANN = 'b',
	GLAUBER		= 'g'
};

enum Dynamics {
	FLIP = false,
	EXCHANGE = true
};

struct Specifications {
	int ENSEMBLE_SIZE;

	int Lx, Ly;
	int scale;
	double* Temperature;
	int _t_points;
	double BoltzConstant;
	double Coupling;
	double Field;

	TransProb Transition;
	Dynamics SpinKinetics;
};

void init_system(std::string filename, Specifications* S);
