/*
 * Simple Structure-of-Arrays with a predefined layout, compile-time size and alignment,
 * and accessors to the "rows" and "columns".
 */

// CUDA attributes
#ifdef __CUDACC__
#define SOA_HOST_ONLY __host__
#define SOA_HOST_DEVICE __host__ __device__
#else
#define SOA_HOST_ONLY
#define SOA_HOST_DEVICE
#endif

// compile-time sized SoA
template <size_t SIZE, size_t ALIGN=0>
struct SoA {

  // these could be moved to an external type trait to free up the symbol names
  static const size_t size = SIZE;
  static const size_t alignment = ALIGN;

  // AoS-like (row-wise) accessor to individual elements
  struct element {
    SOA_HOST_DEVICE
    element(SoA & soa, size_t index) :
      soa_(soa),
      index_(index)
    { }

    SOA_HOST_DEVICE
    element& operator=(element const& other) {
      x() = other.x();
      y() = other.y();
      z() = other.z();
      colour() = other.colour();
      value() = other.value();
      name() = other.name();
      return *this;
    }

    SOA_HOST_DEVICE
    element& operator=(element && other) {
      x() = other.x();
      y() = other.y();
      z() = other.z();
      colour() = other.colour();
      value() = other.value();
      name() = other.name();
      return *this;
    }

    SOA_HOST_DEVICE double & x() { return soa_.x_[index_]; }
    SOA_HOST_DEVICE double & y() { return soa_.y_[index_]; }
    SOA_HOST_DEVICE double & z() { return soa_.z_[index_]; }
    SOA_HOST_DEVICE uint16_t & colour() { return soa_.colour_[index_]; }
    SOA_HOST_DEVICE int32_t & value() { return soa_.value_[index_]; }
    SOA_HOST_DEVICE const char* & name() { return soa_.name_[index_]; }

    SoA & soa_;
    const size_t index_;
  };

  // AoS-like (row-wise) access to individual elements
  SOA_HOST_DEVICE
  element operator[](size_t index) { return element(*this, index); }

  // accessors
  SOA_HOST_DEVICE double* x() { return x_; }
  SOA_HOST_DEVICE double* y() { return y_; }
  SOA_HOST_DEVICE double* z() { return z_; }
  SOA_HOST_DEVICE uint16_t* colour() { return colour_; }
  SOA_HOST_DEVICE int32_t* value() { return value_; }
  SOA_HOST_DEVICE const char** name() { return name_; }

  // dump the SoA internal structure
  template <typename T> SOA_HOST_ONLY friend void dump();

private:
  // data members
  alignas(ALIGN) double x_[SIZE];
  alignas(ALIGN) double y_[SIZE];
  alignas(ALIGN) double z_[SIZE];
  alignas(ALIGN) uint16_t colour_[SIZE];
  alignas(ALIGN) int32_t value_[SIZE];
  alignas(ALIGN) const char* name_[SIZE];
};

// -----------------------------------------------------------------------------
#include <iostream>

// dump the SoA internal structure
template <typename T>
SOA_HOST_ONLY
void dump() {
  using SoA = T;
  std::cout << "SoA<" << SoA::size << ", " << SoA::alignment << "): " << '\n';
  std::cout << "  sizeof(...): " << sizeof(SoA) << '\n';
  std::cout << "  alignof(...): " << alignof(SoA) << '\n';
  std::cout << "  x_[" << SoA::size << "] at " << offsetof(SoA, x_) << " has size " << sizeof(SoA::x_) << '\n';
  std::cout << "  y_[" << SoA::size << "] at " << offsetof(SoA, y_) << " has size " << sizeof(SoA::y_) << '\n';
  std::cout << "  z_[" << SoA::size << "] at " << offsetof(SoA, z_) << " has size " << sizeof(SoA::z_) << '\n';
  std::cout << "  colour_[" << SoA::size << "] at " << offsetof(SoA, colour_) << " has size " << sizeof(SoA::colour_) << '\n';
  std::cout << "  value_[" << SoA::size << "] at " << offsetof(SoA, value_) << " has size " << sizeof(SoA::value_) << '\n';
  std::cout << "  name_[" << SoA::size << "] at " << offsetof(SoA, name_) << " has size " << sizeof(SoA::name_) << '\n';
  std::cout << std::endl;
}
