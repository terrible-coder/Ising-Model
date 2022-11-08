#include <string>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

#include "defaults.hpp"
#include "../context.hpp"

/**
 * @brief Tokenise a string using given delimiter.
 * 
 * @param s The string to tokenise.
 * @param delim The delimiting character.
 * @param stop Tokenising stops after first occurrence of this character.
 * @return std::vector<std::string> 
 */
std::vector<std::string> tokenise(std::string s, char delim, char stop);

/**
 * @brief Initialise the simulation. The context settings are read from a file
 * containing key-value pairs representing the values to be used.
 * 
 * @param filename 
 * @param ctx 
 * @return true false
 */
bool init_system(std::string filename, Context* ctx);

/**
 * @brief Create directory structure for an experiment.
 * 
 * @param ctx 
 * @return true false 
 */
bool prepExperiment(Context* ctx);
