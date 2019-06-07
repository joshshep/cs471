#ifndef FIXED_HEAP_H_
#define FIXED_HEAP_H_

#include <algorithm>
#include <functional>
#include <queue>

namespace fixed_heap {

using std::priority_queue;
using std::vector;
using std::function;

// https://stackoverflow.com/a/2034936
template<typename T>
void push_fixed_size(priority_queue<T, vector<T>, function<bool(T, T)>> & q, T new_elem, size_t max_size) {
	q.push(new_elem);
	if (q.size() > max_size) {
		// exceeding size
		q.pop();
	}
}

template<typename T>
class FixedHeap : public priority_queue<T, vector<T>, function<bool(T, T)>> {
public:
	FixedHeap(const function<bool(T, T)>& compare, size_t maximum_size)
		: priority_queue<T, vector<T>, function<bool(T, T)>>(compare), max_size(maximum_size) {
		
	}
	void fixed_push(T new_val) {
		if (this->size() < max_size) {
			// the value doesn't matter if the heap is not already at it's fixed size
			this->push(new_val);
			return;
		}
		if (this->comp(new_val, this->top())) {
			// ok we should add this value
			this->pop();
			this->push(new_val);
		}
		// else this value isn't pushable
	}
	const size_t max_size;
};
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
