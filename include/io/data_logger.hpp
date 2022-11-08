#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "defaults.hpp"
#include "Ising.hpp"

/**
 * @brief Indicates which types of data to store.
 */
enum LogData {
	ENERGY = 1,
	ORDER_PARAM = 2
};

void logOnly(std::vector<std::string> tokens);

void openLogger(std::string parentDir, float T);
void closeLogger();

void createLog(Ising& c);
void nextEnsemble();
