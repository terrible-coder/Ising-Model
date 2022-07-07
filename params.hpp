#include <iostream>
#include <string.h>
#include <fstream>

struct params {
	double kB, T;
	double J, H;
	int width, height;
	int scale;
	int boundary;
	std::string probability;
	std::string kinetics;
};

params readInput(std::string);
