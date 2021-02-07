#include "distance.h"
#include <cmath>



bool isWalkable(size_t x, size_t y, size_t z, std::vector<std::vector<std::vector<int>>> arr) {
	if (z == 0) { return true; }
	return (arr[z - 1][y][x] == 0);
}

// distance internal computer
float distance::distance(PathNode* a, PathNode* b, int key) {
	if (key == 0) {
		return distance::diagonal(a, b);
	}
	if (key == 1) {
		return distance::fastDiagonal(a, b);
	}
	if (key == 2) {
		return distance::manhattan(a, b);
	}
	return 1;
}

// diagonal distance
float distance::fastDiagonal(PathNode* a, PathNode* b) {
	if (a->pos.size() != b->pos.size()) {
		return (float)1.0;
	}
	auto a_iter = a->pos.begin();
	auto b_iter = b->pos.begin();
	float distance = 0.0;
	for (; a_iter != a->pos.end(); a_iter++) {
		distance = distance + pow((*a_iter) - (*b_iter), 2);
		b_iter++;
	}
	return distance;
}
float distance::diagonal(PathNode* a, PathNode* b) {
	return sqrt(distance::fastDiagonal(a, b));
}
float distance::manhattan(PathNode* a, PathNode* b) {
	if (a->pos.size() != b->pos.size()) {
		return (float)1.0;
	}
	auto a_iter = a->pos.begin();
	auto b_iter = b->pos.begin();
	float distance = 0.0;
	for (; a_iter != a->pos.end(); a_iter++) {
		distance = distance + abs((*a_iter) - (*b_iter));
		b_iter++;
	}
	return distance;
}



std::vector<std::tuple<int, int, int>> movements::movements(int key) {
	if (key == 0) {
		return movements::noDiagonal();
	}
	if (key == 1) {
		return movements::fullDiagonal();
	}
	if (key == 2) {
		return movements::oneMovementDiagonal();
	}
	if (key == 3) {
		return movements::towMovementDiagonal();
	}
	return {};
}
std::vector<std::tuple<int, int, int>> movements::noDiagonal() {
	return { {0,0,1},{0,0,-1},{0,1,0},{0,-1,0},{1,0,0},{-1,0,0},
	{0,1,1}, {0,-1,1}, {1,0,1}, {-1,0,1} ,
	{0,1,-1},{0,-1,-1},{1,0,-1},{-1,0,-1} };
}
std::vector<std::tuple<int, int, int>> movements::fullDiagonal() {
	return { {1,1,1},{1,1,0},{1,1,-1},{1,0,1},{1,0,0},{1,0,-1},{1,-1,1},{1,-1,0},{1,-1,-1},{0,1,1},{0,1,0},{0,1,-1},{0,0,1},{0,0,-1},{0,-1,1},{0,-1,0},{0,-1,-1},{-1,1,1},{-1,1,0},{-1,1,-1},{-1,0,1},{-1,0,0},{-1,0,-1},{-1,-1,1},{-1,-1,0},{-1,-1,-1} };
}
std::vector<std::tuple<int, int, int>> movements::oneMovementDiagonal() {
	return movements::fullDiagonal();
}
std::vector<std::tuple<int, int, int>> movements::towMovementDiagonal() {
	return movements::fullDiagonal();
}

bool movements::furtherMovement(int key, std::vector<std::vector<std::vector<int>>> arr, std::tuple<int, int, int>pos, std::tuple<int,int,int> direction) {
	if (key == 0) {
		return movements::noDiagonal(arr, pos, direction);
	}
	if (key == 1) {
		return movements::fullDiagonal(arr, pos, direction);
	}
	if (key == 2) {
		return movements::oneMovementDiagonal(arr, pos, direction, 1);
	}
	if (key == 3) {
		return movements::oneMovementDiagonal(arr, pos, direction, 2);
	}
	if (key == 4) {
		return movements::oneMovementDiagonal(arr, pos, direction, 3);
	}
	return false;
}
bool movements::noDiagonal(std::vector<std::vector<std::vector<int>>> arr, std::tuple<int, int, int>pos, std::tuple<int, int, int> direction) {
	return true;
}
bool movements::fullDiagonal(std::vector<std::vector<std::vector<int>>> arr, std::tuple<int, int, int>pos, std::tuple<int, int, int> direction) {
	return true;
};
bool movements::oneMovementDiagonal(std::vector<std::vector<std::vector<int>>> arr, std::tuple<int, int, int>pos, std::tuple<int, int, int> direction, int nums) {
	int counter = 0;
	int x, y, z;
	for (int iter = 0; iter < 3; iter++) {
		x = std::get<0>(pos);
		y = std::get<1>(pos);
		z = std::get<2>(pos);
		if (iter == 0) {
			x = x + std::get<0>(direction);
		}
		if (iter == 1) {
			y = y + std::get<1>(direction);
		}
		if (iter == 2) {
			z = z + std::get<2>(direction);
		}
		if (x >= 0 && y >= 0 && z >= 0 && x < arr.size() && y < arr[0].size() && z < arr[0][0].size()) {
			if (arr[x][y][z] == 0) {
				counter++;
			}
		}

	}
	return counter < nums;
}
