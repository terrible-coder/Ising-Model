#include "Ising.hpp"

float Ising::flipEnergyChange(pos const& i) {
	std::vector<Surface>::iterator it;
	Edge e = onEdge(i, this->p.L);
	it = this->p.whichSurface(e);
	// assuming interaction values are static

	static float J_bulk = coupling(this->p.Eaa, this->p.Ebb, this->p.Eab);
	static float H_bulk = this->p.q * field(this->p.Eaa, this->p.Ebb);

	uSize nSum;
	uSize n = 0u;
	bool s = this->operator()(i);
	nSum = this->sumNeighbours(i, &n);
	if (it == this->p.surfaces.end())	// the spin is in the bulk
		return 2 * J_bulk * bool2spin(s? nSum : n - nSum, n);

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
	Edge iE = onEdge(i, this->p.L);
	Edge jE = onEdge(j, this->p.L);
	std::vector<Surface>::iterator iit = p.whichSurface(iE);
	std::vector<Surface>::iterator jit = p.whichSurface(jE);

	float J_bulk = coupling(this->p.Eaa, this->p.Ebb, this->p.Eab);
	float H_bulk = this->p.q * field(this->p.Eaa, this->p.Ebb);

	// float diff = 2.f * (this->operator()(i) - this->operator()(j));
	// since the spins are opposite, we simplify
	float diff = 2.f * bool2spin(this->operator()(i));

	if (iit == this->p.surfaces.end() && jit == this->p.surfaces.end()) {
		// none of them are on edge
		// both spins are in bulk
		float nDiff = 2.f * ((float)jSum - (float)iSum); // diff of neighbour sums
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
	bool offSurfI;
	bool offSurfJ;
	switch (iE) {
	case Edge::X_BEG:
		offSurfI = this->operator()(pos{1u, i.y, i.z});
		iSum -= offSurfI;
		offSurfJ = this->operator()(pos{1u, j.y, j.z});
		jSum -= offSurfJ;
		break;

	case Edge::X_END:
		offSurfI = this->operator()(pos{(uIndx)(p.L.x-2), i.y, i.z});
		iSum -= offSurfI;
		offSurfJ = this->operator()(pos{(uIndx)(p.L.x-2), j.y, j.z});
		jSum -= offSurfJ;
		break;

	case Edge::Y_BEG:
		offSurfI = this->operator()(pos{i.x, 1u, i.z});
		iSum -= offSurfI;
		offSurfJ = this->operator()(pos{j.x, 1u, j.z});
		jSum -= offSurfJ;
		break;

	case Edge::Y_END:
		offSurfI = this->operator()(pos{i.x, (uIndx)(p.L.y-2), i.z});
		iSum -= offSurfI;
		offSurfJ = this->operator()(pos{j.x, (uIndx)(p.L.y-2), j.z});
		jSum -= offSurfJ;
		break;

	case Edge::Z_BEG:
		offSurfI = this->operator()(pos{i.x, i.y, 1u});
		iSum -= offSurfI;
		offSurfJ = this->operator()(pos{j.x, j.y, 1u});
		jSum -= offSurfJ;
		break;

	case Edge::Z_END:
		offSurfI = this->operator()(pos{i.x, i.y, (uIndx)(p.L.z-2)});
		iSum -= offSurfI;
		offSurfJ = this->operator()(pos{j.x, j.y, (uIndx)(p.L.z-2)});
		iSum -= offSurfI;
		break;
	}
	float bulkChange = -diff * J_bulk * 2.f * (offSurfJ - offSurfI);
	float surfChange = -diff * J_surf * 2.f * (jSum - iSum);
}
