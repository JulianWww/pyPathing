#include <list>
class PathNode;


class updateEvent{
	// nodes inserted
public: std::list<PathNode*> inserts;
	// nodes removed well actualy turend off
public: std::list<PathNode*> deletions;
};