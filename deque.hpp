#include <iostream>
#include <vector>

const size_t kContainerSize = 32;

template <typename T>
class Deque {
 public:
  template <bool isConst, bool isReverse>  // NOLINT
  class FatherIterator {
   public:
    template <bool is_const, bool is_reverse>
    friend class FatherIterator;
    using value_type = std::conditional_t<isConst, const T, T>;  // NOLINT
    using pointer = value_type*;                                 // NOLINT
    using reference = value_type&;                               // NOLINT
    using iterator_category = std::random_access_iterator_tag;   // NOLINT
    using difference_type = int;                                 // NOLINT

    FatherIterator(T** vertical_ptr, size_t horizontal_index)
        : vertical_ptr_(vertical_ptr), horizontal_index_(horizontal_index) {}
    FatherIterator& operator=(
        const FatherIterator<false, isReverse>& other_iter) {
      vertical_ptr_ = other_iter.vertical_ptr_;
      horizontal_index_ = other_iter.horizontal_index_;
      return *this;
    }
    FatherIterator(const FatherIterator<false, isReverse>& other_iter) {
      vertical_ptr_ = other_iter.vertical_ptr_;
      horizontal_index_ = other_iter.horizontal_index_;
    }
    reference operator*() const {
      return *(*vertical_ptr_ + horizontal_index_);
    }
    pointer operator->() const { return *vertical_ptr_ + horizontal_index_; }
    bool operator<(const FatherIterator<isConst, isReverse>& other_iter) const {
      if (vertical_ptr_ == other_iter.vertical_ptr_) {
        if (isReverse) {
          return horizontal_index_ > other_iter.horizontal_index_;
        }
        return horizontal_index_ < other_iter.horizontal_index_;
      }
      if (isReverse) {
        return vertical_ptr_ > other_iter.vertical_ptr_;
      }
      return vertical_ptr_ < other_iter.vertical_ptr_;
    }
    bool operator>(const FatherIterator<isConst, isReverse>& other_iter) const {
      if (vertical_ptr_ == other_iter.vertical_ptr_) {
        if (isReverse) {
          return horizontal_index_ < other_iter.horizontal_index_;
        }
        return horizontal_index_ > other_iter.horizontal_index_;
      }
      if (isReverse) {
        return vertical_ptr_ < other_iter.vertical_ptr_;
      }
      return vertical_ptr_ > other_iter.vertical_ptr_;
    }
    bool operator==(
        const FatherIterator<isConst, isReverse>& other_iter) const {
      return ((vertical_ptr_ == other_iter.vertical_ptr_) &&
              (horizontal_index_ == other_iter.horizontal_index_)) ||
             (!*vertical_ptr_ && !*other_iter.vertical_ptr_);
    }
    bool operator<=(
        const FatherIterator<isConst, isReverse>& other_iter) const {
      return (*this < other_iter) || (*this == other_iter);
    }
    bool operator>=(
        const FatherIterator<isConst, isReverse>& other_iter) const {
      return (*this > other_iter) || (*this == other_iter);
    }
    bool operator!=(
        const FatherIterator<isConst, isReverse>& other_iter) const {
      return !(*this == other_iter);
    }
    FatherIterator<isConst, isReverse>& operator++() {
      (isReverse) ? back_transition() : forward_transition();
      return *this;
    }
    FatherIterator<isConst, isReverse>& operator--() {
      (isReverse) ? forward_transition() : back_transition();
      return *this;
    }

    FatherIterator<isConst, isReverse>& operator+=(int n) {
      for (int i = 0; i < std::abs(n); ++i) {
        n < 0 ? --*this : ++*this;
      }
      return *this;
    }
    FatherIterator<isConst, isReverse>& operator-=(int n) {
      for (int i = 0; i < std::abs(n); ++i) {
        n < 0 ? ++*this : --*this;
      }
      return *this;
    }
    FatherIterator<isConst, isReverse> operator++(int) {
      FatherIterator copy = *this;
      (isReverse) ? --(*this) : ++(*this);
      return copy;
    }
    FatherIterator<isConst, isReverse> operator--(int) {
      FatherIterator copy = *this;
      (isReverse) ? ++(*this) : --(*this);
      return copy;
    }
    FatherIterator<isConst, isReverse> operator+(int number) const {
      FatherIterator copy = *this;
      copy += number;
      return copy;
    }
    FatherIterator<isConst, isReverse> operator-(int number) const {
      FatherIterator copy = *this;
      copy -= number;
      return copy;
    }
    int operator-(const FatherIterator<isConst, isReverse>& other_iter) {
      if (!*vertical_ptr_ && !*other_iter.vertical_ptr_) {
        return 0;
      }
      auto copy = *this;
      int res = 0;
      int adjust = (copy < other_iter) ? 1 : -1;
      while (copy != other_iter) {
        copy += adjust;
        res -= adjust;
      }
      return res;

      if (vertical_ptr_ == other_iter.vertical_ptr_) {
        return horizontal_index_ - other_iter.horizontal_index_;
      }
      if (vertical_ptr_ < other_iter.vertical_ptr_) {
        int counter =
            kContainerSize + other_iter.horizontal_index_ - horizontal_index_;
        counter +=
            (other_iter.vertical_ptr_ - vertical_ptr_ - 1) * kContainerSize;
        return (isReverse) ? counter : -counter;
      }
      int counter =
          kContainerSize - other_iter.horizontal_index_ + horizontal_index_;
      counter +=
          (vertical_ptr_ - other_iter.vertical_ptr_ - 1) * kContainerSize;
      return (isReverse) ? -counter : counter;
    }

