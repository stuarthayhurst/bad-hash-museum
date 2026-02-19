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
  constexpr unsigned int outputBitWidth = 64;
  uintmax_t output[outputBitWidth / (sizeof(uintmax_t) * 8)] = {0};

  unsigned char accum = 0;
  unsigned char index = 0;

  for (unsigned int i = 0; i < inputCount; i++) {
    for (const char& character : inputs[i]) {
      const unsigned int rotateBits = accum % (sizeof(uintmax_t) * 8);
      output[index % (sizeof(output) / sizeof(uintmax_t))] ^= std::rotr((uintmax_t)character, rotateBits) ^ accum;

      index++;
      accum += character;
    }
  }

  //Split upper and lower half of each byte, add to 'A' and store
  std::string outputString(sizeof(output), 0);
  for (std::size_t i = 0; i < sizeof(output); i++) {
    outputString[i] = (char)('A' + (char)((output[i / (sizeof(output) / 2)] >> (i * 4)) & 0xF));
  }

  return outputString;
}

#endif
