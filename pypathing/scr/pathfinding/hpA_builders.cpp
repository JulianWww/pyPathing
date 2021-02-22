#include "hpA_builders.h"

size_t utils::buildNewPos(long long x, long long y, long long z, std::tuple<int, int, int> pos, std::vector<std::vector<std::vector<int>>> arr) {
	if (std::get<0>(pos) + x >= 0 && std::get<1>(pos) + y >= 0 && std::get<2>(pos) + z >= 0 && std::get<0>(pos) + x < (int)arr[0][0].size() && std::get<1>(pos) + y < (int)arr[0].size() && std::get<2>(pos) + z < (int)arr.size()) {
		return std::get<0>(pos) + x + (std::get<1>(pos) + y) * arr[0][0].size() + (std::get<2>(pos) + z) * arr[0][0].size() * arr[0].size() + 1;
	}
	else { return 0; }
}
bool utils::isWalkable(size_t x, size_t y, size_t z, std::vector<std::vector<std::vector<int>>> arr) {
	if (z == 0) { return true; }
	return (arr[z - 1][y][x] == 0);
}