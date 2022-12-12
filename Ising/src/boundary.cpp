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

void imposeBC(vec3<uIndx> const& len, vec3<int> const& uIdx, vec3<uIndx>* aIdx, vec3<BoundaryCondition> const& boundary) {
	aIdx->x = imposeBC(len.x, uIdx.x, boundary.x);
	aIdx->y = imposeBC(len.y, uIdx.y, boundary.y);
	aIdx->z = imposeBC(len.z, uIdx.z, boundary.z);
}

uSize idx3to1(pos const& p, vec3<uIndx> const& L) {
	uSize x = (uSize) p.x;
	uSize y = (uSize) p.y;
	uSize z = (uSize) p.z;
	return z * L.x*L.y + y * L.x + x;
}

void idx1to3(uSize idx, vec3<uIndx> const& L, pos* p) {
	uSize _2D_size = (uSize) L.x * L.y;
	p->z = idx / _2D_size;
	p->y = (idx % _2D_size) / L.x;
	p->x = (idx % _2D_size) % L.x;
}

bool outOfBounds(vec3<uIndx> const& L, pos const& idx) {
	return (idx.x == L.x) || (idx.y == L.y) || (idx.z == L.z);
}
