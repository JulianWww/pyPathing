#include "updateEvent.h"
#include "node.h"
#include "path.h"
#include "distance.h"

std::pair<PathNode*, PathNode*> updateEvent::getEntryAndExitsInserts(Path* path)
{
	PathNode* inserstsInNode = nullptr;
	PathNode* inserstsOutNode = nullptr;

	for (auto const& currentNode : path->path) {
		if (this->isInInserts(currentNode)) {
			if (inserstsInNode == nullptr) {
				inserstsInNode = currentNode;
			}
			inserstsOutNode = currentNode;
		}
	}
	return { inserstsInNode, inserstsOutNode};
}

std::pair<PathNode*, PathNode*> updateEvent::getEntryAndExitsDels(Path* path)
{
	PathNode* delsInNode = nullptr;
	PathNode* delsOutNode = nullptr;

	for (auto const& currentNode : path->path) {
		if (this->isInDels(currentNode)) {
			if (delsInNode == nullptr) {
				delsInNode = currentNode;
			}
			delsOutNode = currentNode;
		}
	}
	return { delsInNode, delsOutNode };
}

bool updateEvent::isInDels(PathNode* testNode)
{
	for (auto const& Node : this->deletions) {
		if (testNode == Node) {
			return true;
		}
	}
	return false;
}

bool updateEvent::isInInserts(PathNode* testNode)
{
	for (auto const& Node : this->inserts) {
		if (testNode == Node) {
			return true;
		}
	}
	return false;
}

float updateEvent::distance(PathNode* A, int key)
{
	float distance = NULL;
	float testDist;
	for (auto const& Node : this->deletions) {
		testDist = distance::distance(A, Node, key);
		if (testDist < distance || distance == NULL) {
			distance = testDist;
		}
	}
	return distance;
}
