/**
 * @file accessors.cpp
 * @author terrible-coder (github.com/terrible-coder)
 * @brief This file contains the getters and setters for the Ising class.
 * @date 2022-07-24
 * 
 */

#include "Ising.hpp"

uIndx Ising::getSizeX() {
	return this->p.L.x;
}

uIndx Ising::getSizeY() {
	return this->p.L.y;
}

uIndx Ising::getSizeZ() {
	return this->p.L.z;
}

uSize Ising::getSize() {
	return this->p.N;
}

float Ising::getTemp() {
	return this->T;
}

uWord* Ising::getInit() {
	return this->initial;
}

uWord* Ising::getRaw() {
	return this->lattice;
}

bool Ising::operator() (int x, int y, int z) {
	pos a;
	imposeBC(this->p.L, {x, y, z}, &a, this->p.boundary);

	if (a.x == this->p.L.x || a.y == this->p.L.y || a.z == this->p.L.z) return NULL;
	uSize idx = idx3to1(a, this->p.L);
	return (this->lattice[idx / WORD_SIZE] >> (WORD_SIZE - idx%WORD_SIZE - 1)) & 1 ;
}

void Ising::equiv(int* x, int* y, int* z) {
	const vec3<int> uIdx{*x, *y, *z};
	pos aIdx;
	imposeBC(this->p.L, uIdx, &aIdx, this->p.boundary);
	*x = aIdx.x;
	*y = aIdx.y;
	*z = aIdx.z;
}
pos& Ising::equiv(vec3<int> const& idx) {
	pos aIdx;
	imposeBC(this->p.L, idx, &aIdx, this->p.boundary);
	return aIdx;
}

uIndx Ising::sumNeighbours(pos const& i) {
	Edge e = onEdge(i, this->p.L);
	vec3<uIndx> offX = {1, 0, 0};
	vec3<uIndx> offY = {0, 1, 0};
	vec3<uIndx> offZ = {0, 0, 1};

	switch (e) {
	case Edge::NONE:
		return   this->operator()(i+offX) + this->operator()(i-offX);
		       + this->operator()(i+offY) + this->operator()(i-offY);
		       + this->operator()(i+offZ) + this->operator()(i-offZ);

	case Edge::X_BEG:
		return   this->operator()(i+offX);
		       + this->operator()(i+offY) + this->operator()(i-offY);
		       + this->operator()(i+offZ) + this->operator()(i-offZ);

	case Edge::X_END:
		return   this->operator()(i-offX);
		       + this->operator()(i+offY) + this->operator()(i-offY);
		       + this->operator()(i+offZ) + this->operator()(i-offZ);

	case Edge::Y_BEG:
		return   this->operator()(i+offX) + this->operator()(i-offX);
		       + this->operator()(i+offY);
		       + this->operator()(i+offZ) + this->operator()(i-offZ);

	case Edge::Y_END:
		return   this->operator()(i+offX) + this->operator()(i-offX);
		       + this->operator()(i-offY);
		       + this->operator()(i+offZ) + this->operator()(i-offZ);

	case Edge::Z_BEG:
		return   this->operator()(i+offX) + this->operator()(i-offX);
		       + this->operator()(i+offY) + this->operator()(i-offY);
		       + this->operator()(i+offZ);

	case Edge::Z_END:
		return   this->operator()(i+offX) + this->operator()(i-offX);
		       + this->operator()(i+offY) + this->operator()(i-offY);
		       + this->operator()(i-offZ);
	}
}
uIndx Ising::sumNeighbours(pos const& i, pos const& except) {
	return this->sumNeighbours(i) - this->operator()(except);
}

void Ising::__nXShift(uWord* shifted) {
	bool* MSBs = new bool[this->rawN];
	for (uint idx = 0; idx < this->rawN; idx++) {
		MSBs[idx] = this->lattice[idx] >> (WORD_SIZE - 1);
		shifted[idx] = this->lattice[idx] << 1;
	}
	for (uSize idx = 0; idx < this->p.N; idx += WORD_SIZE) {
		pos xyz;
		idx1to3(idx, this->p.L, &xyz);
		imposeBC(this->p.L, vec3<int>{xyz.x-1, xyz.y, xyz.z}, &xyz, this->p.boundary);
		if (outOfBounds(this->p.L, xyz)) // for free boundary condition
			continue;
		shifted[idx3to1(xyz, this->p.L) / WORD_SIZE] |= MSBs[idx / WORD_SIZE];
	}
}

void Ising::__nYShift(uWord* shifted) {
	uIndx z, i, idx, offset, xy_size = this->raw.x * this->raw.y;
	for (z = 0; z < this->raw.z; z += 1u)
		for (i = this->raw.x, offset = z*xy_size; i < this->raw.y; i += 1u)
			shifted[offset + i] = this->lattice[offset + i - this->raw.x];
	pos k;
	imposeBC(this->p.L, {0, -1, 0}, &k, this->p.boundary);
	if (outOfBounds(this->p.L, k)) // for free boundary condition
		for (z = 0; z < this->raw.z; z += 1u)
			for (i = 0, offset = z*xy_size; i < this->raw.x; i += 1u)
				shifted[offset + i] = 0;
	else
		for (z = 0, idx = idx3to1(k, this->p.L)/WORD_SIZE; z < this->raw.z; z += 1u)
			for (i = 0, offset = z * xy_size; i < this->raw.x; i += 1u)
				shifted[offset + i] = this->lattice[idx + i];
}

void Ising::__nZShift(uWord* shifted) {
	uIndx i, idx, xy_size = this->raw.x * this->raw.y;
	uIndx offset = this->rawN - xy_size;
	for (i = xy_size; i < this->rawN; i += 1u)
		shifted[i - xy_size] = this->lattice[i];
	pos k;
	imposeBC(this->p.L, {0, 0, this->p.L.z}, &k, this->p.boundary);
	if (outOfBounds(this->p.L, k))	// for free boundary condition
		for (i = 0; i < xy_size; i += 1u)
			shifted[offset + i] = 0;
	else
		for (i = 0, idx = idx3to1(k, this->p.L)/WORD_SIZE; i < xy_size; i += 1u)
			shifted[offset + i] = this->lattice[idx + i];
}
