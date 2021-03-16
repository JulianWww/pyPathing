#include <vector>
#include <math.h>

namespace jce {

	template<typename T>
	class jvector : public std::vector<T>{

	public: jvector() {};
	public: jvector(size_t size) : std::vector<T>(size) {};
	public: jvector(const std::vector<T>& other) : std::vector<T>(other) {}
	public: jvector(std::vector<T>& a) : std::vector<T>(a.begin(), a.end()) {};
	
	public: jvector<T> operator+(jvector<T>& other) {
		jvector<T> out = *this;
		auto outIter = out.begin();
		for (auto otherIter = other.begin(); otherIter != other.end(); otherIter++, outIter++) {
			*outIter = (*outIter) + (*otherIter);
		}
		return out;
	}

	public: jvector<T> operator-(jvector<T>& other) {
		jvector<T> out = *this;
		auto outIter = out.begin();
		for (auto otherIter = other.begin(); otherIter != other.end(); otherIter++, outIter++) {
			*outIter = (*outIter) - (*otherIter);
		}
		return out;
	}
	public: jvector<T> operator-(std::vector<T>& other) {
		jvector<T> out = *this;
		auto outIter = out.begin();
		for (auto otherIter = other.begin(); otherIter != other.end(); otherIter++, outIter++) {
			*outIter = (*outIter) - (*otherIter);
		}
		return out;
	}

	public: jvector<T> operator/(T& other) {
		jvector<T> out = *this;
		auto outIter = out.begin();
		for (auto outIter = out.begin(); outIter != out.end(); outIter++) {
			*outIter = (*outIter) / other;
		}
		return out;
	}
	public: jvector<T> operator*(T& other) {
		jvector<T> out = *this;
		auto outIter = out.begin();
		for (auto outIter = out.begin(); outIter != out.end(); outIter++) {
			*outIter = (*outIter) / other;
		}
		return out;
	}

	public: T dot(jvector<T>& other) {
		T out = 0;
		auto outIter = this->begin();
		for (auto otherIter = other.begin(); otherIter != other.end(); otherIter++, outIter++) {
			out += (*outIter) * (*otherIter);
		}
		return out;
	}
	public: T abs() {
		T out = 0;

		for (auto const& val : *this) {
			out += val * val;
		}
		return (T)sqrt(out);
	}
	};
	template<typename T, typename O>
	std::vector<T> convert(std::vector<O>& inp) {
		std::vector<T>out(inp.size());
		auto outIter = out.begin();
		for (auto inpIter = inp.begin(); outIter != out.end(); outIter++, inpIter++) {
			*outIter = (T)(*inpIter);
		}
		return out;
	}
}