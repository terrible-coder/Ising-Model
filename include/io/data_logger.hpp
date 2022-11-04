#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "defaults.hpp"

void openLogger(std::string parentDir, float T);
void closeLogger();

void logData(float energy, float magnet);
void nextEnsemble();
