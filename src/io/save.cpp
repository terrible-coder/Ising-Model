#include "io/save.hpp"

static int _last_en = -1;
std::ofstream evolution;

void open(Ising* config, int en, std::string parentDir) {
	if (en != _last_en)
		_last_en = en;

	std::string T = "Temp" + std::to_string(config->getTemp()) + "/";
	std::string path = parentDir + T + snaps + "En"
									 + std::to_string(en) + BIN_EXT;
	evolution.open(path, std::ios::binary);
	if (!evolution) {
		std::cout << "Could not open file: " + path << std::endl;
		return;
	}
	std::uint16_t w = config->getWidth();
	std::uint16_t h = config->getHeight();
	evolution.write((char*) &w, sizeof(std::uint16_t));
	evolution.write((char*) &h, sizeof(std::uint16_t));
}

bool snap(Ising* config) {
	uint i;
	uWord_t* rawSpins = config->getRaw();
	uWord_t number;
	for(i = 0; i < config->getSize() / WORD_SIZE; i++) {
		number = rawSpins[i];
		if (!evolution.write((char*) &number, sizeof(uWord_t)))
			break;
	}
	if (i < config->getSize() / WORD_SIZE) {
		std::cout << "Something went wrong in writing." << std::endl;
		return false;
	}
	return true;
}

void close() {
	evolution.close();
}
