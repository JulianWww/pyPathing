#pragma once
#include <vector>
#include <set>
#include <unordered_map>
#include <tuple>
#include <math.h>
#include "distances.h"


namespace AlphaStar {
	// get somthing relating to pythegorean distance
	int estimatedDistanceToDestination(std::pair<int, int>current, std::set<std::pair<int, int>> end, bool fast) {
		// compute the estimated distance bewean the curent location and the result pos
		// get the first postion
		auto curent_pos = end.begin();
		auto value = *curent_pos;
		unsigned int cost = distance::diagonalDistance(value, current);
		// go over every other one and if its cheaper use that one
		for (curent_pos++; curent_pos != end.end(); curent_pos++) {
			value = *curent_pos;
			unsigned int newCost = abs(value.first - current.first) + abs(value.second - current.second);
			if (newCost < cost) { cost = newCost; }
		}
		if (fast) {
			return cost;
		}
		return sqrt(cost);
	};
}

std::vector<std::pair<int, int>> Dijkstra_c(std::vector<std::vector<int>> maze1D_l, std::pair<int,int> start_Pos, std::set<std::pair<int,int>> endPos_vec, bool scanned=false) {
	//auto movementPositions_l = static_cast<std::vector<std::vector<int>>>(maze1D_l);

	std::vector<std::pair<int, int>> activePositions = { start_Pos };
	std::vector<int> postionalInitiater = {0};
	int currentVal = 0;
std::vector<std::pair<int, int>> archivedPositions = { start_Pos };
std::vector<std::pair<int, int>> tempPoses = {};
std::vector<std::pair<int, int>> movement = { {0, 1},
											  {1, 0},
											  {0,-1},
											  {-1,0} };
std::pair<int, int>sizes = { maze1D_l.size(), maze1D_l[0].size() };
while (activePositions.size() > 0) {
	for (auto currentPos = activePositions.begin(); currentPos != activePositions.end(); currentPos++) {
		std::pair<int, int> currentPosition = *currentPos;
		// go in every dierection
		for (auto direnction = movement.begin(); direnction != movement.end(); direnction++) {
			std::pair<int, int> directionalVector = *direnction;
			// make pair from constructor


			auto newPos = std::make_pair(directionalVector.first + currentPosition.first, directionalVector.second + currentPosition.second);
			// get the new position
			if (std::find(archivedPositions.begin(), archivedPositions.end(), newPos) == archivedPositions.end() && newPos.first >= 0 && newPos.second >= 0 && newPos.first < sizes.first && newPos.second < sizes.second) {
				// if the position is not blocked by a wall
				if (maze1D_l[newPos.first][newPos.second] == 1) {
					//check if we already calculated the position or will in this round
					tempPoses.push_back(newPos);
					// also add it to archive and along with the one how built it
					archivedPositions.push_back(newPos);
					postionalInitiater.push_back(currentVal);

					if (endPos_vec.find(newPos) != endPos_vec.end()) {
						if (scanned) {
							return archivedPositions;
						}
						std::vector<std::pair<int, int>> pathPoses_pair = { newPos };
						int loc = currentVal;
						while (!loc == 0) {
							pathPoses_pair.push_back(archivedPositions[loc]);
							loc = postionalInitiater[loc];
						}
						pathPoses_pair.push_back(archivedPositions[0]);
						return pathPoses_pair;
					}

				}
			}
			// ad the postiion we built the new ones from to archive queue
		}
		currentVal++;
	}
	// move temp to active postions
	activePositions.clear();
	activePositions = tempPoses;
	tempPoses.clear();
}
return { {-1,-1} };
}

std::vector<std::pair<int, int>> Astar_c(std::vector<std::vector<int>> maze1D_l, std::pair<int, int> start_Pos, std::set<std::pair<int, int>> end, bool scaned=false, bool fast = true) {
	// A* algorythem for mazes saved as 0, 1 numpy arrays its basicly a grid world

	std::vector<std::pair<int, int>> visited_positions = { start_Pos };
	std::vector<int> PathMap = { 0 };
	// create the visited posion containers
	// next create the search container
	// it contais the index of the creator as well ist distance as well as its position
	std::unordered_map<int, std::vector<std::tuple<int, int, std::pair<int, int>>>> scannerQueue = { {0, {{0, 0, start_Pos}}} };

	// get some global data about the maze
	std::vector<std::pair<int, int>> movement = { {0, 1},
												  {1, 0},
												  {0,-1},
												  {-1,0} };
	std::pair<int, int>sizes = { maze1D_l.size(), maze1D_l[0].size() };

	int currentCost = 0;

	// run algorythem while values in queue
	while (scannerQueue.size() > 0) {
		if (scannerQueue.count(currentCost)) {
			// sann one of the lowerst prority values
			if (scannerQueue[currentCost].size() > 0) {
				// the data of the current position
				std::tuple<int, int, std::pair<int, int>> posData = scannerQueue[currentCost][scannerQueue[currentCost].size()-1];
				// remove the position from the activ search options
				scannerQueue[currentCost].pop_back();
				// iterate over the movement values
				for (auto direction_iter = movement.begin(); direction_iter != movement.end(); direction_iter++) {
					// the direction of the new Node
					std::pair<int, int> direction = *direction_iter;
					// the new position based on old data
					std::pair<int, int> newPosition = std::make_pair(std::get<2>(posData).first + direction.first,
						std::get<2>(posData).second + direction.second);
					// check weather pos in maze
					if (newPosition.first >= 0 && newPosition.second >= 0 && newPosition.first < sizes.first && newPosition.second < sizes.second) {
						// see if its unvisited and walkable
						if (maze1D_l[newPosition.first][newPosition.second] == 1 && std::find(visited_positions.begin(), visited_positions.end(), newPosition) == visited_positions.end()) {
							// comute new cost according to algorythem specs
							int newCost = AlphaStar::estimatedDistanceToDestination(newPosition, end, fast) + std::get<1>(posData);
							if (scannerQueue.count(newCost)==0) {
								scannerQueue.insert({ newCost ,{} });
							}
							// get the values of this priority
							std::vector<std::tuple<int, int, std::pair<int, int>>> priority = scannerQueue.at(newCost);
							// add new position to this prority at the end
							priority.push_back({ PathMap.size(), std::get<1>(posData) + 1 ,newPosition });
							// replace the old prority values with the updated ones
							scannerQueue.insert_or_assign(newCost, priority);
							// add this value to parent taceback
							PathMap.push_back(std::get<0>(posData));
							// state that its bean visited
							visited_positions.push_back(newPosition);
							// if the new cost is lower than the curent going vlue update that
							if (newCost < currentCost) { currentCost = newCost; };
							// if wer done traceback the nodes that lead us hear
							if (std::find(end.begin(), end.end(), newPosition) != end.end()){
								// is the nodes that where scanned where requested return those
								if (scaned) {
									return visited_positions;
								}
								/*start at the current node->
								get its dady form the pathMap by index->
								get the dady of the dady etc 
								put them all in al list and return sed list*/
								int loc = PathMap.size() - 1;
								std::vector<std::pair<int, int>> output = { {} };
								while (loc != 0) {
									output.push_back(visited_positions[loc]);
									loc = PathMap[loc];
								}
								return output;
							}	
						}
					}
				}
			}
			else {
				// if no nodes with this prority exist remove the prority
				scannerQueue.erase(currentCost);
				currentCost++;
			}
		}
		else {
			// if we cant find the prority move on
			currentCost++;
		}
	}
	// woops somthing dosnt fit so tell python as mutch
	return {{-1,-1}};
}