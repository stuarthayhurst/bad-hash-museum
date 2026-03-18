#ifndef STANDARDLIBRARYHASH
#define STANDARDLIBRARYHASH

#include <cstdint>
#include <cstddef>
#include <functional>
#include <string>

#if defined(UINT64_MAX)
#else
  #error "Attempting to compile unsupported hash"
#endif

/*
 - Hash an array of string using std::hash
   - Use a faster integer to string conversion to match the format of the other hashes
   - To be entirely fair (performance-wise), it should use std::to_string,
     but that makes it harder to analyse
 - Written as performance reference for the other hashes
*/

static INLINEATTRIBUTE std::string hashStrings(const std::string* inputs, unsigned int inputCount) {
  uint64_t total = 0;
  for (unsigned int i = 0; i < inputCount; i++) {
    total += std::hash<std::string>{}(inputs[i]);
  }

  //Split upper and lower half of each byte, add to 'A' and store
  constexpr unsigned int outputSize = sizeof(total) * 2;
  std::string outputString(outputSize, 0);
  for (std::size_t i = 0; i < outputSize; i++) {
    //Map from output bits to elements, filling half a byte each time
    outputString[i] = (char)('A' + (char)((total >> (i * 4)) & 0xF));
  }

  return outputString;
}

#endif
