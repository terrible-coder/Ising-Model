#include "specs.hpp"

bool initiated = false;

void trim(std::string& str) {
	str.erase(0, str.find_first_not_of(" \t"));
	str.erase(str.find_last_not_of(" \t")+1);
	// return str;
}

void extract_values(std::string txt, std::string* first, std::string* second) {
	int ePos = txt.find("=");
	*first = txt.substr(0, ePos);
	trim(*first);
	*second = txt.substr(ePos+1);
	trim(*second);
}

TransProb readTrans(std::string str) {
	if (str == "boltzmann") return TransProb::BOLTZMANN;
	if (str == "glauber"  ) return TransProb::GLAUBER;
}

Dynamics readKinet(std::string str) {
	if (str == "flip"    ) return Dynamics::FLIP;
	if (str == "exchange") return Dynamics::EXCHANGE;
}

int countOccurence(std::string str, char c) {
	int count = 0;
	for (char ch : str)
		if (ch == c)
			count++;

	return count;
}

double* readTemp(std::string str) {
	double Ti, Tf, interval;
	// interpret above values from str using : delimiter
	int points;
	int colonCount = countOccurence(str, ':');
	if (colonCount == 0) {
		Ti = std::stod(str);
		interval = 0;
		Tf = Ti;
		points = 1;
	} else
	if (colonCount == 1) {
		int cPos = str.find(':');
		std::string first = str.substr(0, cPos);
		std::string second = str.substr(cPos + 1);
		Ti = std::stod(first);
		interval = 0.1; // default interval value;
		Tf = std::stod(second);
		points = static_cast<int> ((Tf - Ti) / interval);
	} else
	if (colonCount == 2) {
		int cPos1 = str.find_first_of(':');
		int cPos2 = str.find_last_of(':');
		std::string first = str.substr(0, cPos1);
		std::string second = str.substr(cPos1+1, cPos2);
		std::string third = str.substr(cPos2+1);
		Ti = std::stod(first);
		interval = std::stod(second);
		Tf = std::stod(third);
		points = static_cast<int> ((Tf - Ti) / interval);
	}

	double* T = (double*) malloc(points * sizeof(double));
	for (int i = 0; i < points; i++)
		T[i] = Ti + i * interval;
	return T;
}

void init_system(std::string filename, Specifications* S) {
	// allow system to be initialised only once. any second attempt to initialise
	// system variables during the run will simply be ignored.
	if (initiated)
		return;
	initiated = true;

	std::ifstream input_file(filename);
	if (!input_file.is_open())
		std::cout << "Cannot read input file.";

	std::string line;
	while(getline(input_file, line)) {
		if (line == "") continue;
		if (line.at(0) == '#') continue; // allowing comments
		// separate parts before and after "="
		std::string key, value;
		extract_values(line, &key, &value);
		if (key == "kB"         ) S->BoltzConstant = std::stod(value); else
		if (key == "Coupling"   ) S->Coupling      = std::stod(value); else
		if (key == "Field"      ) S->Field         = std::stod(value); else
		if (key == "probability") S->Transition    = readTrans(value); else
		if (key == "dynamics"   ) S->SpinKinetics  = readKinet(value); else
		if (key == "temperature") S->Temperature   = readTemp (value); else
		if (key == "Lx"         ) S->Lx            = std::stoi(value); else
		if (key == "Ly"         ) S->Ly            = std::stoi(value); else
		if (key == "scale"      ) S->scale         = std::stoi(value);
	}

	std::cout << "Temp: " << S->Temperature[0] << "\n";
	std::cout << "Lx: " << S->Lx << "\n";
	std::cout << "Ly: " << S->Ly << "\n";
	std::cout << "Coupling: " << S->Coupling << "\n";
	std::cout << std::endl;

	input_file.close();
}
