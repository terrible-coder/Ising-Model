#include "boundary.hpp"

void imposeBC(int w, int h, int ui, int uj, int* ai, int* aj, BoundaryCondition boundary) {
	switch (boundary) {
		case BoundaryCondition::PERIODIC:
			*ai = (ui + h) % h;			// For periodic boundary condition
			*aj = (uj + w) % w;			// the lattice is lives on a torus
			break;

		case BoundaryCondition::SCREW:
			*ai = (ui + h) % h;					// The lattices sites are on a
			if (uj >= h || uj < 0) {		// single string. They wrap from
				*aj = (uj + w) % w;				// end of a row to the start of
				*ai += uj >= w ? 1 : -1;	// next row
			}
			break;

		case BoundaryCondition::FREE:
			*ai = (ui >= h || ui < 0) ? -1 : ui;		// For free edge boundary
			*aj = (uj >= w || uj < 0) ? -1 : uj;		// the site sees nothing
			break;

		default:
			std::cout << "Unknown boundary condition" << std::endl;
	}
}

uint idx2to1(uint i, uint j, uint w) {
	return i * w + j;
}

void idx1to2(uint idx, uint w, uint* i, uint* j) {
	*i = idx / w;
	*j = idx % w;
}
