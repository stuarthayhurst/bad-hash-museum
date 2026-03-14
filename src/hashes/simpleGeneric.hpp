#ifndef SIMPLEGENERICHASH
#define SIMPLEGENERICHASH

#include <algorithm>
#include <bit>
#include <cstdint>
#include <cstddef>
#include <string>

/*
 - Rotate an integer left by 21 bits, enough to move it partially
   out of the region the next character is likely to affect
 - Multiply the next character by 1024 - 1 to widen it, then sum with the rotation
*/

static std::string INLINEATTRIBUTE hashStrings(const std::string* inputs, unsigned int inputCount) {
  constexpr unsigned int resultBitWidth = 64;
  constexpr unsigned int resultByteWidth = resultBitWidth / 8;
  constexpr unsigned int resultElements = std::max(resultByteWidth / sizeof(uintmax_t), (uintmax_t)1);
  uintmax_t result[resultElements] = {0};

  unsigned int index = 0;
  for (unsigned int i = 0; i < inputCount; i++) {
    for (const char& character : inputs[i]) {
      const uintmax_t rotated = std::rotl(result[(index) % resultElements], 16 + 3);
      result[(index) % resultElements] = rotated + (unsigned int)character * (1024 - 1);
      index++;
    }
  }

  //Split upper and lower half of each byte, add to 'A' and store
  constexpr unsigned int outputSize = sizeof(result) * 2;
  std::string outputString(outputSize, 0);
  for (std::size_t i = 0; i < outputSize; i++) {
    //Map from output bytes to elements, filling half a byte each time
    const uintmax_t resultElement = result[(i / sizeof(uintmax_t)) / 2];

    outputString[i] = (char)('A' + (char)((resultElement >> (i * 4)) & 0xF));
  }

  return outputString;
}

#endif
