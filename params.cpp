#include "params.hpp"

params readInput(std::string filename) {
	params p{-1., -1., -1., -1.};
	std::string line;
	std::ifstream inputFile(filename);
	if (inputFile.is_open()) {
		while (getline(inputFile, line))
			if (line != "") {
				int ePos = line.find("=");
				std::string sVal = line.substr(ePos+1);
				double val = std::stod(sVal);
				if (line.rfind("kB", 0) == 0)			p.kB = val;
				else if (line.rfind("T", 0) == 0) p.T  = val;
				else if (line.rfind("J", 0) == 0) p.J  = val;
				else if (line.rfind("H", 0) == 0) p.H  = val;
				std::cout << line << "\t" << val << std::endl;
			}
		inputFile.close();
	} else std::cout << "No such file." << std::endl;

	return p;
}
