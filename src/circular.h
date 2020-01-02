#ifndef CIRCULAR_H_
#define CIRCULAR_H_

#include <cstddef>

template <class T, size_t S>
class circular_buffer
{
  public:
  explicit circular_buffer() : buf_()
  {
  }

  void put(T item)
  {
    buf_[head_] = item;

    if (full_)
    {
      tail_ = (tail_ + 1) % S;
    }

    head_ = (head_ + 1) % S;

    full_ = head_ == tail_;
  }

  T get()
  {
    if (empty())
    {
      return T();
    }

    // Read data and advance the tail (we now have a free space)
    auto val = buf_[tail_];
    full_ = false;
    tail_ = (tail_ + 1) % S;

    return val;
  }

  void reset()
  {
    head_ = tail_;
    full_ = false;
  }

  bool empty() const
  {
    // if head and tail are equal, we are empty
    return (!full_ && (head_ == tail_));
  }

  bool full() const
  {
    // If tail is ahead the head by 1, we are full
    return full_;
  }

  size_t capacity() const
  {
    return S;
  }

  size_t size() const
  {
    size_t size = S;

    if (!full_)
    {
      if (head_ >= tail_)
      {
        size = head_ - tail_;
      }
      else
      {
        size = S + head_ - tail_;
      }
    }

    return size;
  }

  private:
  T buf_[S];
  size_t head_ = 0;
  size_t tail_ = 0;
  bool full_ = 0;
};

#endif /* CIRCULAR_H_ */