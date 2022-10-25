#include "Ising.hpp"

float coupling(float aa, float bb, float ab) {
	return 0.25f * (aa + bb - 2.f*ab);
}

float field(float aa, float bb) {
	return 0.25f * (aa - bb);
}

Edge onEdge(pos& i, vec3<uIndx>& s) {
	if (i.x == 0)     return Edge::X_BEG;
	if (i.x == s.x-1) return Edge::X_END;
	if (i.y == 0)     return Edge::Y_BEG;
	if (i.y == s.y-1) return Edge::Y_END;
	if (i.z == 0)     return Edge::Z_BEG;
	if (i.z == s.z-1) return Edge::Z_END;
	return Edge::NONE;
}

ModelParams::ModelParams() {}
ModelParams::ModelParams(vec3<uIndx>& size, vec3<BoundaryCondition>& bc) {
	this->L = size;
	this->boundary = bc;

	this->N = (uSize)size.x * size.y * size.z;
	this->q = 1;
	if (size.x > 1)	this->q *= 2;
	if (size.y > 1)	this->q *= 2;
	if (size.z > 1)	this->q *= 2;
	this->__static_bulk = true;
}

void ModelParams::setInteractions(float aa, float bb, float ab) {
	this->Eaa = aa;
	this->Ebb = bb;
	this->Eab = ab;
}

void ModelParams::create_surface(Surface& s) {
	this->surfaces.push_back(s);
}
void ModelParams::create_surface(Edge loc, float aa, float bb, float ab) {
	this->surfaces.push_back({loc, aa, bb, ab});
}

float ModelParams::J(pos& i, pos& j) {
	if (this->surfaces.size() == 0)
		return coupling(this->Eaa, this->Ebb, this->Eab);

	std::vector<Surface>::iterator it;
	Edge e;
	auto sCond = [e](Surface& s) {
		return s.loc == e;
	};

	Edge iE = onEdge(i, this->L);
	Edge jE = onEdge(j, this->L);
	if (iE == jE && jE != Edge::NONE) {    // points lie on the same edge
		e = iE;
		// find out if there is a surface at Edge e
		it = std::find_if(this->surfaces.begin(), this->surfaces.end(), sCond);
	} else it = this->surfaces.end();

	if (it != this->surfaces.end())
		return coupling(it->Eaa, it->Ebb, it->Eab);
	return coupling(this->Eaa, this->Ebb, this->Eab);
}

float ModelParams::H(pos& i) {
	if (this->surfaces.size() == 0)
		return	this->q * field(this->Eaa, this->Ebb);
	Edge iE = onEdge(i, this->L);
	if (iE == Edge::NONE)    // point is not on an edge
		return	this->q * field(this->Eaa, this->Ebb);

	auto sCond = [iE](Surface& s) {
		return s.loc == iE;
	};
	auto it = std::find_if(this->surfaces.begin(), this->surfaces.end(), sCond);
	if (it != this->surfaces.end())
		return	q * field(this->Eaa, this->Ebb);	// cheat, act as if the free edge opposite the surface is not free
	return 0.5 * q * field(this->Eaa, this->Ebb) + field(it->Eaa, it->Ebb);
}
