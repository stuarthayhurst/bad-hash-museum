#ifndef STANDARDLIBRARYHASH
#define STANDARDLIBRARYHASH

#include <cstdint>
#include <string>

/*
 - Hash an array of string using std::hash and std::to_string
 - Written as performance reference for the other hashes
*/

static std::string hashStrings(const std::string* inputs, unsigned int inputCount) {
  uint64_t total = 0;
  for (unsigned int i = 0; i < inputCount; i++) {
    total += std::hash<std::string>{}(inputs[i]);
  }

  return std::to_string(total);
}

#endif
