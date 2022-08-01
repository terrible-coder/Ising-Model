#include "io/prepare.hpp"

std::string parentDir(int w, int h, int en) {
	return DATA_DIR_PREFIX
			 + std::to_string(w) + "x" + std::to_string(h)
			 + "en" + std::to_string(en) + "/";
}

bool safeCreate(std::string path) {
	return std::filesystem::exists(path) || std::filesystem::create_directories(path);
}

std::string prepExperiment(Context* ctx) {
	bool success = true;

	std::string parent = parentDir(ctx->Lx, ctx->Ly, ctx->ENSEMBLE_SIZE);
	std::string snaps = "snaps/";
	std::string frame = "frames/";

	for (int i = 0; i < ctx->_t_points; i++) {
		std::string T = "Temp" + std::to_string(ctx->Temperature[i]) + "/";
		success = success && safeCreate(parent+T+snaps);
		success = success && safeCreate(parent+T+frame);

		for (int j = 0; j < ctx->ENSEMBLE_SIZE && success; j++)
			success = success && safeCreate(parent+T+frame+"En"+std::to_string(j+1));

		if (!success) {
			std::cout << "Could not prepare directories properly" << std::endl;
			return "";
		}
	}
	return parent;
}
