#ifndef FIXED_HEAP_H_
#define FIXED_HEAP_H_

#include <algorithm>
#include <functional>
#include <queue>

namespace fixed_heap {

// https://stackoverflow.com/a/2034936
template<typename T>
void push_fixed_size(std::priority_queue<T, std::vector<T>, std::function<bool(T, T)>> & q, T & new_elem, size_t max_size) {
	if (new_elem <= q.top()) {
		return;
	}
	// new_elem needs to be pushed
	if (q.size() >= max_size) {
		// exceeding size
		q.pop();
	}
	q.push(new_elem);
}
/*
template<typename T>
class FixedHeap {
public:
	FixedHeap(int max_size) : max_size_(max_size){
	}
	~FixedHeap() {

	}
	T top() {
		return q_.top();
	}
	void push(T & val) {
		q_.comp()
		// TODO general comparator
		if (val <= q_.top()) {
			return;
		}
		// val belongs in the q
		if (q_.size() >= max_size) {
			// exceeding size
			q_.pop();
		}
		q_.push(val);
	}
	void pop() {
		q_.pop();
	}

private:
	std::priority_queue<T> q_;
	const int max_size_;
};
*/

} // namespace fixed_heap

#endif