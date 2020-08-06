#include <type_traits>

/*
 * Simple Structure-of-Arrays with a predefined layout, compile-time size and alignment,
 * a struct-like reference to access a "row", and direct access to the "columns".
 */

// CUDA attributes
#ifdef __CUDACC__
#define SOA_HOST_ONLY __host__
#define SOA_HOST_DEVICE __host__ __device__
#else
#define SOA_HOST_ONLY
#define SOA_HOST_DEVICE
#endif

// return the smallest integer greater than or equal to `x` that is a multiple of `n`
template <typename T>
constexpr
T next_multiple(T x, T n) {
  T c = (x + n - 1) / n;
  return c * n;
}

// struct type with value semantics, corresponding to an individual entry in a SoA
struct SoAEntry {
public:
  double x;
  double y;
  double z;
  uint16_t colour;
  int32_t value;
  const char* name;
};


// compile-time sized SoA
template <size_t SIZE, size_t ALIGN=0>
struct SoATemplate {

  // type used for the indices into the SoA
  using size_type = size_t;

  // type used for the difference between indices into the SoA
  using difference_type = std::make_signed_t<size_type>;

  // struct type with value semantics, corresponding to an individual entry in a SoA
  using value_type = SoAEntry;

  // forward declaration for types with reference semantics, used as accessors to individual entries in a SoA
  struct reference;
  struct const_reference;

  // forward declaration for types with iterator semantics, used as accessors to individual entries in a SoA
  struct iterator;
  struct const_iterator;

  // type with reference semantics, used as an accessor to individual entries in a SoA
  struct reference {
  public:
    constexpr
    reference(SoATemplate<SIZE,ALIGN> & soa, size_type index) :
      x(soa.x[index]),
      y(soa.y[index]),
      z(soa.z[index]),
      colour(soa.colour[index]),
      value(soa.value[index]),
      name(soa.name[index])
    { }

    constexpr
    reference& operator=(reference const& e) {
      x = e.x;
      y = e.y;
      z = e.z;
      colour = e.colour;
      value = e.value;
      name = e.name;
      return *this;
    }

    constexpr
    reference& operator=(const_reference const& e) {
      x = e.x;
      y = e.y;
      z = e.z;
      colour = e.colour;
      value = e.value;
      name = e.name;
      return *this;
    }

    constexpr
    reference& operator=(value_type const& e) {
      x = e.x;
      y = e.y;
      z = e.z;
      colour = e.colour;
      value = e.value;
      name = e.name;
      return *this;
    }

    constexpr
    operator value_type() const {
      return value_type{x, y, z, colour, value, name};
    }

  public:
    double & x;
    double & y;
    double & z;
    uint16_t & colour;
    int32_t & value;
    const char* & name;
  };

  // type with const reference semantics, used as a const accessor to individual entries in a const SoA
  struct const_reference {
  public:
    constexpr
    const_reference(SoATemplate<SIZE,ALIGN> const& soa, size_type index) :
      x(soa.x[index]),
      y(soa.y[index]),
      z(soa.z[index]),
      colour(soa.colour[index]),
      value(soa.value[index]),
      name(soa.name[index])
    { }

    const_reference& operator=(const_reference const& e) = delete;
    const_reference& operator=(const_reference && e) = delete;

    constexpr
    operator value_type() const {
      return value_type{x, y, z, colour, value, name};
    }

  public:
    double const& x;
    double const& y;
    double const& z;
    uint16_t const& colour;
    int32_t const& value;
    const char* const& name;
  };

  // type with iterator semantics, used to identify an individual entry in a SoA
  struct iterator {
    iterator() = default;
    iterator(iterator const&) = default;
    iterator(iterator &&) = default;
    iterator& operator=(iterator const&) = default;
    iterator& operator=(iterator &&) = default;

    iterator(SoATemplate* soa, size_type index) :
      soa_(soa),
      index_(index)
    {}

    constexpr
    iterator& operator++() {
      ++index_;
      return *this;
    }

    constexpr
    iterator operator++(int) {
      iterator it = *this;
      ++index_;
      return it;
    }

    constexpr
    iterator& operator+=(difference_type inc) {
      index_ += inc;
      return *this;
    }

    constexpr
    iterator& operator--() {
      --index_;
      return *this;
    }

    constexpr
    iterator operator--(int) {
      iterator it = *this;
      --index_;
      return it;
    }

    constexpr
    iterator& operator-=(difference_type dec) {
      index_ -= dec;
      return *this;
    }

    constexpr
    iterator operator+(difference_type inc) const {
      iterator it = *this;
      it += inc;
      return it;
    }

    constexpr
    iterator operator-(difference_type dec) const {
      iterator it = *this;
      it -= dec;
      return it;
    }

    constexpr
    difference_type operator-(iterator other) const {
      assert(soa_ == other.soa_);
      return static_cast<difference_type>(index_ - other.index_);
    }

    constexpr
    bool operator==(iterator const& other) const {
      return (soa_ == other.soa_) and (index_ == other.index_);
    }

    constexpr
    bool operator!=(iterator const& other) const {
      return not (*this == other);
    }

    constexpr
    bool operator<(iterator const& other) const {
      return (soa_ == other.soa_) ? (index_ < other.index_) : (soa_ < other.soa_);
    }

    constexpr
    bool operator>(iterator const& other) const {
      return (other < *this);
    }

    constexpr
    bool operator<=(iterator const& other) const {
      return not (*this > other);
    }

    constexpr
    bool operator>=(iterator const& other) const {
      return not (*this < other);
    }

    constexpr
    reference operator*() const {
      return reference(*soa_, index_);
    }

