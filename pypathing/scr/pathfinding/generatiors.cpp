#include "generatiors.h"
#include "Cluster.h"
#include "node.h"
#include "scr/sort.h"
#include <list>


Maze::Maze(int x_size, int y_size) {
	std::vector<unsigned short int> _temp = {};
	for (int x = 0; x < y_size; x++) {
		_temp.push_back(0);
	}
	for (int x = 0; x < x_size; x++) {
		this->board.push_back(_temp);
	}
}

std::set<std::pair<unsigned int, unsigned int>> prim_get_allowed_directions(Maze m, std::pair<unsigned int, unsigned int> pos, unsigned short int val, unsigned short int dis)
{
	std::set<std::pair<unsigned int, unsigned int>> dirs = {};

	if (pos.first > 1) {
		if (m.board[pos.first - 2][pos.second] == val) {
			dirs.insert(std::make_pair(pos.first - dis, pos.second));
		}
	}
	if (pos.second > 1) {
		if (m.board[pos.first][pos.second - 2] == val) {
			dirs.insert(std::make_pair(pos.first, pos.second - dis));
		}
	}

	if (pos.first < m.board.size() - 1) {
		if (m.board[pos.first + 2][pos.second] == val) {
			dirs.insert(std::make_pair(pos.first + dis, pos.second));
		}
	}
	if (pos.second < m.board[0].size() - 1) {
		if (m.board[pos.first][pos.second + 2] == val) {
			dirs.insert(std::make_pair(pos.first, pos.second + dis));
		}
	}
	return dirs;
}

Maze getConections(Maze m, std::pair<unsigned int, unsigned int> pos, int zeda)
{
	auto poses = prim_get_allowed_directions(m, pos, 1, 1);
	if (poses.size() == 0) { return m; };
	bool Continue = true;
	while (Continue && poses.size() > 0) {
		auto number = poses.begin();
		for (unsigned short int x = 0; x < rand() % poses.size(); x++) { number++; };
		std::pair<unsigned int, unsigned int> val = *number;
		poses.erase(val);
		m.board[val.first][val.second] = 1;
		if (rand() % 100 > zeda + 1) {
			Continue = false;
		}
	}
	return m;
}

std::list<int> prim_Generator_C(int x_size, int y_size, int seed, int zada)
{
	// build a maze using the prim algorithem
	std::srand(seed);
	Maze maze(x_size, y_size);
	std::set<std::pair<unsigned int, unsigned int>> toCompute = { std::make_pair<unsigned int, unsigned int>(0,0) };
	while (toCompute.size() > 0) {
		auto value = toCompute.begin();
		for (int r = rand() % toCompute.size(); r != 0; r--) {
			value++;
		}
		std::pair<unsigned int, unsigned int> position = *value;
		toCompute.erase(position);
		maze.board[position.first][position.second] = 1;
		auto positions = prim_get_allowed_directions(maze, position);
		for (auto val = positions.begin(); val != positions.end(); val++) {
			toCompute.insert(*val);
		}
		maze = getConections(maze, position, zada);

	}
	std::list<int> res = {};
	for (auto iter = maze.board.begin(); iter != maze.board.end(); iter++) {
		std::vector<unsigned short int> val = *iter;
		for (auto xiter = val.begin(); xiter != val.end(); xiter++) {
			res.push_back(static_cast<int>(*xiter));
		}
	}

	return res;
}



bool primNodeGreater(edge*, edge*) {
	return true;
}
Cluster* prim_Generator_C(Cluster* clus, PathNode* start)
{
	throw "not implemented";
	Cluster* primClus = new Cluster();
	
	std::list<PathNode*> knownNodes = { start };
	std::list<edge*> unkonwEdges;

	return primClus;
}
