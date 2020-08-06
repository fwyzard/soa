/*
 * Structure-of-Arrays template defined at compile time through preprocessor macros,
 * with compile-time size and alignment, and accessors to the "rows" and "columns".
 */

#include <iostream>

#include <boost/preprocessor.hpp>

// CUDA attributes
#ifdef __CUDACC__
#define SOA_HOST_ONLY __host__
#define SOA_HOST_DEVICE __host__ __device__
#else
#define SOA_HOST_ONLY
#define SOA_HOST_DEVICE
#endif

// compile-time sized SoA

/* declare SoA data members; these should exapnd to, for example
 *
 *   alignas(ALIGN) double x_[SIZE];
 *
 */

#define _DECLARE_SOA_DATA_MEMBER_IMPL(TYPE, NAME)                                                                                   \
  alignas(ALIGN) TYPE BOOST_PP_CAT(NAME, _[SIZE]);

#define _DECLARE_SOA_DATA_MEMBER(R, DATA, TYPE_NAME)                                                                                \
  BOOST_PP_EXPAND(_DECLARE_SOA_DATA_MEMBER_IMPL TYPE_NAME)

#define _DECLARE_SOA_DATA_MEMBERS(...)                                                                                              \
  BOOST_PP_SEQ_FOR_EACH(_DECLARE_SOA_DATA_MEMBER, ~, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))


/* declare SoA accessors; these should expand to, for example
 *
 *   double* x() { return x_; }
 *
 */

#define _DECLARE_SOA_ACCESSOR_IMPL(TYPE, NAME)                                                                                      \
  SOA_HOST_DEVICE                                                                                                                   \
  TYPE* NAME() { return BOOST_PP_CAT(NAME, _); }

#define _DECLARE_SOA_ACCESSOR(R, DATA, TYPE_NAME)                                                                                   \
  BOOST_PP_EXPAND(_DECLARE_SOA_ACCESSOR_IMPL TYPE_NAME)

#define _DECLARE_SOA_ACCESSORS(...)                                                                                                 \
  BOOST_PP_SEQ_FOR_EACH(_DECLARE_SOA_ACCESSOR, ~, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))


/* assignment of individual fields; these should expand to, for example
 *
 *   x() = other.x();
 *
 */

#define _DECLARE_SOA_ELEMENT_ASSIGNMENT_IMPL(TYPE, NAME)                                                                            \
  NAME() = other.NAME();

#define _DECLARE_SOA_ELEMENT_ASSIGNMENT(R, DATA, TYPE_NAME)                                                                         \
  BOOST_PP_EXPAND(_DECLARE_SOA_ELEMENT_ASSIGNMENT_IMPL TYPE_NAME)

#define _DECLARE_SOA_ELEMENT_ASSIGNMENTS(...)                                                                                       \
  BOOST_PP_SEQ_FOR_EACH(_DECLARE_SOA_ELEMENT_ASSIGNMENT, ~, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))


/* declare AoS-like element accessors; these should expand to, for example
 *
 *   double & x() { return * (soa_.x() + index_); }
 *
 */

#define _DECLARE_SOA_ELEMENT_ACCESSOR_IMPL(TYPE, NAME)                                                                              \
  SOA_HOST_DEVICE                                                                                                                   \
  TYPE & NAME() { return * (soa_. NAME () + index_); }

#define _DECLARE_SOA_ELEMENT_ACCESSOR(R, DATA, TYPE_NAME)                                                                           \
  BOOST_PP_EXPAND(_DECLARE_SOA_ELEMENT_ACCESSOR_IMPL TYPE_NAME)

#define _DECLARE_SOA_ELEMENT_ACCESSORS(...)                                                                                         \
  BOOST_PP_SEQ_FOR_EACH(_DECLARE_SOA_ELEMENT_ACCESSOR, ~, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))


/* dump SoA fields information; these should expand to, for example
 *
 *   std::cout << "  x_[" << SoA::size << "] at " 
 *             << offsetof(SoA, SoA::x_) << " has size " << sizeof(SoA::x_) << std::endl;
 *
 */

