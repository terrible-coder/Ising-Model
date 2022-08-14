#include <iostream>

enum BoundaryCondition {
	PERIODIC = 0,
	SCREW,
	FREE
};

void imposeBC(int w, int h, int ui, int uj, int* ai, int* aj, BoundaryCondition boundary);

uint idx2to1(uint i, uint j, uint w);
void idx1to2(uint idx, uint w, uint* i, uint* j);
