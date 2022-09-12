#include <cstdint>
#include <string>
#include <iostream>
#include <fstream>

#include "defaults.hpp"
#include "Ising.hpp"

#define snaps "snaps/"
#define frames "frames/"

void open(Ising* config, int en, std::string parentDir);
bool snap(Ising* config);
void close();
bool saveInit(Ising* config, std::string parentDir);
