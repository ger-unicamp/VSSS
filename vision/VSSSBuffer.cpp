#include "vision.h"

template class VSSSBuffer<Mat>;

template <class T>
VSSSBuffer<T>::VSSSBuffer() {
	this->empty = true;
}

template <class T>
void VSSSBuffer<T>::get(T &t) {
	unique_lock<mutex> lck(this->frame_mtx);
	while(empty) {
		(this->not_empty).wait(lck);
	}
	t = this->frame_buffer;
	empty = true;
	lck.unlock();
}

template <class T>
void VSSSBuffer<T>::update (T const& t) {
	unique_lock<mutex> lck(this->frame_mtx);

	this->frame_buffer = t;
	empty = false;
	(this->not_empty).notify_one();

	lck.unlock();
}
