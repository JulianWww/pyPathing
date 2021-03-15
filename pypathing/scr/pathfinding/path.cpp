#include "path.h"
#include "node.h"
#include "Astar.h"
#include "Edge.h"
#include "updateEvent.h"
#include "distance.h"
#include <list>
#include <cmath>

// DPA*
AstarPath::AstarPath(PathNode* start, PathNode* end, int posKey, int speed)
{
	this->speed = speed;
	this->key = posKey;

	this->start = start;
	this->end = end;

	this->rebuild();

	auto const& last = *(path.end() - 1);
	this->cost = last->distance;
}

void AstarPath::rebuild(int posKey)
{
	std::list<PathNode*> nodes;
	if (speed == 0) {
		nodes = serchers::Astar_c_node(this->start, this->end, posKey);
	}
	else {
		nodes = serchers::Astar_c_node_wspeed(this->start, this->end, posKey, false, this->speed);
	}
	nodes.push_front(this->start);
	std::vector<PathNode*> path(nodes.begin(), nodes.end());
	this->path = path;
	if (this->path.size() > 0) {
		this->valid = true;
	}
	this->valid = true;
}


void DPAstarPath::update(updateEvent* update, PathNode* current, int distanceKey) {
	if (this->valid) {

		if (current == nullptr) {
			current = this->start;
		}
		this->updateDeletions(update, current, distanceKey);
		this->updateInserts(update, current, distanceKey);
	}
	else {
		this->rebuild(distanceKey);
	}
}

void DPAstarPath::cheapUpdate(updateEvent* update, PathNode* current, int distanceKey)
{
	if (this->valid) {
		if (current == nullptr) {
			current = this->path.front();
		}
		this->updateDeletions(update, current, distanceKey);
	}
	else {
		this->rebuild(distanceKey);
	}
}

void DPAstarPath::updateDeletions(updateEvent* update, PathNode* current, int distanceKey) {
	auto turningNodes = update->getEntryAndExitsDels(this);
	if (std::get<0>(turningNodes) == nullptr) {
		return;
	}
	PathNode* A = std::get<0>(turningNodes);
	PathNode* B = std::get<1>(turningNodes);
	PathNode*& goal = this->end;

	std::pair<std::list<PathNode*>, std::list<PathNode*>> validSubPathes = this->getValidSubpathes(A, B, current);

	std::priority_queue<float, std::vector<float>, std::greater<float>> distanceQueue;
	distanceQueue.push(0);
	current->distance = 0;
	current->movedFrom = nullptr;
	std::unordered_map<float, std::list<PathNode*>> nodeMap = { {0, {current} } };
	std::list<PathNode*> visited = { current };

	while (distanceQueue.size() > 0) {
		float currentDistance = distanceQueue.top();
		distanceQueue.pop();
		PathNode* currentNode = nodeMap.at(currentDistance).front();
		nodeMap.at(currentDistance).pop_front();

		if (currentNode == goal) {
			this->path = this->buildPath(currentNode);
			return;
		}

		for (auto const& currentEdge : currentNode->edges) {
			PathNode* newNode = currentEdge.first;

			if (newNode->walkable && currentEdge.second->walkable) {
				auto pos = std::find(validSubPathes.second.begin(),
					validSubPathes.second.end(), currentNode);

				if (pos != validSubPathes.second.end() && newNode != *std::next(pos)) {
					continue;
				}

				pos = std::find(validSubPathes.first.begin(),
					validSubPathes.first.end(), newNode);

				if (pos != validSubPathes.first.end() && pos != validSubPathes.first.begin() && newNode != *std::next(pos, -1)) {
					continue;
				}

				float distance = currentNode->distance + currentEdge.second->getLength(currentNode, this->speed);
				bool notcontains = (std::find(visited.begin(), visited.end(), newNode) == visited.end());
				if (distance < newNode->distance || notcontains) {
					if (notcontains) {
						visited.push_back(newNode);
					}
					newNode->distance = distance;
					newNode->movedFrom = currentNode;

					float distEstimate = distance + distance::distance(newNode, goal, distanceKey);

					distanceQueue.push(distEstimate);

					if (nodeMap.count(distEstimate) == 0) {
						nodeMap.insert({ distEstimate, std::list<PathNode*>() });
					}
					nodeMap.at(distEstimate).push_back(newNode);
				}
			}
		}
	}
	this->valid = false;
	this->path.clear();                               // should i do this??
	throw std::exception("could not find new path");
}

