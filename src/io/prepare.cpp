#include "io/prepare.hpp"

bool safeCreate(std::string path) {
	return std::filesystem::exists(path) || std::filesystem::create_directories(path);
}

std::string prepExperiment(Context* ctx) {
	bool success = true;

	std::string parent = ctx->saveDir;
	std::string snaps = "snaps/";
	std::string frame = "frames/";

	for (auto it = ctx->Temperature.begin(); it != ctx->Temperature.end(); it++) {
		std::string T = "Temp" + std::to_string(*it) + "/";
		success = success && safeCreate(parent+T+snaps);
		success = success && safeCreate(parent+T+frame);

		for (int j = 0; (j < ctx->Ensemble_Size) && success; j++)
			success = success && safeCreate(parent+T+frame+"En"+std::to_string(j+1));

		if (!success) {
			std::cout << "Could not prepare directories properly" << std::endl;
			return "";
		}
	}
	return parent;
}
