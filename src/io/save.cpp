#include "io/save.hpp"

static int _last_en = -1;
std::ofstream evolution;

void open(Ising& config, int en, std::string parentDir) {
	if (en != _last_en)
		_last_en = en;

	std::string T = "Temp" + std::to_string(config.getTemp()) + "/";
	std::string path = parentDir + T + snaps + "En"
									 + std::to_string(en) + BIN_EXT;
	evolution.open(path, std::ios::binary);
	if (!evolution) {
		std::cout << "Could not open file: " + path << std::endl;
		return;
	}
	uIndx Lx = config.getSizeX();
	uIndx Ly = config.getSizeY();
	uIndx Lz = config.getSizeZ();
	evolution.write((char*) &Lx, sizeof(uIndx));
	evolution.write((char*) &Ly, sizeof(uIndx));
	evolution.write((char*) &Lz, sizeof(uIndx));
}

bool snap(Ising& config) {
	uIndx i, rawSize = (uIndx)(config.getSize() / WORD_SIZE);
	uWord* rawSpins = config.getRaw();
	uWord number;
	for(i = 0; i < rawSize; i += 1u) {
		number = rawSpins[i];
		if (!evolution.write((char*) &number, sizeof(uWord)))
			break;
	}
	if (i < rawSize) {
		std::cout << "Something went wrong in writing." << std::endl;
		return false;
	}
	return true;
}

void close() {
	evolution.close();
}

bool saveInit(Ising& config, std::string parentDir) {
	std::string T = "Temp" + std::to_string(config.getTemp()) + "/";
	std::string path = parentDir + T + "initial" + BIN_EXT;
	std::ofstream initial(path, std::ios::binary);
	if (!initial) {
		std::cout << "Could not open file: " + path << std::endl;
		return false;
	}
	uIndx Lx = config.getSizeX();
	uIndx Ly = config.getSizeY();
	uIndx Lz = config.getSizeZ();
	initial.write((char*) &Lx, sizeof(uIndx));
	initial.write((char*) &Ly, sizeof(uIndx));
	initial.write((char*) &Lz, sizeof(uIndx));
	uint i;
	uWord* rawSpins = config.getInit();
	uWord number;
	for(i = 0; i < config.getSize() / WORD_SIZE; i++) {
		number = rawSpins[i];
		if (!initial.write((char*) &number, sizeof(uWord)))
			break;
	}
	if (i < config.getSize() / WORD_SIZE) {
		std::cout << "Something went wrong in writing." << std::endl;
		return false;
	}
	return true;
}
