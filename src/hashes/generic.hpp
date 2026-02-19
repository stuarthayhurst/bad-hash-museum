#ifndef GENERICHASH
#define GENERICHASH

#include <bit>
#include <cstdint>
#include <cstddef>
#include <string>

/*
 - The replacement fallback hash for Ammonite
 - Less thorough than the original, but much faster
 - Uses larger output elements to allow characters to straddle bytes,
   and improve performance
*/

static std::string hashStrings(const std::string* inputs, unsigned int inputCount) {
  constexpr unsigned int resultBitWidth = 64;
  constexpr unsigned int resultByteWidth = resultBitWidth / 8;
  uintmax_t result[resultByteWidth / sizeof(uintmax_t)] = {0};
  constexpr unsigned int resultElements = sizeof(result) / sizeof(uintmax_t);

  unsigned char accum = 0;
  unsigned char index = 0;

  for (unsigned int i = 0; i < inputCount; i++) {
    for (const char& character : inputs[i]) {
      const unsigned int rotateBits = accum % (sizeof(uintmax_t) * 8);
      result[index % resultElements] ^= std::rotr((uintmax_t)character, rotateBits) ^ accum;

      index++;
      accum += character;
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