   private:
    T** vertical_ptr_;
    size_t horizontal_index_;
    void forward_transition() {
      if (horizontal_index_ == kContainerSize - 1) {
        horizontal_index_ = 0;
        ++vertical_ptr_;
      } else {
        ++horizontal_index_;
      }
    }
    void back_transition() {
      if (horizontal_index_ == 0) {
        horizontal_index_ = kContainerSize - 1;
        --vertical_ptr_;
      } else {
        --horizontal_index_;
      }
    }
  };
  using iterator = FatherIterator<false, false>;              // NOLINT
  using const_iterator = FatherIterator<true, false>;         // NOLINT
  using reverse_iterator = FatherIterator<false, true>;       // NOLINT
  using const_reverse_iterator = FatherIterator<true, true>;  // NOLINT

  Deque() { initialize(); }
  Deque(const Deque<T>& other_deque) {
    initialize();
    for (Deque<T>::const_iterator it = other_deque.begin();
         it != other_deque.end(); ++it) {
      try {
        push_back(*it);
      } catch (...) {
        throw;
      }
    }
  }
  Deque(size_t size) {
    initialize();
    T empty_element{};
    for (size_t i = 0; i < size; ++i) {
      try {
        push_back(empty_element);
      } catch (...) {
        throw;
      }
    }
  }
  Deque(size_t size, T element) {
    initialize();
    for (size_t i = 0; i < size; ++i) {
      try {
        push_back(element);
      } catch (...) {
        throw;
      }
    }
  }
  void destroy() {
    if (size_ == 0) {
      return;
    }
    for (size_t i = first_elem_index_; i < kContainerSize; ++i) {
      (deque_[begin_index_] + i)->~T();
    }
    delete[] reinterpret_cast<uint8_t*>(deque_[begin_index_]);
    for (size_t i = begin_index_ + 1; i < end_index_; ++i) {
      for (size_t j = 0; j < kContainerSize; ++j) {
        (deque_[i] + j)->~T();
      }
      delete[] reinterpret_cast<uint8_t*>(deque_[i]);
    }
    if (begin_index_ != end_index_) {
      for (size_t i = 0; i <= last_elem_index_; ++i) {
        (deque_[end_index_] + i)->~T();
      }
      delete[] reinterpret_cast<uint8_t*>(deque_[end_index_]);
    }
    size_ = 0;
  }

  ~Deque() { destroy(); }
  Deque& operator=(const Deque<T>& other_deque) {
    if (size_ == other_deque.size()) {
      bool flag = false;
      for (size_t i = 0; i < other_deque.size(); ++i) {
        if ((*this)[i] != other_deque[i]) {
          flag = true;
          break;
        }
      }
      if (!flag) {
        return *this;
      }
    }
    destroy();
    initialize();
    Deque<T>::const_iterator it = other_deque.begin();
    for (it = other_deque.begin(); it != other_deque.end(); ++it) {
      try {
        push_back(*it);
      } catch (...) {
        throw;
      }
    }
    return *this;
  }
  [[nodiscard]] size_t size() const { return size_; }
  void push_back(const T& new_element);
  void pop_back();
  void push_front(const T& new_element);
  void pop_front();
  T& operator[](size_t index);
  T& at(size_t index);
  const T& operator[](size_t index) const;
  const T& at(size_t index) const;
  iterator begin() {
    return iterator(&deque_[begin_index_], first_elem_index_);
  }
  iterator end() { return iterator(&deque_[end_index_], last_elem_index_ + 1); }
  const_iterator begin() const {
    return const_iterator(const_cast<T**>(&deque_[begin_index_]),
                          first_elem_index_);
  }
  const_iterator end() const {
    return const_iterator(const_cast<T**>(&deque_[end_index_]),
                          last_elem_index_ + 1);
  }
  const_iterator cbegin() const {
    return const_iterator(const_cast<T**>(&deque_[begin_index_]),
                          first_elem_index_);
  }
  const_iterator cend() const {
    return const_iterator(const_cast<T**>(&deque_[end_index_]),
                          last_elem_index_ + 1);
  }
  reverse_iterator rbegin() {
    return reverse_iterator(&deque_[end_index_], last_elem_index_);
  }
  reverse_iterator rend() {
    return reverse_iterator(&deque_[begin_index_], first_elem_index_ - 1);
  }
  const_reverse_iterator rbegin() const {
    return const_reverse_iterator(const_cast<T**>(&deque_[end_index_]),
                                  last_elem_index_);
  }
  const_reverse_iterator rend() const {
    return const_reverse_iterator(const_cast<T**>(&deque_[begin_index_]),
                                  first_elem_index_ - 1);
  }
  const_reverse_iterator crbegin() const {
    return const_reverse_iterator(const_cast<T**>(&deque_[end_index_]),
                                  last_elem_index_);
  }
  const_reverse_iterator crend() const {
    return const_reverse_iterator(const_cast<T**>(&deque_[begin_index_]),
                                  first_elem_index_ - 1);
  }
  void insert(iterator it, const T& element);
  void erase(iterator it);

