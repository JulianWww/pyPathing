#include <unordered_map>
#include <unordered_set>
#include <queue>

#ifndef OPENLIST_H
#define OPENLIST_H

namespace jce {
	template<typename T>
	struct pointerGreater {
		bool operator()(T* lhs, T* rhs) const
		{
			bool out = !((*lhs) > (*rhs));
			if ((*lhs) == (*rhs)) {
				return false;
			}
			return out;
		}
	};

	template<typename T>
	struct pointerSmaler {
		bool operator()(T* lhs, T* rhs) const
		{
			bool out = ((*lhs) > (*rhs));
			return out;
		}
	};


	template<typename T>
	class NodeQueue : public std::priority_queue<T*, std::vector<T*>, pointerSmaler<T>>
	{
	public:

		bool remove(const T* value) {
			auto it = std::find(this->c.begin(), this->c.end(), value);

			if (it != this->c.end()) {
				this->c.erase(it);
				std::make_heap(this->c.begin(), this->c.end(), this->comp);
				return true;
			}
			else {
				return false;
			}
		}
		bool updateAndContains(T* value) {
			if (this->remove(value)) {
				this->push(value);
				return true;
			}
			return false;
		}
		T* topAndPop() {
			T* v = this->top();
			this->pop();
			return v;
		}
		bool contains(T* value) {
			auto it = std::find(this->c.begin(), this->c.end(), value);
			return it != this->c.end();
		}
	};

	template<typename key, typename T>
	class nodeMap : public std::unordered_map<key, T*> {
	public: void insertOnce(key val, T* item) {
		if (this->find(val) == this->end()) {
			this->insert({ val, item });
		}
	}

	public: T* get(key val) {
		if (this->find(val) == this->end()) {
			auto newT = new T(val);
			this->insert({ val, newT });
			return newT;
		}
		return this->at(val);
	}
	};
}

#endif // OPENLIST_H
