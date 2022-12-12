#include "Ising.hpp"

float Ising::flipEnergyChange(pos const& i) {
	std::vector<Surface>::iterator it;
	int e = onEdge(i, this->p.L);
	it = this->p.whichSurface(e);
	// assuming interaction values are static

	static float J_bulk = coupling(this->p.Eaa, this->p.Ebb, this->p.Eab);

	uSize nSum;
	uSize n = 0u;
	bool s = this->operator()(i);
	nSum = this->sumNeighbours(i, &n);
	if (it == this->p.surfaces.end())	// the spin is in the bulk
		return 2 * J_bulk * bool2spin((s? nSum : (n - nSum)), n);

	// point is on a surface

	float J_surf = coupling(it->Eaa, it->Ebb, it->Eab);
	uSize nSurf = 0u;
	uSize except = 0u;
	switch (e) {
	case Edge::X_BEG:	// point is on a surface along x
	case Edge::X_END:	// surface corrections along y, z directions
		nSurf = this->sumNeighbours(i, {0u, 1u, 1u}, &except);
		n -= except;
		break;
	case Edge::Y_BEG:	// point is on a surface along y
	case Edge::Y_END:	// surface corrections along x, z directions
		nSurf = this->sumNeighbours(i, {1u, 0u, 1u}, &except);
		n -= except;
		break;
	case Edge::Z_BEG:	// point is on a surface along z
	case Edge::Z_END:	// surface corrections along x, y directions
		nSurf = this->sumNeighbours(i, {1u, 1u, 0u}, &except);
		n -= except;
		break;
	}
	nSum -= nSurf;
	float bulkChange = 2.f * J_bulk * bool2spin(nSum, n);
	float surfChange = 2.f * J_surf * bool2spin(nSurf, except);
	return bulkChange + surfChange;
}

float Ising::exchangeEnergyChange(pos const& i, pos const& j) {
	uSize iSum, jSum;
	uSize iN, jN;
	iN = jN = 0u;
	iSum = this->sumNeighbours(i, {1u, 1u, 1u}, j, &iN); // neighbour sum should
	jSum = this->sumNeighbours(j, {1u, 1u, 1u}, i, &jN); // not have each other

	float J_bulk = coupling(this->p.Eaa, this->p.Ebb, this->p.Eab);
	// float diff = 2.f * (this->operator()(i) - this->operator()(j));
	// since the spins are opposite, we simplify
	float diff = 2.f * bool2spin(this->operator()(i));
	if (this->p.surfaces.size() == 0) {
		// there are no surfaces to worry about
		// everything is bulk
		float nDiff = bool2spin(
			(float)jSum - iSum, (float)jN - iN		// diff of neighbour sums
		);
		return - J_bulk * diff * nDiff;
	}

	int iE = onEdge(i, this->p.L);
	int jE = onEdge(j, this->p.L);
	std::vector<Surface>::iterator iit = p.whichSurface(iE);
	std::vector<Surface>::iterator jit = p.whichSurface(jE);

	if (iit == this->p.surfaces.end() && jit == this->p.surfaces.end()) {
		// none of them are on edge
		// both spins are in bulk
		float nDiff = bool2spin(
			(float)jSum - iSum, (float)jN - iN		// diff of neighbour sums
		);
		return - J_bulk * diff * nDiff;
	}

	float H1 = (p.q-2)*field(jit->Eaa, jit->Ebb) - (p.q-1)*field(p.Eaa, p.Ebb);
	if (iit == this->p.surfaces.end()) {
		// j is on a surface edge
		float J_surf = coupling(jit->Eaa, jit->Ebb, jit->Eab);
		float bulkChange =  diff * J_bulk * bool2spin(iSum, iN);
		float surfChange = -diff * J_surf * bool2spin(jSum, jN);
		return bulkChange + surfChange + H1 * diff;
	}
	if (jit == this->p.surfaces.end()) {
		// i is on a surface edge
		float J_surf = coupling(iit->Eaa, iit->Ebb, iit->Eab);
		float bulkChange = -diff * J_bulk * bool2spin(jSum, jN);
		float surfChange =  diff * J_surf * bool2spin(iSum, iN);
		return bulkChange + surfChange - H1 * diff;
	}

	// they are both on a surface
	// since i and j are nearest neighbours, they must be on the same surface

	float J_surf = coupling(iit->Eaa, iit->Ebb, iit->Eab);
	pos offI;		// the neighbour of `i` which is off the surface
	pos offJ;		// the neighbour of `j` which is off the surface
	bool offSurfI, offSurfJ;
	// std::cout << (iE & jE) << std::endl;
	switch ((iE & jE)) {
	case Edge::X_BEG:
		offI = {(uIndx)(i.x+1u), i.y, i.z};
		offJ = {(uIndx)(j.x+1u), j.y, j.z};
		break;
	case Edge::X_END:
		offI = {(uIndx)(i.x-1u), i.y, i.z};
		offJ = {(uIndx)(j.x-1u), j.y, j.z};
		break;
	case Edge::Y_BEG:
		offI = {i.x, (uIndx)(i.y+1u), i.z};
		offJ = {j.x, (uIndx)(j.y+1u), j.z};
		break;
	case Edge::Y_END:
		offI = {i.x, (uIndx)(i.y-1u), i.z};
		offJ = {j.x, (uIndx)(j.y-1u), j.z};
		break;
	case Edge::Z_BEG:
		offI = {i.x, i.y, (uIndx)(i.z+1u)};
		offJ = {j.x, j.y, (uIndx)(j.z+1u)};
		break;
	case Edge::Z_END:
		offI = {i.x, i.y, (uIndx)(i.z-1u)};
		offJ = {j.x, j.y, (uIndx)(j.z-1u)};
		break;
	}

	offSurfI = this->operator()(offI); iSum -= offSurfI;
	offSurfJ = this->operator()(offJ); jSum -= offSurfJ;
	float bulkChange = -diff * J_bulk * 2.f * (offSurfJ - offSurfI);
	float surfChange = -diff * J_surf * bool2spin(jSum - iSum, jN - iN);
	// iN -> iN-1 and jN -> jN-1, jN-iN remains unchanged
	return bulkChange + surfChange;
}