  private:
    SoATemplate * soa_;
    size_type index_;
  };

  // type with const iterator semantics, used to identify an individual entry in a const SoA
  struct const_iterator {
    const_iterator() = default;
    const_iterator(const_iterator const&) = default;
    const_iterator(const_iterator &&) = default;
    const_iterator& operator=(const_iterator const&) = default;
    const_iterator& operator=(const_iterator &&) = default;

    const_iterator(SoATemplate const* soa, size_type index) :
      soa_(soa),
      index_(index)
    {}

    const_iterator(iterator const& it) :
      soa_(it.soa_),
      index_(it.index_)
    {}

    constexpr
    const_iterator& operator++() {
      ++index_;
      return *this;
    }

    constexpr
    const_iterator operator++(int) {
      const_iterator it = *this;
      ++index_;
      return it;
    }

    constexpr
    const_iterator& operator+=(difference_type inc) {
      index_ += inc;
      return *this;
    }

    constexpr
    const_iterator& operator--() {
      --index_;
      return *this;
    }

    constexpr
    const_iterator operator--(int) {
      const_iterator it = *this;
      --index_;
      return it;
    }

    constexpr
    const_iterator& operator-=(difference_type dec) {
      index_ -= dec;
      return *this;
    }

    constexpr
    const_iterator operator+(difference_type inc) const {
      const_iterator it = *this;
      it += inc;
      return it;
    }

    constexpr
    const_iterator operator-(difference_type dec) const {
      const_iterator it = *this;
      it -= dec;
      return it;
    }

    constexpr
    difference_type operator-(const_iterator other) const {
      assert(soa_ == other.soa_);
      return static_cast<difference_type>(index_ - other.index_);
    }

    constexpr
    bool operator==(const_iterator const& other) const {
      return (soa_ == other.soa_) and (index_ == other.index_);
    }

    constexpr
    bool operator!=(const_iterator const& other) const {
      return not (*this == other);
    }

    constexpr
    bool operator<(const_iterator const& other) const {
      return (soa_ == other.soa_) ? (index_ < other.index_) : (soa_ < other.soa_);
    }

    constexpr
    bool operator>(const_iterator const& other) const {
      return (other < *this);
    }

    constexpr
    bool operator<=(const_iterator const& other) const {
      return not (*this > other);
    }

    constexpr
    bool operator>=(const_iterator const& other) const {
      return not (*this < other);
    }

    constexpr
    const_reference operator*() const {
      return const_reference(*soa_, index_);
    }

  private:
    const SoATemplate * soa_;
    size_type index_;
  };

  // size (numer of entries) in the SoA
  static constexpr
  size_type size() { return SIZE; }

  // alignment of each "column" of the SoA
  static constexpr
  size_type alignment() { return ALIGN; }

  // extent in bytes of the SoA
  static constexpr
  size_t extent() {
    /*
    return next_multiple(sizeof(double) * size(), std::max(alignof(double), alignment())) +
           next_multiple(sizeof(double) * size(), std::max(alignof(double), alignment())) +
           next_multiple(sizeof(double) * size(), std::max(alignof(double), alignment())) +
           next_multiple(sizeof(uint16_t) * size(), std::max(alignof(uint16_t), alignment())) +
           next_multiple(sizeof(int32_t) * size(), std::max(alignof(int32_t), alignment())) +
           next_multiple(sizeof(const char*) * size(), std::max(alignof(const char*), alignment()));
    */
    return sizeof(SoATemplate);
  }

  iterator begin() {
    return iterator(this, 0);
  }

  const_iterator begin() const {
    return const_iterator(this, 0);
  }

  iterator end() {
    return iterator(this, size());
  }

  const_iterator end() const {
    return const_iterator(this, size());
  }

  // AoS-like access
  SOA_HOST_DEVICE
  reference operator[](size_type index) { return reference(*this, index); }

  SOA_HOST_DEVICE
  const_reference operator[](size_type index) const { return const_reference(*this, index); }

  // data members
  alignas(ALIGN) double x[SIZE];
  alignas(ALIGN) double y[SIZE];
  alignas(ALIGN) double z[SIZE];
  alignas(ALIGN) uint16_t colour[SIZE];
  alignas(ALIGN) int32_t value[SIZE];
  alignas(ALIGN) const char* name[SIZE];
};

// -----------------------------------------------------------------------------
#include <iostream>

// dump the SoA internal structure
template <typename T>
SOA_HOST_ONLY
void dump() {
  using SoA = T;
  std::cout << "SoATemplate<" << SoA::size() << ", " << SoA::alignment() << "): " << '\n';
  std::cout << "  sizeof(...): " << sizeof(SoA) << '\n';
  std::cout << "  alignof(...): " << alignof(SoA) << '\n';
  std::cout << "  x[" << SoA::size() << "] at " << offsetof(SoA, x) << " has size " << sizeof(SoA::x) << '\n';
  std::cout << "  y[" << SoA::size() << "] at " << offsetof(SoA, y) << " has size " << sizeof(SoA::y) << '\n';
  std::cout << "  z[" << SoA::size() << "] at " << offsetof(SoA, z) << " has size " << sizeof(SoA::z) << '\n';
  std::cout << "  colour[" << SoA::size() << "] at " << offsetof(SoA, colour) << " has size " << sizeof(SoA::colour) << '\n';
  std::cout << "  value[" << SoA::size() << "] at " << offsetof(SoA, value) << " has size " << sizeof(SoA::value) << '\n';
  std::cout << "  name[" << SoA::size() << "] at " << offsetof(SoA, name) << " has size " << sizeof(SoA::name) << '\n';
  std::cout << std::endl;
}
