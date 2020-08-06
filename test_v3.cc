#include <iostream>

#include "soa_v3.h"

#define check(X) \
  do { std::cout << #X " is " << (X) << std::endl; } while(false)


DECLARE_SOA_TEMPLATE(SoA,
  (double, x),
  (double, y),
  (double, z),
  (uint16_t, colour),
  (int32_t, value),
  (const char *, name)
);


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
