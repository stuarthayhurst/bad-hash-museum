#include <iostream>
#include <string>

#include "hashes/selectHash.hpp"

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "Expected a string to hash" << std::endl;
    return 1;
  } else if (argc > 2) {
    std::cerr << "Expected 1 argument, got " << argc - 1 << std::endl;
    return 1;
  }

  std::string inputString(argv[1]);
  std::cout << hashStrings(&inputString, 1) << std::endl;

  return 0;
}
