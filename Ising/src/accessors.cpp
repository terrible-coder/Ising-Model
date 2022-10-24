/**
 * @file accessors.cpp
 * @author terrible-coder (github.com/terrible-coder)
 * @brief This file contains the getters and setters for the Ising class.
 * @date 2022-07-24
 * 
 */

#include "Ising.hpp"

std::function<double(const pos& i, const pos& j)> Ising::J;
std::function<double(const pos& i)> Ising::H;

void Ising::setCoupling(std::function<double(const pos& i, const pos& j)>& coupling) {
	Ising::J = coupling;
}

void Ising::setField(std::function<double(const pos& i)>& field) {
	Ising::H = field;
}

double Ising::getField(const pos& i) {
	return Ising::H(i);
}

double Ising::getNNCoup(const pos& i, const pos& j) {
	return Ising::J(i, j);
}

uIndx Ising::getSizeX() {
	return this->L.x;
}

uIndx Ising::getSizeY() {
	return this->L.y;
}

uIndx Ising::getSizeZ() {
	return this->L.z;
}

std::uint32_t Ising::getSize() {
	return this->N;
}

double Ising::getTemp() {
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
	imposeBC(this->L, {x, y, z}, &a, this->boundary);

	if (a.x == this->L.x || a.y == this->L.y || a.z == this->L.z) return NULL;
	uSize idx = idx3to1(a, this->L);
	return (this->lattice[idx / WORD_SIZE] >> (WORD_SIZE - idx%WORD_SIZE - 1)) & 1 ;
}

void Ising::equiv(int* x, int* y, int* z) {
	const vec3<int> uIdx{*x, *y, *z};
	pos aIdx;
	imposeBC(this->L, uIdx, &aIdx, this->boundary);
	*x = aIdx.x;
	*y = aIdx.y;
	*z = aIdx.z;
}
pos& Ising::equiv(vec3<int>& idx) {
	pos aIdx;
	imposeBC(this->L, idx, &aIdx, this->boundary);
	return aIdx;
}

void Ising::__nXShift(uWord* shifted) {
	bool* MSBs = new bool[this->rawN];
	for (uint idx = 0; idx < this->rawN; idx++) {
		MSBs[idx] = this->lattice[idx] >> (WORD_SIZE - 1);
		shifted[idx] = this->lattice[idx] << 1;
	}
	for (uSize idx = 0; idx < this->N; idx += WORD_SIZE) {
		pos xyz;
		idx1to3(idx, this->L, &xyz);
		imposeBC(this->L, vec3<int>{xyz.x-1, xyz.y, xyz.z}, &xyz, this->boundary);
		if (outOfBounds(this->L, xyz)) // for free boundary condition
			continue;
		shifted[idx3to1(xyz, this->L) / WORD_SIZE] |= MSBs[idx / WORD_SIZE];
	}
}

void Ising::__nYShift(uWord* shifted) {
	uIndx z, i, idx, offset, xy_size = this->raw.x * this->raw.y;
	for (z = 0; z < this->raw.z; z += 1u)
		for (i = this->raw.x, offset = z*xy_size; i < this->raw.y; i += 1u)
			shifted[offset + i] = this->lattice[offset + i - this->raw.x];
	pos k;
	imposeBC(this->L, {0, -1, 0}, &k, this->boundary);
	if (outOfBounds(this->L, k)) // for free boundary condition
		for (z = 0; z < this->raw.z; z += 1u)
			for (i = 0, offset = z*xy_size; i < this->raw.x; i += 1u)
				shifted[offset + i] = 0;
	else
		for (z = 0, idx = idx3to1(k, this->L)/WORD_SIZE; z < this->raw.z; z += 1u)
			for (i = 0, offset = z * xy_size; i < this->raw.x; i += 1u)
				shifted[offset + i] = this->lattice[idx + i];
}

void Ising::__nZShift(uWord* shifted) {
	uIndx i, idx, xy_size = this->raw.x * this->raw.y;
	uIndx offset = this->rawN - xy_size;
	for (i = xy_size; i < this->rawN; i += 1u)
		shifted[i - xy_size] = this->lattice[i];
	pos k;
	imposeBC(this->L, {0, 0, this->L.z}, &k, this->boundary);
	if (outOfBounds(this->L, k))	// for free boundary condition
		for (i = 0; i < xy_size; i += 1u)
			shifted[offset + i] = 0;
	else
		for (i = 0, idx = idx3to1(k, this->L)/WORD_SIZE; i < xy_size; i += 1u)
			shifted[offset + i] = this->lattice[idx + i];
}
