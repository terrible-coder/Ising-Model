#include "io/data_logger.hpp"

typedef std::vector<std::ofstream> ofVec;

/**
 * @brief Hamiltonian output stream.
 */
std::ofstream energyData;
/**
 * @brief Magnetisation output stream.
 */
std::ofstream magnetData;

static int LogType;

void logOnly(std::vector<std::string> tokens) {
	LogType = 0;
	for (auto it = tokens.begin(); it != tokens.end(); it++) {
		if (*it == "energy")
			LogType |= LogData::ENERGY;
		else if (*it == "order")
			LogType |= LogData::ORDER_PARAM;
		else
			std::cout << "Ignoring unrecognised log request \"" << *it << "\"" << std::endl;
	}
}

void openLogger(std::string parentDir, float T, int rank) {
	if (energyData.is_open() || magnetData.is_open()) {
		std::cout << "The previous data files are still open. You should close that." << std::endl;
		return;
	}

	// file name contains information about temperature
	std::string path = parentDir + "Temp" + std::to_string(T) + "/";
	switch (LogType) {
		case LogData::ENERGY:
			energyData.open(path + "energy" + std::to_string(rank) + DATA_EXT);
			break;
		case LogData::ORDER_PARAM:
			magnetData.open(path + "magnet" + std::to_string(rank) + DATA_EXT);
			break;
		case LogData::ENERGY | LogData::ORDER_PARAM:
			energyData.open(path + "energy" + std::to_string(rank) + DATA_EXT);
			magnetData.open(path + "magnet" + std::to_string(rank) + DATA_EXT);
			break;
	}
}

void closeLogger() {
	energyData.close();
	magnetData.close();
}

void logEnergy(float energy) {
	if (!energyData.is_open()) {
		std::cout << "Energy file not open. Data not logged." << std::endl;
		return;
	}
	energyData << energy << ",";
}

void logMagnet(float magnet) {
	if (!magnetData.is_open()) {
		std::cout << "Magnet file not open. Data not logged." << std::endl;
		return;
	}
	magnetData << magnet << ",";
}

void createLog(Ising& c) {
	switch (LogType) {
		case LogData::ENERGY:
			logEnergy(c.Hamiltonian());
			break;
		case LogData::ORDER_PARAM:
			logMagnet(c.Magnetisation());
			break;
		case LogData::ENERGY | LogData::ORDER_PARAM:
			logEnergy(c.Hamiltonian());
			logMagnet(c.Magnetisation());
			break;
	}
}

void nextEnsemble() {
	energyData << std::endl;
	magnetData << std::endl;
}
