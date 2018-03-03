template<typename T> 
CircularBuffer<T>::CircularBuffer(uint16_t size)
{
    buffer_ = new T[size];   // Allocated memory.  Seems preferable to use heap vs stack due to memory size limitations of the stack on embedded hardware
    tail_ = head_ = buffer_; // Both at beginning
    count_ = 0;         // Empty
    size_ = size;
}

template<typename T> 
CircularBuffer<T>::CircularBuffer() : buffer_(0),  head_(0), tail_(0), count_(0), size_(0) {

}
template <typename T>
CircularBuffer<T>::~CircularBuffer() {
    if (buffer_ != nullptr)
        delete[] buffer_;

    buffer_ = nullptr;
}

template<typename T> 
bool CircularBuffer<T>::init(uint16_t size) {
    buffer_ = new T[size];   // Allocated memory.  Seems preferable to use heap vs stack due to memory size limitations of the stack on embedded hardware
    if(buffer_ == nullptr) // Out of memory
        return false;

    tail_ = head_ = buffer_; // Both at beginning
    count_ = 0;         // Empty
    size_ = size;

    return true;
}

template<typename T> 
bool CircularBuffer<T>::push_front(T value) {
	if (head_ == buffer_) {
		head_ = buffer_ + size_;
	}
	*--head_ = value;
	if (count_ == size_) {
		if (tail_-- == buffer_) {
			tail_ = buffer_ + size_ - 1;
		}
		return false;
	} else {
		if (count_++ == 0) {
			tail_ = head_;
		}
		return true;
	}
}

template<typename T> 
bool CircularBuffer<T>::push_back(T value) {
	if (++tail_ == buffer_ + size_) {
		tail_ = buffer_;
	}
	*tail_ = value;
	if (count_ == size_) {
		if (++head_ == buffer_ + size_) {
			head_ = buffer_;
		}
		return false;
	} else {
		if (count_++ == 0) {
			head_ = tail_;
		}
		return true;
	}
}

template<typename T> 
T CircularBuffer<T>::pop_front() {
    //if(count_ <= 0) throw exception // If empty need to error here
	if (count_ <= 0) return T(); // return default for now
	T result = *head_++;
	if (head_ >= buffer_ + size_) {
		head_ = buffer_;
	}
	count_--;
	return result;
}

template<typename T> 
T CircularBuffer<T>::pop_back() {
    //if(count_ <= 0) throw exception // If empty need to error here
	if (count_ <= 0) return T(); // return default for now
   
	T result = *tail_--;
	if (tail_ < buffer_) {
		tail_ = buffer_ + size_ - 1;
	}
	count_--;
	return result;
}

template<typename T> 
T inline CircularBuffer<T>::front() {
	return *head_;
}

template<typename T> 
T inline CircularBuffer<T>::back() {
	return *tail_;
}

template <typename T>
T CircularBuffer<T>::operator[](uint16_t index) {
    return *(buffer_ + ((head_ - buffer_ + index) % size_));
}
template <typename T>
bool inline CircularBuffer<T>::empty() {  // No items in buffer
    return count_ == 0;
}
template <typename T>
bool inline CircularBuffer<T>::full() {  // Buffer full?  If so item will be removed for additional adds
    return count_ == size_;
}
template <typename T>
uint16_t inline CircularBuffer<T>::size() {  //  Retuns current buffer item count
    return count_;
}
template <typename T>
uint16_t inline CircularBuffer<T>::available() {  // Returns current buffer storage capacity
    return size_ - count_;
}
template <typename T>
uint16_t inline CircularBuffer<T>::capacity() {  // Returns total buffer storage capacity
    return size_;
}
template <typename T>
void inline CircularBuffer<T>::clear() {  // Empty contents of buffer
    memset(buffer_, 0, sizeof(*buffer_));
    head_ = tail_ = buffer_;
    count_ = 0;
}
