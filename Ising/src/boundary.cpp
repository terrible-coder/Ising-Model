#include "boundary.hpp"

uint imposeBC(const uint len, const int uIdx, BoundaryCondition boundary) {
	switch (boundary) {
	case BoundaryCondition::PERIODIC:
		// for periodic boundary condition the lattice points circle back
		return (uIdx + len) % len;
	case BoundaryCondition::FREE:
		// for free boundary condition the edges are sharply defined
		// the lattice points on the edges see no neighbour
		return (uIdx < 0 || (uint)uIdx >= len)? len : uIdx;
	default:
		std::cout << "Unknown boundary condition.";
		return -1;
	}
}

void imposeBC(const vec3<uIndx>& len, const vec3<int>& uIdx, vec3<int>* aIdx, vec3<BoundaryCondition>& boundary) {
	aIdx->x = imposeBC(len.x, uIdx.x, boundary.x);
	aIdx->y = imposeBC(len.y, uIdx.y, boundary.y);
	aIdx->z = imposeBC(len.z, uIdx.z, boundary.z);
}

uSize idx3to1(pos& p, vec3<uIndx>& L) {
	uSize x = (uSize) p.x;
	uSize y = (uSize) p.y;
	uSize z = (uSize) p.z;
	return z * L.x*L.y + y * L.x + x;
}

void idx1to3(uSize idx, vec3<uIndx>& L, pos* p) {
	uSize _2D_size = (uSize) L.x * L.y;
	p->z = idx / _2D_size;
	p->y = (idx % _2D_size) / L.x;
	p->x = (idx % _2D_size) % L.x;
}

bool outOfBounds(vec3<uIndx>& L, pos& idx) {
	return (idx.x == L.x) || (idx.y == L.y) || (idx.z == L.z);
}
