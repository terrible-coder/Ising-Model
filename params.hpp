#include <iostream>
#include <string.h>
#include <fstream>

struct params {
	double kB, T;
	double J, H;
};

params readInput(std::string);
