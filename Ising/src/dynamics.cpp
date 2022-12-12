#include "Ising.hpp"

float Ising::partialEnergy(uWord* shifted, uSize beg, vec3<int> off) {
	uSize realI;
	pos ps, pn;
	uWord sigma, prod;
	float E = 0.f;

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
	// either break function into smaller functions or move to different file

	float E = 0.;
	uWord* shift = new uWord[this->rawN];

	if (!this->p.__static_bulk) {
		this->__nXShift(shift);	E += this->partialEnergy(shift, WORD_SIZE-1, {1, 0, 0});
		this->__nYShift(shift);	E += this->partialEnergy(shift, WORD_SIZE-1, {0, -1, 0});
		this->__nZShift(shift);	E += this->partialEnergy(shift, 0, {0, 0, 1});

		delete shift;
		return E;
	}

	uSize SS = 0;
	static float J_bulk = coupling(this->p.Eaa, this->p.Ebb, this->p.Eab);
	static float h = field(this->p.Eaa, this->p.Ebb);
	static float H_bulk = this->p.q * h;

	// taking care of bi-layer is little cumbersome
	// settle for anything above 2 for now
	if (this->p.L.x > 2) {
		this->__nXShift(shift);
		for (uIndx i = 0; i < this->rawN; i += 1u)
			SS += std::__popcount(~(lattice[i] ^ shift[i]));
	}
	if (this->p.L.y > 2) {
		this->__nYShift(shift);
		for (uIndx i = 0; i < this->rawN; i += 1u)
			SS += std::__popcount(~(lattice[i] ^ shift[i]));
	}
	if (this->p.L.z > 2) {
		this->__nZShift(shift);
		for (uIndx i = 0; i < this->rawN; i += 1u)
			SS += std::__popcount(~(lattice[i] ^ shift[i]));
	}

	// The PBC energy
	E = -J_bulk * bool2spin(SS, this->p.N << 1) - H_bulk * this->Magnetisation();

	// FBC and FBC+Surface corrections
	uSize S_beg, S_end;   // sum of spins
	uSize SS_fbc, SS_sur;  // sum of spin spin product
	uSize ns;	// keeps track of how many spins have been added
	uIndx end;

	if (this->p.boundary.x == BoundaryCondition::FREE) {
		S_beg = S_end = 0u;
		SS_fbc = 0u;
		end = this->p.L.x - 1;
		uSize yz_size = (uSize) this->p.L.y * this->p.L.z;
		for (uIndx y = 0u; y < this->p.L.y; y += 1u) {
			for (uIndx z = 0u; z < this->p.L.z; z += 1u) {
				pos begP = {0u, y, z}, endP = {end, y, z};
				bool s_beg = this->operator()(begP);
				bool s_end = this->operator()(endP);
				SS_fbc += !(s_beg ^ s_end);
				S_beg += s_beg;  S_end += s_end;
			}
		}
		// fbc correction for coupling
		E += J_bulk * bool2spin(SS_fbc, yz_size);

		std::vector<Surface>::iterator surf;
		if ((surf = p.whichSurface(Edge::X_BEG)) != p.surfaces.end()) {
			// x-beg is free edge with surface
			SS_sur = 0u;
			ns = 0u;
			for (uIndx y = 0; y < this->p.L.y; y += 1u) {
				for (uIndx z = 0; z < this->p.L.z; z += 1u) {
					pos begP = {0u, y, z};
					bool s = this->operator()(begP);
					// sum neighbours only on the surface (    v  v ) (y, z directions)
					uSize n = 0u;
					uIndx nSum = this->sumNeighboursP(begP, {0, 1, 1}, &n);
					SS_sur += s? nSum : n - nSum;	// sum of spin products (bin equiv)
					ns += n;
				}
			}
			float H1 = (p.q - 2) * field(surf->Eaa, surf->Ebb);
			float J_surf = coupling(surf->Eaa, surf->Ebb, surf->Eab);
			// surface correction for coupling
			E += (J_bulk - J_surf) * bool2spin(SS_sur, ns);
			// surface correction for field
			// careful with the terms
			E += -H1 * bool2spin(S_beg, yz_size) + h * bool2spin(S_end, yz_size);
		} else		// x-beg is a free edge with no surface
			E += h * bool2spin(S_beg, yz_size);

		if ((surf = p.whichSurface(Edge::X_END)) != p.surfaces.end()) {
			// x-end is a free edge with surface
			SS_sur = 0u;
			ns = 0u;
			for (uIndx y = 0; y < this->p.L.y; y += 1u) {
				for (uIndx z = 0; z < this->p.L.z; z += 1u) {
					pos endP = {end, y, z};
					bool s = this->operator()(endP);
					// sum neighbours only on the surface (    v  v ) (y, z directions)
					uSize n = 0u;
					uIndx nSum = this->sumNeighboursP(endP, {0, 1, 1}, &n);
					SS_sur += s? nSum : n - nSum;	// sum of spin products (bin equiv)
					ns += n;
				}
			}
			float H1 = (p.q - 2) * field(surf->Eaa, surf->Ebb);
			float J_surf = coupling(surf->Eaa, surf->Ebb, surf->Eab);
			// surface correction for coupling
			E += (J_bulk - J_surf) * bool2spin(SS_sur, ns);
			// surface correction for field
			// careful with the terms
			E += - H1 * bool2spin(S_end, yz_size) + h * bool2spin(S_beg, yz_size);
		} else		// x-end is a free edge with no surface
			E += h * bool2spin(S_end, yz_size);
	}


	if (this->p.boundary.y == BoundaryCondition::FREE) {
		S_beg = S_end = 0u;
		SS_fbc = 0u;
		end = this->p.L.y - 1;
		uSize xz_size = (uSize) this->p.L.x * this->p.L.z;
		for (uIndx x = 0u; x < this->p.L.x; x += 1u) {
			for (uIndx z = 0u; z < this->p.L.z; z += 1u) {
				pos begP = {x, 0u, z}, endP = {x, end, z};
				bool s_beg = this->operator()(begP);
				bool s_end = this->operator()(endP);
				SS_fbc += !(s_beg ^ s_end);
				S_beg += s_beg;  S_end += s_end;
			}
		}
		// fbc correction for coupling
		E += J_bulk * bool2spin(SS_fbc, xz_size);

		std::vector<Surface>::iterator surf;
		if ((surf = p.whichSurface(Edge::Y_BEG)) != p.surfaces.end()) {
			// y-beg is free edge with surface
			SS_sur = 0u;
			ns = 0u;
			for (uIndx x = 0; x < this->p.L.x; x += 1u) {
				for (uIndx z = 0; z < this->p.L.z; z += 1u) {
					pos begP = {x, 0u, z};
					bool s = this->operator()(begP);
					// sum neighbours only on the surface ( v     v ) (x, z directions)
					uSize n = 0u;
					uIndx nSum = this->sumNeighboursP(begP, {1, 0, 1}, &n);
					SS_sur += s? nSum : n - nSum;	// sum of spin products (bin equiv)
					ns += n;
				}
			}
			float H1 = (p.q - 2) * field(surf->Eaa, surf->Ebb);
			float J_surf = coupling(surf->Eaa, surf->Ebb, surf->Eab);
			// surface correction for coupling
			E += (J_bulk - J_surf) * bool2spin(SS_sur, ns);
			// surface correction for field
			// careful with the terms
			E += -H1 * bool2spin(S_beg, xz_size) + h * bool2spin(S_end, xz_size);
		} else		// y-beg is a free edge with no surface
		E += h * bool2spin(S_beg, xz_size);

		if ((surf = p.whichSurface(Edge::Y_END)) != p.surfaces.end()) {
			// y-end is a free edge with surface
			SS_sur = 0u;
			ns = 0u;
			for (uIndx x = 0; x < this->p.L.x; x += 1u) {
				for (uIndx z = 0; z < this->p.L.z; z += 1u) {
					pos endP = {x, end, z};
					bool s = this->operator()(endP);
					// sum neighbours only on the surface ( v     v ) (y, z directions)
					uSize n = 0u;
					uIndx nSum = this->sumNeighboursP(endP, {1, 0, 1}, &n);
					SS_sur += s? nSum : n - nSum;	// sum of spin products (bin equiv)
					ns += n;
				}
			}
			float H1 = (p.q - 2) * field(surf->Eaa, surf->Ebb);
			float J_surf = coupling(surf->Eaa, surf->Ebb, surf->Eab);
			// surface correction for coupling
			E += (J_bulk - J_surf) * bool2spin(SS_sur, ns);
			// surface correction for field
			// careful with the terms
			E += -H1 * bool2spin(S_end, xz_size) + h * bool2spin(S_beg, xz_size);
		} else		// y-end is a free edge with no surface
		E += h * bool2spin(S_end, xz_size);
	}


	if (this->p.boundary.z == BoundaryCondition::FREE) {
		S_beg = S_end = 0u;
		SS_fbc = 0u;
		end = this->p.L.z - 1;
		uIndx begP = 0u;
		uIndx endP = this->rawN - (this->raw.x*this->raw.y);  // last layer
		uSize xy_size = (uSize) this->p.L.x * this->p.L.y;
		for (uIndx i = 0; i < this->raw.x*this->raw.y; i += 1u) {
			uWord s_beg = this->lattice[begP + i], s_end = this->lattice[endP + i];
			SS_fbc += std::__popcount(~(s_beg ^ s_end));
			S_beg += std::__popcount(s_beg);  S_end += std::__popcount(s_end);
		}
		// fbc correction for coupling
		E += J_bulk * bool2spin(SS_fbc, xy_size);

		std::vector<Surface>::iterator surf;
		if ((surf = p.whichSurface(Edge::Z_BEG)) != p.surfaces.end()) {
			// z-beg is free edge with surface
			SS_sur = 0u;
			ns = 0u;
			for (uIndx x = 0; x < this->p.L.x; x += 1u) {
				for (uIndx y = 0; y < this->p.L.y; y += 1u) {
					pos begP = {x, y, 0u};
					bool s = this->operator()(begP);
					// sum neighbours only on the surface ( v  v    ) (x, y directions)
					uSize n = 0u;
					uIndx nSum = this->sumNeighboursP(begP, {1, 1, 0}, &n);
					SS_sur += s? nSum : n - nSum;	// sum of spin products (bin equiv)
					ns += n;
				}
			}
			float H1 = (p.q - 2) * field(surf->Eaa, surf->Ebb);
			float J_surf = coupling(surf->Eaa, surf->Ebb, surf->Eab);
			// surface correction for coupling
			E += (J_bulk - J_surf) * bool2spin(SS_sur, xy_size);
			// surface correction for field
			// careful with the terms
			E += -H1 * bool2spin(S_beg, xy_size) + h * bool2spin(S_end, xy_size);
		} else		// z-beg is a free edge with no surface
			E += h * bool2spin(S_beg, xy_size);
		if ((surf = p.whichSurface(Edge::Z_END)) != p.surfaces.end()) {
			// z-end is a free edge with surface
			SS_sur = 0u;
			ns = 0u;
			for (uIndx x = 0; x < this->p.L.x; x += 1u) {
				for (uIndx y = 0; y < this->p.L.y; y += 1u) {
					pos endP = {x, y, end};
					bool s = this->operator()(endP);
					// sum neighbours only on the surface ( v  v    ) (x, y directions)
					uSize n = 0u;
					uIndx nSum = this->sumNeighboursP(endP, {1, 1, 0}, &n);
					SS_sur += s? nSum : n - nSum;	// sum of spin products (bin equiv)
					ns += n;
				}
			}
			float H1 = (p.q - 2) * field(surf->Eaa, surf->Ebb);
			float J_surf = coupling(surf->Eaa, surf->Ebb, surf->Eab);
			// surface correction for coupling
			E += (J_bulk - J_surf) * bool2spin(SS_sur, xy_size);
			// surface correction for field
			// careful with the terms
			E += -H1 * bool2spin(S_end, xy_size) + h * bool2spin(S_beg, xy_size);
		} else		// z-end is a free edge with no surface
			E += h * bool2spin(S_end, xy_size);
	}

	delete shift;
	return E;
}

float Ising::Magnetisation() {
	uSize M = 0;
	for (uIndx i = 0; i < this->rawN; i++)
		M += std::__popcount(this->lattice[i]);
	return bool2spin(M, this->p.N);
}
