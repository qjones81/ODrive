#ifndef __CIRCULAR_BUFFER_H
#define __CIRCULAR_BUFFER_H

#include <stdio.h>
#include <string.h>

// 16bit size for now.  So max items 65536 items max
template <class T>
class CircularBuffer {
    
   private:
    T* buffer_;
    T* head_;
    T* tail_;
    uint16_t count_;
    uint16_t size_;

   public:
    CircularBuffer(uint16_t size);
    CircularBuffer();
    ~CircularBuffer();
    bool init(uint16_t size);
    // Read Front or Back of buffer without removing element
    T front(void);
    T back(void);

    T pop_front();       // Remove item from beginning
    bool push_front(T);  // Add item to beginning

    T pop_back();       // Remove item from end
    bool push_back(T);  // Add item to end

    T operator[](uint16_t index);  // Array type access

    bool inline empty();  // No items in buffer

    bool inline full();  // Buffer full?  If so item will be removed for additional adds

    uint16_t inline size();  //  Retuns current buffer item count

    uint16_t inline available();  // Returns current buffer storage capacity

    uint16_t inline capacity();  // Returns total buffer storage capacity
    void inline clear();         // Empty contents of buffer
};

#include "CircularBuffer_impl.h"
#endif /* __CIRCULAR_BUFFER_H */
