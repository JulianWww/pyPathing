#pragma once



namespace distance {
	int manhattenDistance(std::pair<int, int> current, std::pair<int, int>end) {
		return abs(current.first - end.first) + abs(current.second - end.second);
	}
	int diagonalDistance(std::pair<int, int> current, std::pair<int, int>value) {
		return (value.first - current.first) * (value.first - current.first) + (value.second - current.second) * (value.second - current.second);
	}
}