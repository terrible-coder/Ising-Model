#include "context.hpp"

/**
 * @brief Keep track if the system has been initialised. This flag allows the
 * system to be initialised only once.
 */
bool initiated = false;

/**
 * @brief Trim trailing and leading white spaces (tabs and spaces).
 * 
 * @param str 
 */
void trim(std::string& str) {
	str.erase(0, str.find_first_not_of(" \t"));
	str.erase(str.find_last_not_of(" \t")+1);
}

/**
 * @brief Interpret a key-value pair from a given line of text. Uses the "="
 * character as delimiter. The function assumes single occurrence of "="
 * character in the text.
 * 
 * @param txt The line of text.
 * @param first The part before "=".
 * @param second The part after "=".
 */
void extract_values(std::string txt, std::string* first, std::string* second) {
	int ePos = txt.find("=");
	*first = txt.substr(0, ePos); trim(*first);
	*second = txt.substr(ePos+1); trim(*second);
}

/**
 * @brief Interpret transition probability type from string.
 * 
 * @param str 
 * @return TransProb 
 */
TransProb readTrans(std::string str) {
	if (str == "boltzmann") return TransProb::BOLTZMANN;
	if (str == "suku"     ) return TransProb::SUZU_KUBO;
}

/**
 * @brief Interpret the type of dynamics for the Ising model.
 * 
 * @param str 
 * @return Dynamics 
 */
Dynamics readKinet(std::string str) {
	if (str == "flip"    ) return Dynamics::FLIP;
	if (str == "exchange") return Dynamics::EXCHANGE;
}

/**
 * @brief Count the occurrence of a character in a string.
 * 
 * @param str The string to search in.
 * @param c The character to search for.
 * @return int 
 */
int countOccurrence(std::string str, char c) {
	int count = 0;
	for (char ch : str)
		count += (ch == c);
	return count;
}

/**
 * @brief Interpret the temperature range from given string.
 * 
 * @param str The string to interpret. 
 * @return std::vector<double> 
 */
std::vector<double> readTemp(std::string str) {
	double Ti, Tf, interval;
	// interpret above values from str using : delimiter
	int points;
	int colonCount = countOccurrence(str, ':');
	if (colonCount == 0) {		// single temperature value
		Ti = std::stod(str);
		interval = 0;
		Tf = Ti;
		points = 1;
	} else
	if (colonCount == 1) {		// range defined with default interval of 0.1
		int cPos = str.find(':');
		std::string first = str.substr(0, cPos);
		std::string second = str.substr(cPos + 1);
		Ti = std::stod(first);
		interval = 0.1; // default interval value;
		Tf = std::stod(second);
		points = (int) ((Tf - Ti) / interval + 0.5);
	} else
	if (colonCount == 2) {		// range defined with given interval
		int cPos1 = str.find_first_of(':');
		int cPos2 = str.find_last_of(':');
		std::string first = str.substr(0, cPos1);
		std::string second = str.substr(cPos1+1, cPos2);
		std::string third = str.substr(cPos2+1);
		Ti = std::stod(first);
		interval = std::stod(second);
		Tf = std::stod(third);
		points = (int) ((Tf - Ti) / interval + 0.5);
	}
	std::vector<double> T;
	T.reserve(points);

	for (int i = 0; i < points; i++)
		T.push_back(Ti + i * interval);
	return T;
}

/**
 * @brief Initialise a system from input file. This function should be called at
 * before initialising any configurations and this function should be called
 * only once.
 * 
 * @param filename The input file path.
 * @param ctx The system settings to initialise.
 */
void init_system(std::string filename, Context* ctx) {
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
		if (key == "ensemble"   ) ctx->ENSEMBLE_SIZE = std::stoi(value); else
		if (key == "kB"         ) ctx->BoltzConstant = std::stod(value); else
		if (key == "Coupling"   ) ctx->Coupling      = std::stod(value); else
		if (key == "Field"      ) ctx->Field         = std::stod(value); else
		if (key == "probability") ctx->Transition    = readTrans(value); else
		if (key == "dynamics"   ) ctx->SpinKinetics  = readKinet(value); else
		if (key == "temperature") ctx->Temperature   = readTemp (value); else
		if (key == "Lx"         ) ctx->Lx            = (std::uint16_t) std::stoi(value); else
		if (key == "Ly"         ) ctx->Ly            = (std::uint16_t) std::stoi(value);
	}

	std::cout << "Lx: " << ctx->Lx << "\n";
	std::cout << "Ly: " << ctx->Ly << "\n";
	std::cout << "Coupling: " << ctx->Coupling << "\n";
	std::cout << "Temperature points: " << ctx->Temperature.size() << "\n";
	std::cout << std::endl;

	input_file.close();
}
