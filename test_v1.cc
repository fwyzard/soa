#include <cstring>
#include <iostream>

#include "soa_v1.h"

#define check(X) \
  do { std::cout << #X " is " << (X) << std::endl; } while(false)

template <typename T>
T& operator<<(T& out, SoAEntry const& e) {
  out << "{ ";
  out << ".x = " << e.x << ", ";
  out << ".y = " << e.y << ", ";
  out << ".z = " << e.z << ", ";
  out << ".colour = " << e.colour << ", ";
  out << ".value = " << e.value << ", ";
  out << ".name = ";
  if (e.name)
    out << '"' << e.name << '"';
  else
    out << "NULL";
  out << " }";
  return out;
}

template <typename T>
void print(T const& elements) {
  std::cout << "{\n";
  /*
  for (typename T::size_type i = 0; i < elements.size() - 1; ++i)
    std::cout << "  " << elements[i] << ",\n";
  std::cout << "  " << elements[elements.size() - 1] << "\n";
  */
  bool first = true;
  for (auto const& entry: elements) {
    std::cout << (first ? "  " : ",\n  ") << entry;
    first = false;
  }
  std::cout << "\n}" << std::endl;
}

int main(void) {
  std::cout << std::boolalpha;

  check(sizeof(SoATemplate<1>));
  check(SoATemplate<1>::extent());
  std::cout << std::endl;

  dump<SoATemplate<1>>();
  dump<SoATemplate<10>>();
  dump<SoATemplate<31>>();
  dump<SoATemplate<32>>();
  std::cout << std::endl;

  dump<SoATemplate<1, 64>>();
  dump<SoATemplate<10, 64>>();
  dump<SoATemplate<31, 64>>();
  dump<SoATemplate<32, 64>>();
  std::cout << std::endl;

  SoATemplate<10, 32> soa;
  std::memset(&soa, 0x00, soa.extent());
  check(& soa.z[7] == & (soa[7].z));
  std::cout << std::endl;

  soa[7].x = 0.;
  soa[7].y = 3.1416;
  soa[7].z = -1.;
  soa[7].colour = 42;
  soa[7].value = 9999;
  soa[7].name = "element";
  std::cout << "soa[7]: " << soa[7] << std::endl;
  std::cout << std::endl;

  soa[0] = { 1., 2., 3., 16, 1, "hello world" };
  soa[9] = soa[7];
  std::cout << "soa: ";
  print(soa);
  std::cout << std::endl;

  return 0;
}
