#pragma once
#include <set>
#include <vector>
#include <list>
#include <iostream>

class Cluster;
class PathNode;


class Maze {
	public: std::vector<std::vector<unsigned short int>> board = {};
	// the vector containing the values of the mapt for a simple 2d maze or other inveronment
	Maze(int x_size, int y_size);
};

std::set<std::pair<unsigned int, unsigned int>> prim_get_allowed_directions(Maze m, std::pair<unsigned int, unsigned int> pos, unsigned short int val = 0, unsigned short int dis = 2);

Maze getConections(Maze m, std::pair<unsigned int, unsigned int> pos, int zeda);


std::list<int> prim_Generator_C(int x_size, int y_size, int seed = 0, int zada = 10);

Cluster* prim_Generator_C(Cluster* clus, PathNode* start);