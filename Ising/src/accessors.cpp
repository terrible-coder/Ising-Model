/**
 * @file accessors.cpp
 * @author terrible-coder (github.com/terrible-coder)
 * @brief This file contains the getters and setters for the Ising class.
 * @date 2022-07-24
 * 
 */

#include "Ising.hpp"


static vec3<uIndx> offX = {1, 0, 0};
static vec3<uIndx> offY = {0, 1, 0};
static vec3<uIndx> offZ = {0, 0, 1};

uIndx Ising::getQ() {
	return this->p.q;
}

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
	// std::cout << a.x << "," << a.y << "," << a.z << "\t";

	if (a.x == this->p.L.x || a.y == this->p.L.y || a.z == this->p.L.z) return NULL;
	uSize idx = idx3to1(a, this->p.L);
	// std::cout << idx << "\t";
	return (this->lattice[idx / WORD_SIZE] >> (WORD_SIZE - idx%WORD_SIZE - 1)) & 1;
}
bool Ising::operator() (vec3<int> const& i) {
	return this->operator()(i.x, i.y, i.y);
}
bool Ising::operator() (pos const& i) {
	if (i.x == this->p.L.x || i.y == this->p.L.y || i.z == this->p.L.z) return NULL;
	uSize idx = idx3to1(i, this->p.L);
	return (this->lattice[idx / WORD_SIZE] >> (WORD_SIZE - idx%WORD_SIZE - 1)) & 1;
}

void Ising::equiv(int* x, int* y, int* z) {
	const vec3<int> uIdx{*x, *y, *z};
	pos aIdx;
	imposeBC(this->p.L, uIdx, &aIdx, this->p.boundary);
	*x = aIdx.x;
	*y = aIdx.y;
	*z = aIdx.z;
}
pos Ising::equiv(vec3<int> const& idx) {
	pos aIdx;
	imposeBC(this->p.L, idx, &aIdx, this->p.boundary);
	return aIdx;
}

void Ising::__getNN(pos const& i,
	             vec3<uIndx> const& off, int edge, bool P,
							 std::vector<vec3<int>>* NN) {

	Edge E_BEG, E_END;
	BoundaryCondition bc;
	uIndx L;
	if (off.x) {
		E_BEG = Edge::X_BEG;	E_END = Edge::X_END;
		bc = this->p.boundary.x;	L = this->p.L.x;
	} else if (off.y) {
		E_BEG = Edge::Y_BEG;	E_END = Edge::Y_END;
		bc = this->p.boundary.y;	L = this->p.L.y;
	} else if (off.z) {
		E_BEG = Edge::Z_BEG;	E_END = Edge::Z_END;
		bc = this->p.boundary.z;	L = this->p.L.z;
	} else return;	// no direction mentioned
	if (L == 1u)  // neighbours available along given direction only if
		return;     // length along same direction is at least 2

	if ((edge & E_BEG) != 0) {
		NN->push_back(toIntVec(i + off));
		if (!P && L > 2 && bc == BoundaryCondition::PERIODIC)
			NN->push_back(i - off);
	} else
	if ((edge & E_END) != 0) {
		if (!P)
			NN->push_back(toIntVec(i - off));
		if (L > 2 && bc == BoundaryCondition::PERIODIC)
			NN->push_back(toIntVec(i + off));
	} else {
		NN->push_back(toIntVec(i + off));
		if (!P)
			NN->push_back(i - off);
	}
}

uIndx Ising::__sumDir(pos const& i, vec3<uIndx> const& off, int edge, bool P, uSize* n) {
	std::vector<vec3<int>> neighbours;
	this->__getNN(i, off, edge, P, &neighbours);
	*n += neighbours.size();

	uIndx SS = 0u;
	for (auto it = neighbours.begin(); it != neighbours.end(); it ++)
		SS += this->operator()(*it);
	return SS;
}

