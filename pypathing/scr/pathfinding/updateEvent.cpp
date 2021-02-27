#include "updateEvent.h"
#include "node.h"
#include "path.h"

std::tuple<PathNode*, PathNode*> updateEvent::getEntryAndExitsInserts(Path* path)
{
	PathNode* inserstsInNode = nullptr;
	PathNode* inserstsOutNode = nullptr;

	for (auto const& currentNode : path->path) {
		for (auto const& checkNode : this->inserts) {
			if (currentNode == checkNode) {
				if (inserstsInNode == nullptr) {
					inserstsInNode = checkNode;
				}
				inserstsOutNode = checkNode;
			}
		}
	}
	return { inserstsInNode, inserstsOutNode};
}

std::tuple<PathNode*, PathNode*> updateEvent::getEntryAndExitsDels(Path* path)
{
	PathNode* delsInNode = nullptr;
	PathNode* delsOutNode = nullptr;

	for (auto const& currentNode : path->path) {
		for (auto const& checkNode : this->inserts) {
			if (currentNode == checkNode) {
				if (delsInNode == nullptr) {
					delsInNode = checkNode;
				}
				delsOutNode = checkNode;
			}
		}
	}
	return { delsInNode, delsOutNode };
}
