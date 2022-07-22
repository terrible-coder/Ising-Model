#include "io/data_logger.hpp"

/**
 * @brief Hamiltonian output stream.
 */
std::ofstream energyData;
/**
 * @brief Magnetisation output stream.
 */
std::ofstream magnetData;

void openLogger(int w, int h, int en, double T) {
	static bool log_folder = false;

	if (energyData.is_open() || magnetData.is_open()) {
		std::cout << "The previous data files are still open. You should close that." << std::endl;
		return;
	}

	// folder name contains information about experiment
	std::string EN = std::to_string(en);
	std::string Lx = std::to_string(w);
	std::string Ly = std::to_string(h);
	std::string folder = DATA_DIR_PREFIX + Lx + "x" + Ly + "en" + EN + "/";

	// file name contains information about temperature
	std::string name;
	name = folder + "energy" + std::to_string(T) + DATA_EXT;
	energyData.open(name);

	name = folder + "magnet" + std::to_string(T) + DATA_EXT;
	magnetData.open(name);

	if (!log_folder) {
		log_folder = true;
		std::cout << "Outputting to folder: " << folder << std::endl;
	}
}

void closeLogger() {
	energyData.close();
	magnetData.close();
}

void logData(double energy, double magnet) {
	if (!energyData.is_open() || !magnetData.is_open()) {
		std::cout << "File(s) not open. Data not logged." << std::endl;
		return;
	}
	energyData << energy << ",";
	magnetData << magnet << ",";
}

void nextEnsemble() {
	energyData << std::endl;
	magnetData << std::endl;
}
