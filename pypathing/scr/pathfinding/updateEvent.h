#include <list>
#include <vector>
class PathNode;
class Path;


class updateEvent{
	// nodes inserted
public: std::list<PathNode*> inserts;
	// nodes removed well actualy turend off
public: std::list<PathNode*> deletions;

public: std::pair<PathNode*, PathNode*> getEntryAndExitsInserts(Path* path);
public: std::pair<PathNode*, PathNode*> getEntryAndExitsDels(Path* path);
public: bool isInDels(PathNode* testNode);
public: bool isInInserts(PathNode* testNode);
public: float distance(PathNode* A, int key);
};