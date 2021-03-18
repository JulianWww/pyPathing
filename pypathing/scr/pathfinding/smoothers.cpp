#include "smoothers.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>



std::vector<float> smoothing::LagrangePolynomial::poses(float x)
{
	std::vector<float> vec(this->cors.front().size());
	for (auto v : vec) {
		v = 0;
	}


	auto point = this->points.begin();
	for (auto poly = this->cors.begin(); poly != this->cors.end(); poly++, point++) {
		for (size_t idx = 0; idx < vec.size(); idx++) {
			double value = (*point)[idx];
			for (auto const& pointIter : (*poly)[idx]) {
				value = value*(x + pointIter.first) / (pointIter.second);
				//std::cout << value << " " << pointIter.first << " " << pointIter.second << std::endl;
			}
			vec[idx] += value;
		}
	}
	return vec;
}

smoothing::LagrangePolynomial::LagrangePolynomial(std::vector<std::vector<int>> path)
{
	short currentIdx = 0;
	for (auto const& node : path) {
		std::vector<std::list<std::pair<float, float>>> values(node.size());
		std::vector<float>pos(node.begin(), node.end());

		for (size_t idx = 0; idx < node.size(); idx++) {
			std::list<std::pair<float, float>> subCors;
			short otherIdx = 0;
			for (auto const& other : path) {
				if (other != node) {
					subCors.push_back(std::make_pair(-otherIdx, currentIdx - otherIdx));
				}
				otherIdx++;
			}
			values[idx] = subCors;
		}
		this->points.push_back(pos);
		this->cors.push_back(values);
		currentIdx++;
	}
}
