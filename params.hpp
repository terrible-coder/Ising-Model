#include <iostream>
#include <string.h>
#include <fstream>

struct params {
	double kB, T;
	double J, H;
	int width, height;
	int scale;
	int boundary;
};

params readInput(std::string);