 private:
  size_t size_;
  std::vector<T*> deque_;
  size_t begin_index_;
  size_t end_index_;
  size_t first_elem_index_;
  size_t last_elem_index_;
  void initialize() {
    begin_index_ = kContainerSize / 2;
    end_index_ = kContainerSize / 2;
    size_ = 0;
    for (size_t i = 0; i < kContainerSize; ++i) {
      deque_.push_back(nullptr);
    }
  }
  size_t resize(size_t curr_index);
  const T& get_index(size_t index) const;
  T& get_index(size_t index);
  void left_shifting(size_t curr_index);
  void right_shifting(size_t curr_index);
};
template <typename T>
void Deque<T>::erase(Deque::iterator it) {
  T element = *it;
  iterator copy = it;
  iterator next = it + 1;
  try {
    while (next < end()) {
      *it = *next;
      ++next;
      ++it;
    }
    pop_back();
  } catch (...) {
    while (copy < it) {
      --it;
    }
    *next = *it;
    *it = element;
    throw;
  }
}
template <typename T>
void Deque<T>::insert(Deque::iterator it, const T& element) {
  auto itt = end();
  try {
    push_back(element);
  } catch (...) {
    throw;
  }
  if (it == itt) {
    return;
  }
  iterator prev(&deque_[end_index_], last_elem_index_);
  iterator prev_copy = prev;
  iterator curr = prev - 1;
  try {
    while (it < curr) {
      *prev = *curr;
      --curr;
      --prev;
    }
    *prev = *curr;
    *curr = element;
  } catch (...) {
    while (prev < prev_copy) {
      ++prev;
    }
    pop_back();
    throw;
  }
}
template <typename T>
void Deque<T>::pop_back() {
  (deque_[end_index_] + last_elem_index_)->~T();
  if (last_elem_index_ == 0) {
    try {
      delete[] reinterpret_cast<uint8_t*>(deque_[end_index_]);
    } catch (...) {
      throw;
    }
    --end_index_;
    last_elem_index_ = kContainerSize - 1;
  } else {
    --last_elem_index_;
  }
  --size_;
}
template <typename T>
void Deque<T>::pop_front() {
  (deque_[begin_index_] + first_elem_index_)->~T();
  if (first_elem_index_ == kContainerSize - 1) {
    try {
      delete[] reinterpret_cast<uint8_t*>(deque_[begin_index_]);
    } catch (...) {
      throw;
    }
    ++begin_index_;
    first_elem_index_ = 0;
  } else {
    ++first_elem_index_;
  }
  --size_;
}
template <typename T>
const T& Deque<T>::get_index(size_t index) const {
  if (index < kContainerSize - first_elem_index_) {
    return *(deque_[begin_index_] + first_elem_index_ + index);
  }
  size_t elems_count = index + 1 - kContainerSize + first_elem_index_;
  size_t shift_count = elems_count / kContainerSize +
                       static_cast<size_t>(elems_count % kContainerSize != 0);
  if (elems_count % kContainerSize == 0) {
    elems_count = kContainerSize;
  } else {
    elems_count %= kContainerSize;
  }
  return *(deque_[begin_index_ + shift_count] + elems_count - 1);
}
template <typename T>
T& Deque<T>::get_index(size_t index) {
  if (index < kContainerSize - first_elem_index_) {
    return *(deque_[begin_index_] + first_elem_index_ + index);
  }
  size_t elems_count = index + 1 - kContainerSize + first_elem_index_;
  size_t shift_count = elems_count / kContainerSize +
                       static_cast<size_t>(elems_count % kContainerSize != 0);
  if (elems_count % kContainerSize == 0) {
    elems_count = kContainerSize;
  } else {
    elems_count %= kContainerSize;
  }
  return *(deque_[begin_index_ + shift_count] + elems_count - 1);
}
template <typename T>
const T& Deque<T>::at(size_t index) const {
  if (index >= size_) {
    throw std::out_of_range("Index out of range!");
  }
  return get_index(index);
}
template <typename T>
T& Deque<T>::at(size_t index) {
  if (index >= size_) {
    throw std::out_of_range("Index out of range!");
  }
  return get_index(index);
}
template <typename T>
const T& Deque<T>::operator[](size_t index) const {
  return get_index(index);
}
template <typename T>
T& Deque<T>::operator[](size_t index) {
  return get_index(index);
}
template <typename T>
void Deque<T>::push_back(const T& new_element) {  // NOLINT
  if (size_ == 0) {
    try {
      deque_[end_index_] =
          reinterpret_cast<T*>(new uint8_t[kContainerSize * sizeof(T)]);
    } catch (...) {
      throw;
    }
    last_elem_index_ = 0;
    first_elem_index_ = 0;
    try {
      new (deque_[end_index_] + last_elem_index_) T(new_element);
    } catch (...) {
      throw;
    }
    ++size_;
    return;
  }
  if (last_elem_index_ < kContainerSize - 1) {
    ++last_elem_index_;
    try {
      new (deque_[end_index_] + last_elem_index_) T(new_element);
    } catch (...) {
      throw;
    }
    ++size_;
    return;
  }
  if (last_elem_index_ == kContainerSize - 1) {
    try {
      right_shifting(end_index_);
    } catch (...) {
      throw;
    }
    ++end_index_;
    last_elem_index_ = 0;
    try {
      new (deque_[end_index_] + last_elem_index_) T(new_element);
    } catch (...) {
      delete[] reinterpret_cast<uint8_t*>(deque_[end_index_]);
      --end_index_;
      last_elem_index_ = kContainerSize - 1;
      throw;
    }
    ++size_;
  }
}
template <typename T>
void Deque<T>::push_front(const T& new_element) {  // NOLINT
  if (size_ == 0) {
    try {
      deque_[begin_index_] =
          reinterpret_cast<T*>(new uint8_t[kContainerSize * sizeof(T)]);
    } catch (...) {
      throw;
    }
    last_elem_index_ = kContainerSize - 1;
    first_elem_index_ = kContainerSize - 1;
    try {
      new (deque_[begin_index_] + first_elem_index_) T(new_element);
    } catch (...) {
      throw;
    }
    ++size_;
    return;
  }
  if (first_elem_index_ > 0) {
    --first_elem_index_;
    try {
      new (deque_[begin_index_] + first_elem_index_) T(new_element);
    } catch (...) {
      throw;
    }
    ++size_;
    return;
  }
  if (first_elem_index_ == 0) {
    try {
      left_shifting(begin_index_);
    } catch (...) {
      throw;
    }
    --begin_index_;
    first_elem_index_ = kContainerSize - 1;
    try {
      new (deque_[begin_index_] + first_elem_index_) T(new_element);
    } catch (...) {
      delete[] reinterpret_cast<uint8_t*>(deque_[begin_index_]);
      ++begin_index_;
      first_elem_index_ = 0;
      throw;
    }
    ++size_;
    return;
  }
}
template <typename T>
void Deque<T>::left_shifting(size_t curr_index) {
  if (curr_index == 0) {
    curr_index = resize(curr_index);
  }
  --curr_index;
  try {
    deque_[curr_index] =
        reinterpret_cast<T*>(new uint8_t[kContainerSize * sizeof(T)]);
  } catch (...) {
    throw;
  }
}
template <typename T>
void Deque<T>::right_shifting(size_t curr_index) {
  if (curr_index == deque_.size() - 1) {
    curr_index = resize(curr_index);
  }
  ++curr_index;
  try {
    deque_[curr_index] =
        reinterpret_cast<T*>(new uint8_t[kContainerSize * sizeof(T)]);
  } catch (...) {
    throw;
  }
}
template <typename T>
size_t Deque<T>::resize(size_t curr_index) {
  std::vector<T*> new_deque(deque_.size() * 2);
  size_t copy_begin_index = begin_index_;
  end_index_ = new_deque.size() / 2 + (end_index_ - begin_index_);
  begin_index_ = new_deque.size() / 2;
  for (size_t i = 0; i <= end_index_ - begin_index_; ++i) {
    new_deque[i + begin_index_] = deque_[i + copy_begin_index];
  }
  curr_index = begin_index_ + (curr_index - copy_begin_index);
  deque_ = new_deque;
  return curr_index;
}