void DPAstarPath::updateInserts(updateEvent* update, PathNode* start, int distanceKey) {
	auto turningNodes = update->getEntryAndExitsInserts(this);
	auto validSubPaths =  this->getValidSubpathes(turningNodes.first, turningNodes.second, start);
	if (turningNodes.first == nullptr) {
		validSubPaths.second = validSubPaths.first;
	}

	std::priority_queue<float, std::vector<float>, std::greater<float>> distanceQueue;
	distanceQueue.push(0);
	start->distance = 0;
	start->movedFrom = nullptr;

	updateNode* current = new updateNode(start);
	std::unordered_map<float, std::list<updateNode*>> nodeMap = { {0, {current} } };
	std::list<PathNode*> visited = { start };

	std::unordered_map<PathNode*, updateNode*> correspondingUpdate = { {start, current } };

	PathNode* goal = this->end;

	while (distanceQueue.size() > 0) {
		float distance = distanceQueue.top();
		distanceQueue.pop();

		current = nodeMap.at(distance).front();
		nodeMap.at(distance).pop_front();

		if (current->Node == this->path.back()) {
			this->path = this->buildPath(current->Node);;
			return;
		}

		current->Rvisited = current->parent->Rvisited || update->isInDels(current->Node);
		current->Rworse = current->parent->Rworse;
		bool checkAll;

		auto Iter = std::find(validSubPaths.second.begin(), validSubPaths.second.end(), current->Node);
		if (Iter == validSubPaths.second.end()) {
			checkAll = true;
		}
		else {
			current->Rworse = current->parent->Rworse || (current->Node->distance
														  + update->distance(current->Node, distanceKey)
														  + update->distance(goal, distanceKey)) > this->cost;
			
			if (!current->Rworse) {
				checkAll = true;
			}
			else if (Iter != validSubPaths.second.end()) {
				checkAll = false;
			}
			else {
				continue;
			}

		}
	
		for (auto const& currentEdge : current->Node->edges) {
			if (current->Node->walkable && currentEdge.second->walkable) {
				if (!checkAll && Iter != validSubPaths.second.begin() && currentEdge.first != *std::next(Iter, -1)) {
					continue;
				}
				float edgeLength = currentEdge.second->getLength(currentEdge.first, this->speed);
				auto testPos = std::find(visited.begin(), visited.end(), currentEdge.first);

				if (current->Node->distance + edgeLength < currentEdge.first->distance
					|| testPos == visited.end()) {
					if (testPos == visited.end()) {
						visited.push_back(currentEdge.first);
					}

					currentEdge.first->movedFrom = current->Node;
					currentEdge.first->distance = current->Node->distance + edgeLength;

					updateNode* nextUpdate;
					if (correspondingUpdate.count(currentEdge.first) == 0) {
						nextUpdate = new updateNode(currentEdge.first);
						correspondingUpdate.insert({ currentEdge.first, nextUpdate });
					}
					else {
						nextUpdate = correspondingUpdate.at(currentEdge.first);
					}
					nextUpdate->parent = current;


					float distEstimate = distance + distance::distance(nextUpdate->Node, goal, distanceKey);

					distanceQueue.push(distEstimate);

					if (nodeMap.count(distEstimate) == 0) {
						nodeMap.insert({ distEstimate, std::list<updateNode*>() });
					}
					nodeMap.at(distEstimate).push_back(nextUpdate);
				}
			}
		}
	}
}

std::pair<std::list<PathNode*>, std::list<PathNode*>> DPAstarPath::getValidSubpathes(PathNode* A, PathNode* B, PathNode* Tnode)
{
	std::list<PathNode*> front;
	std::list<PathNode*> back;

	bool onFront = false;
	bool onBack = false;

	for (auto const& current : this->path) {
		if (current == Tnode) {
			onFront = true;
		}
		if (current == A) {
			onFront = false;
		}
		if (current == B) {
			onBack = true;
		}
		if (onFront) {
			front.push_back(current);
		}
		else if (onBack) {
			back.push_back(current);
		}
	}
	return { front, back };
}

std::vector<PathNode*> DPAstarPath::buildPath(PathNode* currentNode)
{
	std::list<PathNode*> path;
	while (currentNode->movedFrom != nullptr) {
		path.push_front(currentNode);
		currentNode = currentNode->movedFrom;
	}
	path.push_front(currentNode);
	std::vector<PathNode*> path_vec(path.begin(), path.end());
	this->cost = path.back()->distance;
	return path_vec;
}



updateNode::updateNode(PathNode* Node)
{
	this->Node = Node;
	this->Rvisited = false;
	this->Rworse = false;
	this->parent = this;
}

