#include <iostream>

enum BoundaryCondition {
	PERIODIC = 0,
	SCREW,
	FREE
};

void imposeBC(int w, int h, int ui, int uj, int* ai, int* aj, BoundaryCondition boundary);
