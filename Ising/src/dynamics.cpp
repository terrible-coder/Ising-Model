#include "Ising.hpp"

float Ising::partialEnergy(uWord* shifted, uSize beg, vec3<int> off) {
	uSize realI;
	pos ps, pn;
	uWord sigma, prod;
	double E = 0.;

	/* Multiplication of spin values (+1 or -1) can be mapped directly to
	 * multiplication of boolean values.
	 * A_spin * B_spin \equiv A XNOR B
	 * The actual value can be achieved using the very useful bool2spin.
	 */
	realI = beg;
	for (uIndx i = 0; i < this->rawN; i++, realI += WORD_SIZE) {
		sigma = this->lattice[i];
		prod = ~(this->lattice[i] ^ shifted[i]);

		idx1to3(realI, this->p.L, &ps);
		pn = pos{ps.x, ps.y, ps.z};
		pn.x += off.x; // neighbour along x
		pn.y += off.y; // neighbour along y
		pn.z += off.z; // neighbour along z
		for (uIndx j = 0; j < WORD_SIZE; j += 1u) {
			E -= this->getField(ps) * bool2spin(sigma & 1);
			E -= this->getNNCoup(ps, pn) * bool2spin(prod & 1);
			prod >>= 1;	sigma >>= 1;
			ps.x -= 1u;	pn.x -= 1u;
		}
	}
	return E;
}

void Ising::flip(uIndx x, uIndx y, uIndx z) {
	pos p{x, y, z};
	this->flip(p);
}
void Ising::flip(pos& p) {
	uSize idx = idx3to1(p, this->p.L);
	flipBit(&(this->lattice[idx / WORD_SIZE]), idx % WORD_SIZE);
}

void Ising::exchange(int x1, int y1, int z1, int x2, int y2, int z2) {
	vec3<int> p1{x1, y1, z1}; pos a1;
	vec3<int> p2{x2, y2, z2}; pos a2;
	imposeBC(this->p.L, p1, &a1, this->p.boundary);
	imposeBC(this->p.L, p2, &a2, this->p.boundary);

	this->exchange(a1, a2);
}
void Ising::exchange(pos& p1, pos& p2) {
	uSize idx1 = idx3to1(p1, this->p.L); // row major index of spin 1
	uSize idx2 = idx3to1(p2, this->p.L); // row major index of spin 2
	uWord* n1 = &(this->lattice[idx1 / WORD_SIZE]); // the word where spin 1 is stored
	uWord* n2 = &(this->lattice[idx2 / WORD_SIZE]); // the word where spin 2 is stored
	flipBit(n1, idx1 % WORD_SIZE); // this function gets called only if the
	flipBit(n2, idx2 % WORD_SIZE); // two spins are not the same
}

float Ising::Hamiltonian() {
	float E = 0.;
	uWord* shift = new uWord[this->rawN];

	this->__nXShift(shift);	E += this->partialEnergy(shift, WORD_SIZE-1, {1, 0, 0});
	this->__nYShift(shift);	E += this->partialEnergy(shift, WORD_SIZE-1, {0, -1, 0});
	this->__nZShift(shift);	E += this->partialEnergy(shift, 0, {0, 0, 1});

	delete shift;
	return E;
}

float Ising::Magnetisation() {
	if (this->__M <= (float) this->p.N) return this->__M;
	uint M = 0;
	for (uIndx i = 0; i < this->rawN; i++)
		M += std::__popcount(this->lattice[i]);
	this->__M = bool2spin(M, this->p.N);
	return __M;
}
