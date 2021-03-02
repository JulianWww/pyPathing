#include <list>
#include <vector>
#include <unordered_set>
class PathNode;
class Path;


class updateEvent{
	// nodes inserted
public: std::unordered_set<PathNode*> inserts;
	// nodes removed well actualy turend off
public: std::unordered_set<PathNode*> deletions;

public: std::pair<PathNode*, PathNode*> getEntryAndExitsInserts(Path* path);
public: std::pair<PathNode*, PathNode*> getEntryAndExitsDels(Path* path);
public: bool isInDels(PathNode* testNode);
public: bool isInInserts(PathNode* testNode);
public: float distance(PathNode* A, int key);
};