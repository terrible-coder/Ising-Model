#include "params.hpp"

std::string& trim(std::string& str) {
	str.erase(0, str.find_first_not_of(" \t"));
	str.erase(str.find_last_not_of(" \t")+1);
	return str;
}

params readInput(std::string filename) {
	params p;
	std::string line;
	std::ifstream inputFile(filename);
	if (inputFile.is_open()) {
		while (getline(inputFile, line)) {
			if (line == "")
				continue;
			int ePos = line.find("=");
			std::string key = line.substr(0, ePos);
			std::string sVal = line.substr(ePos+1);
			trim(key);
			trim(sVal);
			std::cout << key << ": " << sVal << std::endl;
			// double val = std::stod(sVal);
			if      (key == "kB") p.kB = std::stod(sVal);
			else if (key == "T")  p.T  = std::stod(sVal);
			else if (key == "J")  p.J  = std::stod(sVal);
			else if (key == "H")  p.H  = std::stod(sVal);
			else if (key == "width")  p.width  = std::stoi(sVal);
			else if (key == "height") p.height = std::stoi(sVal);
			else if (key == "scale")  p.scale  = std::stoi(sVal);
			else if (key == "boundary") {
				if      (sVal == "periodic") p.boundary = 0;
				else if (sVal == "screw")    p.boundary = 1;
				else if (sVal == "free")     p.boundary = 2;
			}
		}
		inputFile.close();
	} else std::cout << "No such file." << std::endl;

	return p;
}
