#include <iostream>
#include <vector>

/*
 * Array of structure
 */

#define check(X) \
  do { std::cout << #X " is " << (X) << std::endl; } while(false)

struct element {
  double x;
  double y;
  double z;
  uint16_t colour;
  int32_t value;
  const char* name;
};

template <typename T>
T& operator<<(T& out, element const& e) {
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
  for (typename T::size_type i = 0; i < elements.size() - 1; ++i)
    std::cout << "  " << elements[i] << ",\n";
  std::cout << "  " << elements[elements.size() - 1] << "\n";
  std::cout << "}" << std::endl;
}

int main(void) {
  std::cout << std::boolalpha;

  check(sizeof(element));
  std::cout << std::endl;

  std::vector<element> vec(10);

  vec[7].x = 0.;
  vec[7].y = 3.1416;
  vec[7].z = -1.;
  vec[7].colour = 42;
  vec[7].value = 9999;
  vec[7].name = "element";
  std::cout << "vec[7]: " << vec[7] << std::endl;
  std::cout << std::endl;

  vec[0] = { 1., 2., 3., 16, 1, "hello world" };
  vec[9] = vec[7];
  std::cout << "vec: ";
  print(vec);
  std::cout << std::endl;

  return 0;
}
