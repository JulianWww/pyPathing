#include <list>
#include <vector>
class PathNode;
class Path;


class updateEvent{
	// nodes inserted
public: std::list<PathNode*> inserts;
	// nodes removed well actualy turend off
public: std::list<PathNode*> deletions;

public: std::tuple<PathNode*, PathNode*> getEntryAndExitsInserts(Path* path);
public: std::tuple<PathNode*, PathNode*> getEntryAndExitsDels(Path* path);
};