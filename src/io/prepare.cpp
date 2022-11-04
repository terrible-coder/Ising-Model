#include "io/prepare.hpp"


typedef std::vector<std::string> sToken;

/*
KEYWORDS
========

	"name",        // name of the directory to save data in
	"kB",          // the value of the Boltzmann constant to use
	"interaction", // the interaction values

	"box",         // the dimensions of the box
	"boundary",    // the boundary conditions on the different sides
	"surface",     // introduce a surface

	"temperature", // the temperature(s) at which to run the simulation
	"ensemble",    // number of ensembles to generate

	"ar_prob",     // accept-reject probability model
	"dynamics",    // the kind of dynamics to force on the model
*/

sToken tokenise(std::string s, char delim, char stop) {
	sToken tokens;
	std::string st = "";
	for (int i = 0; i < s.size(); i++) {
		char ch = s.at(i);
		if (ch == stop) break;
		if (ch == delim && st != "") {
			tokens.push_back(st);
			st = "";
		} else
			st += ch;
	}
	if (st != "") tokens.push_back(st);
	return tokens;
}

bool deal_with(sToken const& tokens, Context* ctx) {
	std::string keyword = tokens[0];
	if (keyword == "name") ctx->saveDir = tokens[1]; else
	if (keyword == "kB")   ctx->BoltzConstant = std::stof(tokens[1]); else
	if (keyword == "interaction") { "ctx->bulkAA, bulkBB, bulkAB; return false if can't read all;"; } else
	if (keyword == "box") { if (!readSize(tokens, &(ctx->size))) return false; } else
	if (keyword == "surface") { "do similar like above to create vector of surfaces to create"; } else
	if (keyword == "temperature") { if (!readTemp(tokens, &(ctx->Temperature))) return false; } else
	if (keyword == "ensemble") ctx->Ensemble_Size = (uIndx) std::stoi(tokens[1]); else
	if (keyword == "ar_prob") { if (!readProb(tokens, &(ctx->Transition))) return false; } else
	if (keyword == "dynamics") { if (!readKinet(tokens, &(ctx->SpinKinetics))) return false; }
}

bool readSize(sToken const& tokens, vec3<uIndx>* L) {
	if (tokens.size() < 4) return false;
	L->x = (uIndx) std::stoi(tokens[1]);
	L->y = (uIndx) std::stoi(tokens[2]);
	L->z = (uIndx) std::stoi(tokens[3]);
	return true;
}

bool readTemp(sToken const& tokens, std::vector<float>* T) {
	if (tokens.size() < 2) return false;
	std::string st = "";
	if (tokens.size() > 2)
		for (int i = 1; i < tokens.size(); i++)
			st += tokens[i];
	else
		st = tokens[1];

	float dT;
	sToken values = tokenise(st, ':', '\n');
	switch (values.size()) {
	case 0:
		T->push_back(std::stof(st));
		break;
	case 1:
		dT = 0.1f;    // default interval value
		break;
	case 2:
		dT = std::stof(values[1]);
		break;
	default:
		std::cout << "Temperature range is not defined properly." << std::endl;
		return false;
	}
	float Ti = std::stof(*(values.begin()));
	float Tf = std::stof(*(values.end()-1));
	float v = Ti;
	while (v <= Tf) {
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
	else if (tokens[2] == "exchange")
		*d = Dynamics::EXCHANGE;
	else {
		std::cout << "Unrecognised dynamics model." << std::endl;
		return false;
	}
	return true;
}

static bool initiated = false;

bool init_system(std::string filename, Context* ctx) {
	if (initiated) {
		std::cout << "Attempt to overwrite system data. Ignoring attempt." << std::endl;
		ctx = nullptr;
		return false;
	}

	std::ifstream input_file(filename);
	if (!input_file.is_open()) {
		std::cout << "Cannot read input file. Exiting." << std::endl;
		ctx = nullptr;
		return false;
	}

	static const char DELIMITER = ' ';
	static const char STOP_CHAR = '#';

	sToken tokens;
	std::string line;
	bool success = false;
	while(getline(input_file, line))
		success &= deal_with(tokenise(line, DELIMITER, STOP_CHAR), ctx);
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
