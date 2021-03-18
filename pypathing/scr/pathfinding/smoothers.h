#include <list>
#include <vector>

namespace smoothing{
	class Point {
		std::vector<float> pos;
	};

	class LagrangePolynomial {
	public: std::list<std::vector<std::list<std::pair<float, float>>>> cors;
	public: std::list<std::vector<float>> points;
	public: std::vector<float> poses(float val);
	public: LagrangePolynomial(std::vector<std::vector<int>>path);
	};
};