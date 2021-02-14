#pragma once
#include <vector>
#include <unordered_map>
#include <tuple>

#ifndef buildPos
#define buildPos(x,y,z, arr)(x + y * arr[0][0].size() + z*arr[0][0].size()*arr[0].size()+1)
#endif

#ifndef HPA_UTILS
#define HPA_UTILS
namespace utils {
	size_t buildNewPos(long long x, long long y, long long z, std::tuple<int, int, int> pos, std::vector<std::vector<std::vector<int>>> arr);
	bool isWalkable(size_t x, size_t y, size_t z, std::vector<std::vector<std::vector<int>>> arr);
	template<typename T>
	size_t buildNewPos(std::vector<T> const& pos, std::vector<T> const& sizes) {
		size_t res = 1, muls = 1;
		auto size_iter = sizes.begin();
		auto  pos_iter = pos.begin();
		muls = 1;
		for (; size_iter != sizes.end(); (size_iter++, pos_iter++)) {
			res = res + (*pos_iter) * muls;
			muls = muls * (*size_iter);
		}
		return res;

	}
	template<typename T>
	size_t buildNodeNewPos(std::vector<T> const& pos, std::vector<T> const& sizes) {
		size_t res = 1, muls = 1;
		auto size_iter = sizes.begin();
		auto  pos_iter = pos.rbegin();
		muls = 1;
		for (; size_iter != sizes.end(); (size_iter++, pos_iter++)) {
			res = res + (*pos_iter) * muls;
			muls = muls * (*size_iter);
		}
		return res;

	}
}
#endif