#define _DECLARE_SOA_DUMP_INFO_IMPL(TYPE, NAME)                                                                                     \
  std::cout << "  " BOOST_PP_STRINGIZE(NAME) "_[" << SoA::size << "] at "                                                           \
            << offsetof(SoA, SoA:: BOOST_PP_CAT(NAME, _)) << " has size " << sizeof(SoA:: BOOST_PP_CAT(NAME, _)) << '\n';

#define _DECLARE_SOA_DUMP_INFO(R, DATA, TYPE_NAME)                                                                                  \
  BOOST_PP_EXPAND(_DECLARE_SOA_DUMP_INFO_IMPL TYPE_NAME)

#define _DECLARE_SOA_DUMP_INFOS(...)                                                                                                \
  BOOST_PP_SEQ_FOR_EACH(_DECLARE_SOA_DUMP_INFO, CLASS, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))


#define DECLARE_SOA_TEMPLATE(CLASS, ...)                                                                                            \
                                                                                                                                    \
/* dump the SoA internaul structure */                                                                                              \
template <typename T>                                                                                                               \
SOA_HOST_ONLY                                                                                                                       \
void dump() {                                                                                                                       \
  using SoA = T;                                                                                                                    \
  std::cout << #CLASS "<" << SoA::size << ", " << SoA::alignment << "): " << '\n';                                                  \
  std::cout << "  sizeof(...): " << sizeof(CLASS) << '\n';                                                                          \
  std::cout << "  alignof(...): " << alignof(CLASS) << '\n';                                                                        \
  _DECLARE_SOA_DUMP_INFOS(__VA_ARGS__)                                                                                              \
  std::cout << std::endl;                                                                                                           \
}                                                                                                                                   \
                                                                                                                                    \
template <size_t SIZE, size_t ALIGN=0>                                                                                              \
struct CLASS {                                                                                                                      \
                                                                                                                                    \
  /* these could be moved to an external type trait to free up the symbol names */                                                  \
  using self_type = CLASS;                                                                                                          \
  static const size_t size = SIZE;                                                                                                  \
  static const size_t alignment = ALIGN;                                                                                            \
                                                                                                                                    \
  /* AoS-like accessor to individual elements */                                                                                    \
  struct element {                                                                                                                  \
    SOA_HOST_DEVICE                                                                                                                 \
    element(CLASS & soa, size_t index) :                                                                                            \
      soa_(soa),                                                                                                                    \
      index_(index)                                                                                                                 \
    { }                                                                                                                             \
                                                                                                                                    \
    SOA_HOST_DEVICE                                                                                                                 \
    element& operator=(element const& other) {                                                                                      \
      _DECLARE_SOA_ELEMENT_ASSIGNMENTS(__VA_ARGS__)                                                                                 \
      return *this;                                                                                                                 \
    }                                                                                                                               \
                                                                                                                                    \
    SOA_HOST_DEVICE                                                                                                                 \
    element& operator=(element && other) {                                                                                          \
      _DECLARE_SOA_ELEMENT_ASSIGNMENTS(__VA_ARGS__)                                                                                 \
      return *this;                                                                                                                 \
    }                                                                                                                               \
                                                                                                                                    \
    _DECLARE_SOA_ELEMENT_ACCESSORS(__VA_ARGS__)                                                                                     \
                                                                                                                                    \
  private:                                                                                                                          \
    CLASS & soa_;                                                                                                                   \
    const size_t index_;                                                                                                            \
  };                                                                                                                                \
                                                                                                                                    \
  /* AoS-like accessor */                                                                                                           \
  SOA_HOST_DEVICE                                                                                                                   \
  element operator[](size_t index) { return element(*this, index); }                                                                \
                                                                                                                                    \
  /* accessors */                                                                                                                   \
  _DECLARE_SOA_ACCESSORS(__VA_ARGS__)                                                                                               \
                                                                                                                                    \
  /* dump the SoA internal structure */                                                                                             \
  template <typename T> SOA_HOST_ONLY friend void dump();                                                                           \
                                                                                                                                    \
private:                                                                                                                            \
  /* data members */                                                                                                                \
  _DECLARE_SOA_DATA_MEMBERS(__VA_ARGS__)                                                                                            \
}
