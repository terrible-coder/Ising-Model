#include <cstdint>
#include <string>
#include <iostream>
#include <fstream>

#include "../Ising.hpp"

#define snaps "snaps/"
#define frames "frames/"

#define BUFFER 32

void open(Ising* config, int en, std::string parentDir);

bool snap(Ising* config);

void close();
