#include <iostream>

#include "soa_v4.h"

#define check(X) \
  do { std::cout << #X " is " << (X) << std::endl; } while(false)

// declare a statically-sized SoA, templated on the column size and (optional) alignment

declare_SoA_template(SoA,
  // predefined static scalars
  // size_t size;
  // size_t alignment;

  // columns: one value per element
  SoA_column(double, x),
  SoA_column(double, y),
  SoA_column(double, z),
  SoA_column(uint16_t, colour),
  SoA_column(int32_t, value),
  SoA_column(const char *, name),

  // scalars: one value for the whole structure
  SoA_scalar(const char *, description)
);

using LargeSoA = SoA<1024>;


int main(void) {
  std::cout << std::boolalpha;

  check(sizeof(SoA<1>));
  std::cout << std::endl;

  dump<SoA<1>>();
  dump<SoA<10>>();
  dump<SoA<31>>();
  dump<SoA<32>>();
  std::cout << std::endl;

  dump<SoA<1, 64>>();
  dump<SoA<10, 64>>();
  dump<SoA<31, 64>>();
  dump<SoA<32, 64>>();
  std::cout << std::endl;

  SoA<10, 32> soa;
  check(& soa.z()[7] == & (soa[7].z()));

  soa[7].x() = 0.;
  soa[7].y() = 3.1416;
  soa[7].z() = -1.;
  soa[7].colour() = 42;
  soa[7].value() = 9999;
  soa[7].name() = "element";

  soa[9] = soa[7];

  return not (& soa.z()[7] == & (soa[7].z()));
}