updateNode::updateNode(PathNode* Node, bool visited, bool worse, updateNode* parent)
{
	this->Node = Node;
	this->Rvisited = visited;
	this->Rworse = worse;
	this->parent = parent;
}


// LPA*
std::list<PathNode*> LPAstar::LPAstarNode::getPrevious()
{
	std::list<PathNode*>nodes(this->Node->furtherNodes.begin(), this->Node->furtherNodes.end());

	for (auto const& N : this->Node->edges) {
		if (!N.second->oneDirectional) {
			nodes.push_back(N.first);
		}
	}
	return nodes;
}

LPAstar::LPAstarNode& LPAstar::LPAstarNode::operator=(const LPAstarNode& other)
{
		this->Node = other.Node;
		this->rhs = other.rhs;
		this->g = other.g;
		this->parent = other.parent;
		this->key = other.key;
		return *this;
}

bool LPAstar::LPAstarNode::IsConsistent() const
{
	return this->g == this->rhs;
}

bool LPAstar::LPAstarNode::operator>(const LPAstarNode& other)
{
	if (other.key == this->key) {
		return this > &other;
	}
	else if (other.key > this->key) {
		return true;
	}
	return false;
}

bool LPAstar::LPAstarNode::operator==(const LPAstarNode& other)
{
	return this->key == other.key;
}


LPAstarPath::LPAstarPath(PathNode* start, PathNode* end, int posKey, int speed)
{

	start->movedFrom = NULL;
	this->Initialize(start, end, posKey);
	this->valid = this->findShortestPath(speed, posKey);
}

LPAstarPath::~LPAstarPath()
{
	for (auto const& N : this->nodes) {
		delete N.second;
	}
}

bool LPAstarPath::update(updateEvent* update, PathNode* current, int distanceKey)
{
	this->updateInserts(update, distanceKey);
	this->updateDels(update, distanceKey);
	this->valid = this->findShortestPath(this->speed, distanceKey);
	return this->valid;
}

LPAstar::Key LPAstarPath::CalculateKey(LPAstar::LPAstarNode* vertex, int distanceKey, float hweight)
{
	LPAstar::Key res(std::min(vertex->g, vertex->rhs + hweight * distance::distance(vertex->Node, this->goal->Node, distanceKey)), std::min(vertex->g, vertex->rhs));
	return res;
}

float LPAstarPath::GetCost(edge* e, PathNode* to, int speed) const
{
	return e->getLength(to, speed);
}

void LPAstarPath::Initialize(PathNode* start, PathNode* end, int metrics)
{
	auto start_node = new LPAstar::LPAstarNode(start);
	auto goal_node = new LPAstar::LPAstarNode(end);
	goal_node->g = std::numeric_limits<float>::infinity();
	goal_node->rhs = std::numeric_limits<float>::infinity();

	this->nodes.clear();

	this->nodes.insert({ end, goal_node });
	this->goal = goal_node;

	start_node->rhs = 0;
	start_node->g = std::numeric_limits<double>::infinity();
	start_node->key = CalculateKey(start_node, metrics);

	this->nodes.insert({ start, start_node });
	this->start = start_node;


	open.push(start_node); //add start cell to OPEN queue its a priority queue

	this->goal_became_unreachable = false;
}

bool LPAstarPath::findShortestPath(int speed, int distanceKey)
{
	auto k = CalculateKey(this->goal, distanceKey);
	bool v = (this->open.top()->key < CalculateKey(this->goal, distanceKey));
	while (this->open.top()->key < CalculateKey(this->goal, distanceKey) || this->goal->rhs > this->goal->g) {
		auto current = open.top();
		if (current->Node == this->goal->Node) {
			this->buildPath();
			return true;
		}
		if (current->g > current->rhs) {
			open.pop();
			current->g = current->rhs;
			for (auto const& succ : this->getSucsessors(current)) {
				if (succ.first->rhs > current->g + succ.second->getLength(succ.first->Node, speed)) {
					succ.first->parent = current;
					succ.first->rhs = current->g + succ.second->getLength(succ.first->Node, speed);
				}
				this->updateNode(succ.first, distanceKey);
			}
		}
		else {
			current->g = std::numeric_limits<float>::infinity();
			auto succs = this->getSucsessors(current);
			succs.push_back({ current, nullptr });
			for (auto const& succ : succs) {
				if (!(succ.first == this->start) && succ.first->parent->Node != current->Node) {
					auto minPred = this->GetMinPredecessor(succ.first, speed);
					succ.first->rhs = minPred.rhs;
					succ.first->parent = minPred.parent;
				}
				this->updateNode(succ.first, distanceKey);
			}

		}
	}
	if (this->goal->rhs != std::numeric_limits<float>::infinity()) {
		this->buildPath();
		return true;
	}
	return false;
}

