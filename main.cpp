#include <iostream>
#include <fstream>
#include <string>
#include <SFML/Graphics.hpp>

int main() {
	std::string line;
	std::ifstream params("params.txt");
	if (params.is_open()) {
		while (getline(params, line))
			if (line != "")
				std::cout << line << std::endl;
		params.close();
	} else std::cout << "No such file." << std::endl;
	return 0;
}
