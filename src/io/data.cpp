#include "io/data_logger.hpp"

/**
 * @brief Hamiltonian output stream.
 */
std::ofstream energyData;
/**
 * @brief Magnetisation output stream.
 */
std::ofstream magnetData;

void openLogger(std::string parentDir, float T) {
	if (energyData.is_open() || magnetData.is_open()) {
		std::cout << "The previous data files are still open. You should close that." << std::endl;
		return;
	}

	// file name contains information about temperature
	std::string path = parentDir + "Temp" + std::to_string(T) + "/";
	energyData.open(path + "energy" + DATA_EXT);
	magnetData.open(path + "magnet" + DATA_EXT);
}

void closeLogger() {
	energyData.close();
	magnetData.close();
}

void logData(float energy, float magnet) {
	if (!energyData.is_open() || !magnetData.is_open()) {
		std::cout << "File(s) not open. Data not logged." << std::endl;
		return;
	}
	energyData << energy << ",";
	// magnetData << magnet << ",";
}

void nextEnsemble() {
	energyData << std::endl;
	magnetData << std::endl;
}