std::list<std::pair<LPAstar::LPAstarNode*, edge*>> LPAstarPath::getSucsessors(LPAstar::LPAstarNode* node)
{
	std::list<std::pair<LPAstar::LPAstarNode*, edge*>>res;
	for (auto const& edgeIter : node->Node->edges) {
		if (edgeIter.first->walkable && edgeIter.second->walkable) {
			res.push_back({ this->getNode(edgeIter.first), edgeIter.second });
		}
	}
	return res;
}

std::list<std::pair<LPAstar::LPAstarNode*, edge*>> LPAstarPath::getAllSucsessors(LPAstar::LPAstarNode* node)
{
	std::list<std::pair<LPAstar::LPAstarNode*, edge*>>res;
	for (auto const& edgeIter : node->Node->edges) {
		res.push_back({ this->getNode(edgeIter.first), edgeIter.second });
	}
	return res;
}

LPAstar::LPAstarNode* LPAstarPath::getNode(PathNode* node)
{
	auto pos = this->nodes.find(node);
	if (pos == this->nodes.end()) {
		auto NewNode = new LPAstar::LPAstarNode(node);
		this->nodes.insert({ node, NewNode });
		return NewNode;
	}
	return (*pos).second;
}

void LPAstarPath::updateNode(LPAstar::LPAstarNode* node, int& distanceKey)
{
	if (this->open.updateAndContains(node)) {
		return;
	}
	else if (!node->IsConsistent()) {
		node->key = CalculateKey(node, distanceKey);
		this->open.push(node);
	}
	else {
		this->open.remove(node);
	}
}

LPAstar::NodeDummy LPAstarPath::GetMinPredecessor(LPAstar::LPAstarNode* current, int speed)
{
	auto all_neighbors = this->getAllSucsessors(current);

	LPAstar::NodeDummy dummy;

	if (!all_neighbors.empty()) {

		dummy.rhs = all_neighbors.front().first->rhs;
		dummy.parent = all_neighbors.front().first;

		for (auto const& n : all_neighbors) {
			if (dummy.rhs > n.first->g + n.second->getLength(n.first->Node, speed)) {
				dummy.rhs = n.first->g + n.second->getLength(n.first->Node, speed);
				dummy.parent = n.first;
			}
		}
	}
	else {
		dummy.parent = nullptr;
	}
	return dummy;
}

void LPAstarPath::buildPath()
{
	std::list<PathNode*> res;
	auto node = this->goal;
	this->cost = node->Node->distance;
	while (node != nullptr) {
		res.push_front(node->Node);
		node = node->parent;
	}
	std::vector<PathNode*> p(res.begin(), res.end());
	this->path = p;
}

void LPAstarPath::updateInserts(updateEvent* update, int key)
{
	for (auto const& val : update->inserts) {
		LPAstar::LPAstarNode* node = this->getNode(val);
		for (auto const& succ : this->getAllSucsessors(node)) {
			if (succ.first->rhs > node->g + succ.second->getLength(val, this->speed)) {
				succ.first->rhs = node->g + succ.second->getLength(val, this->speed);
				succ.first->parent = node;
				this->updateNode(succ.first, key);
			}
		}
	}
}

void LPAstarPath::updateDels(updateEvent* update, int key)
{
	for (auto const& val : update->deletions) {
		LPAstar::LPAstarNode* node = this->getNode(val);
		for (auto const& succ : this->getAllSucsessors(node)) {
			if (succ.first != this->start && succ.first->parent == node) {
				auto argMin = this->GetMinPredecessor(succ.first, this->speed);
				succ.first->parent = argMin.parent;
				succ.first->rhs = argMin.parent->g + succ.second->getLength(succ.first->Node, this->speed);
				succ.first->key = LPAstar::CalculateKey(succ.first, this->goal->Node, key);
				this->updateNode(succ.first, key);
			}
		}
	}
}


LPAstar::LPAstarNode::LPAstarNode(PathNode* Node)
{
	this->Node = Node;
}

LPAstar::Key LPAstar::CalculateKey(LPAstarNode* vertex, PathNode* goal, int distanceKey, float hweight)
{
	LPAstar::Key res(std::min(vertex->g, vertex->rhs + hweight * distance::distance(vertex->Node, goal, distanceKey)), std::min(vertex->g, vertex->rhs));
	return res;
}
