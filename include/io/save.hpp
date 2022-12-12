#include <cstdint>
#include <string>
#include <iostream>
#include <fstream>

#include "defaults.hpp"
#include "Ising.hpp"

#define snaps "snaps/"
#define frames "frames/"

void openSnap(Ising& config, int en, std::string parentDir);
bool takeSnap(Ising& config);
void closeSnap();
bool saveInit(Ising& config, std::string parentDir);
