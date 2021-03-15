#include <list>
#include <vector>
#include "scr/jce/mathVector.h"

class PathNode;

namespace obstacle {
	class ray {
	public: jce::jvector<float>startPos;
	public: jce::jvector<float>heading;
	public: float length;

	public: ray();
	public: ray(std::vector<int>&start, std::vector<int>&end);
	public: void set(std::vector<int>& start, std::vector<int>& end);
	};

	class Baise {
	protected: jce::jvector<float> origin;
	public: std::vector<float> getOrigin();
	public: Baise(std::vector<float> o);
	public: virtual bool intersects(ray const& r);
	public: virtual bool isIn(PathNode*);
	};

	class sphere : public obstacle::Baise {
	public: float r;
		
	public: sphere(float r, std::vector<float> o);
	public: virtual bool intersects(ray const&);
	public: virtual bool isIn(PathNode*);
	};
}