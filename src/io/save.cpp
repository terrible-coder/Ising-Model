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
	int k = 0;
	std::uint64_t number;
	for (int i = 0; i < config->getHeight(); i++) {
		for (int j = 0; j < config->getWidth(); j++) {
			number = (number << 1) | (*config)(i, j);
			k++;
			if (k % BUFFER == 0) {
				evolution.write((char*) &number, sizeof(std::uint64_t));
				number = 0;
			}
		}
	}
	if (k != config->getWidth() * config->getHeight()) {
		std::cout << "something is not write" << std::endl;
		return false;
	}
	return true;
}

void close() {
	evolution.close();
}