uIndx Ising::sumNeighboursP(pos const& i, vec3<uIndx> const& dir, uSize* n) {
	int e = onEdge(i, this->p.L);

	uIndx SS = 0u;
	if (dir.x)
		SS += this->__sumDir(i, offX, e, true, n);
	if (dir.y)
		SS += this->__sumDir(i, offY, e, true, n);
	if (dir.z)
		SS += this->__sumDir(i, offZ, e, true, n);

	return SS;
}

void Ising::getNeighbours(pos const& i, std::vector<vec3<int>>* NN) {
	int e = onEdge(i, this->p.L);

	this->__getNN(i, offX, e, false, NN);
	this->__getNN(i, offY, e, false, NN);
	this->__getNN(i, offZ, e, false, NN);
}

uIndx Ising::sumNeighbours(pos const& i, uSize* n) {
	int e = onEdge(i, this->p.L);

	// sum of neighbours in all directions
	return this->__sumDir(i, offX, e, false, n)
	     + this->__sumDir(i, offY, e, false, n)
			 + this->__sumDir(i, offZ, e, false, n);
}
uIndx Ising::sumNeighbours(pos const& i, vec3<uIndx> const& dir, uSize* n) {
	int e = onEdge(i, this->p.L);

	uIndx SS = 0u;
	if (dir.x)
		SS += this->__sumDir(i, offX, e, false, n);
	if (dir.y)
		SS += this->__sumDir(i, offY, e, false, n);
	if (dir.z)
		SS += this->__sumDir(i, offZ, e, false, n);

	return SS;
}
uIndx Ising::sumNeighbours(pos const& i, vec3<uIndx> const& dir, pos const& except, uSize* n) {
	if (outOfBounds(this->p.L, except))
		return this->sumNeighbours(i, dir, n);
	uIndx S = this->sumNeighbours(i, dir, n) - this->operator()(except);
	*n -= 1;
	return S;
}

void Ising::__nXShift(uWord* shifted) {
	bool* MSBs = new bool[this->rawN];
	for (uint idx = 0; idx < this->rawN; idx++) {
		MSBs[idx] = this->lattice[idx] >> (WORD_SIZE - 1);
		shifted[idx] = this->lattice[idx] << 1;
	}
	pos xyz;
	for (uSize idx = 0; idx < this->p.N; idx += WORD_SIZE) {
		idx1to3(idx, this->p.L, &xyz);
		// shift MSB of x to x-1
		if (xyz.x == 0) xyz.x = this->p.L.x-1;
		else xyz.x -= 1u;
		shifted[idx3to1(xyz, this->p.L) / WORD_SIZE] |= MSBs[idx / WORD_SIZE];
	}
	delete MSBs;
}

void Ising::__nYShift(uWord* shifted) {
	uIndx z, i, idx, offset;
	const uSize xy_size = this->raw.x * this->raw.y;
	for (z = 0; z < this->raw.z; z += 1u)
		for (i = this->raw.x, offset = z*xy_size; i < this->raw.y; i += 1u)
			shifted[offset + i] = this->lattice[offset + i - this->raw.x];
	pos k{
		0,
		(uIndx)(this->p.L.y - 1), // populate y-beg with y-end for each z
		0
	};
	idx = idx3to1(k, this->p.L) / WORD_SIZE;
	for (z = 0; z < this->raw.z; z += 1u)
		for (i = 0, offset = z * xy_size; i < this->raw.x; i += 1u)
			shifted[offset + i] = this->lattice[idx + i];
}

void Ising::__nZShift(uWord* shifted) {
	uIndx i;
	const uSize xy_size = this->raw.x * this->raw.y;
	uIndx offset = this->rawN - xy_size;
	for (i = xy_size; i < this->rawN; i += 1u)
		shifted[i - xy_size] = this->lattice[i];
	// populate last z with first z
	for (i = 0; i < xy_size; i += 1u)
		shifted[offset + i] = this->lattice[i];
}

float Ising::getNNCoup(pos const& i, pos const& j) {
	return this->p.J(i, j);
}

float Ising::getField(pos const& i) {
	return this->p.H(i);
}

vec3<uIndx> Ising::getVecSize() {
	return this->p.L;
}
