#include "io/prepare.hpp"

typedef std::vector<std::string> sToken;

/*
KEYWORDS
========

	"name"           // name of the directory to save data in
	"kB"             // the value of the Boltzmann constant to use
	"interaction"    // the interaction values

	"box"            // the dimensions of the box
	"boundary"       // the boundary conditions on the different sides
	"surface"        // introduce a surface

	"temperature"    // the temperature(s) at which to run the simulation
	"concentration"  // concentration of the sample
	"ensemble"       // number of ensembles to generate

	"ar_prob"        // accept-reject probability model
	"dynamics"       // the kind of dynamics to force on the model
*/

sToken tokenise(std::string s, char delim, char stop) {
	sToken tokens;
	std::string st = "";
	for (uint i = 0; i < s.size(); i += 1u) {
		char ch = s.at(i);
		if (ch == stop) break;	// allowing comments
		if (ch == delim && st != "") {
			tokens.push_back(st);
			st = "";
		} else if (ch != delim)
			st += ch;
	}
	if (st != "") tokens.push_back(st);
	return tokens;
}

template<typename T>
bool readVec(sToken const& tokens, vec3<T>* v) {
	if (tokens.size() < 4) return false;
	v->x = (T) std::stof(tokens[1]);
	v->y = (T) std::stof(tokens[2]);
	v->z = (T) std::stof(tokens[3]);
	return true;
}
bool readVec(sToken const& tokens, vec3<std::string>* v) {
	if (tokens.size() < 4) return false;
	v->x = tokens[1];
	v->y = tokens[2];
	v->z = tokens[3];
	return true;
}

bool readTemp(sToken const& tokens, std::vector<float>* T) {
	if (tokens.size() < 2) return false;
	std::string st = "";
	if (tokens.size() > 2)
		for (uint i = 1; i < tokens.size(); i += 1u)
			st += tokens[i];
	else
		st = tokens[1];
	float dT;
	sToken values = tokenise(st, ':', '\n');
	switch (values.size()) {
	case 1:
		T->push_back(std::stof(st));
		return true;
	case 2:
		dT = 0.1f;    // default interval value
		break;
	case 3:
		dT = std::stof(values[1]);
		break;
	default:
		std::cout << "Temperature range is not defined properly." << std::endl;
		return false;
	}
	float Ti = std::stof(*(values.begin()));
	float Tf = std::stof(*(values.end()-1));
	float v = Ti;
	while ((Tf - v) >= -dT/10) {
		T->push_back(v);
		v += dT;
	}
	return true;
}

bool readProb(sToken const& tokens, TransProb* p) {
	if (tokens.size() > 2) {
		std::cout << "Only accepting first token for accept-reject probability model.";
		std::cout << " Passed " << tokens.size() << "." << std::endl;
	}
	if (tokens[1] == "boltzmann")
		*p = TransProb::BOLTZMANN;
	else if (tokens[2] == "suzuki-kubo")
		*p = TransProb::SUZU_KUBO;
	else {
		std::cout << "Unrecognised transition probability model." << std::endl;
		return false;
	}
	return true;
}

bool readKinet(sToken const& tokens, Dynamics* d) {
	if (tokens.size() > 2) {
		std::cout << "Only accepting first token for dynamics model.";
		std::cout << " Passed " << tokens.size() << "." << std::endl;
	}
	if (tokens[1] == "flip")
		*d = Dynamics::FLIP;
	else if (tokens[1] == "exchange")
		*d = Dynamics::EXCHANGE;
	else {
		std::cout << "Unrecognised dynamics model." << std::endl;
		return false;
	}
	return true;
}

bool deal_with(sToken const& tokens, Context* ctx) {
	std::string keyword = tokens[0];
	if (keyword == "name")
		ctx->saveDir = tokens[1];
	else
	if (keyword == "kB")
		ctx->BoltzConstant = std::stof(tokens[1]);
	else
	if (keyword == "interaction")
		if(!readVec(tokens, &(ctx->interact))) return false;
		else {}
	else
	if (keyword == "box")
		if (!readVec(tokens, &(ctx->size))) return false;
		else {}
	else
	if (keyword == "boundary") {
		vec3<std::string> bc;
		if(!readVec(tokens, &bc)) return false;
		if (bc.x=="free" || bc.x=="f") ctx->boundary.x = BoundaryCondition::FREE;
		else ctx->boundary.x = BoundaryCondition::PERIODIC;

		if (bc.y=="free" || bc.y=="f") ctx->boundary.y = BoundaryCondition::FREE;
		else ctx->boundary.y = BoundaryCondition::PERIODIC;

		if (bc.z=="free" || bc.z=="f") ctx->boundary.z = BoundaryCondition::FREE;
		else ctx->boundary.z = BoundaryCondition::PERIODIC;
	} else
	if (keyword == "surface") {
		ctx->surfLocs.push_back(tokens[4]);
		vec3<float> interactions;
		if (!readVec(tokens, &interactions)) return false;
		ctx->surfInts.push_back(interactions);
	} else
	if (keyword == "temperature")
		if (!readTemp(tokens, &(ctx->Temperature))) return false;
		else {}
	else
	if (keyword == "concentration")
		ctx->Concentration = (uIndx) std::stoi(tokens[1]);
	else
	if (keyword == "ensemble")
		ctx->Ensemble_Size = (uIndx) std::stoi(tokens[1]);
	else
	if (keyword == "ar_prob")
		if (!readProb(tokens, &(ctx->Transition))) return false;
		else {}
	else
	if (keyword == "dynamics")
		if (!readKinet(tokens, &(ctx->SpinKinetics))) return false;
	return true;
}

static bool initiated = false;

bool init_system(std::string filename, Context* ctx) {
	if (initiated) {
		std::cout << "Attempt to overwrite system data. Ignoring attempt." << std::endl;
		return false;
	}

	std::ifstream input_file(filename);
	if (!input_file.is_open()) {
		std::cout << "Cannot read input file. Exiting." << std::endl;
		return false;
	}

	static const char DELIMITER = ' ';
	static const char STOP_CHAR = '#';

	sToken tokens;
	std::string line;
	bool success = true;
	while(getline(input_file, line)) {
		sToken tok = tokenise(line, DELIMITER, STOP_CHAR);
		if (tok.size() == 0)
			continue;
		success &= deal_with(tok, ctx);
	}
	input_file.close();
	return success;
}

bool safeCreate(std::string path) {
	return std::filesystem::exists(path) || std::filesystem::create_directories(path);
}

std::string prepExperiment(Context* ctx) {
	bool success = true;

	std::string parent = ctx->saveDir;
	std::string snaps = "snaps/";
	std::string frame = "frames/";

	for (auto it = ctx->Temperature.begin(); it != ctx->Temperature.end(); it++) {
		std::string T = "Temp" + std::to_string(*it) + "/";
		success = success && safeCreate(parent+T+snaps);
		success = success && safeCreate(parent+T+frame);

		for (int j = 0; (j < ctx->Ensemble_Size) && success; j++)
			success = success && safeCreate(parent+T+frame+"En"+std::to_string(j+1));

		if (!success) {
			std::cout << "Could not prepare directories properly" << std::endl;
			return "";
		}
	}
	return parent;
}
