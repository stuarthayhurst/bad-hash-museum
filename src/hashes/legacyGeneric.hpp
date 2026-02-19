#ifndef LEGACYGENERICHASH
#define LEGACYGENERICHASH

#include <cstdint>
#include <cstddef>
#include <string>

/*
 - The original hash algorithm for Ammonite's shader caching
   - It was too slow (compared to the standard library), and probably a little overkill
 - Iterate over each input and each character, XORing it with every byte of the output
   - Carry forward the previous result from each operation
*/

static std::string hashStrings(const std::string* inputs, unsigned int inputCount) {
  constexpr unsigned int hashWidth = 8;
  uint8_t output[hashWidth] = {0};
  uint8_t prev = 0;

  /*
   - XOR the first byte of the hash with the first character of the first path
   - Sequentially XOR every byte of the hash with the result of the previous
     operation of this stage
   - Repeat this process for every character of every path
  */
  for (unsigned int i = 0; i < inputCount; i++) {
    for (const char& pathCharacter : inputs[i]) {
      output[0] ^= pathCharacter;
      for (unsigned char& outputByte : output) {
        outputByte ^= prev;
        prev = outputByte;
      }
    }
  }

  //Split upper and lower half of each byte, add to 'A' and store
  std::string outputString(sizeof(output) * 2, 0);
  for (std::size_t i = 0; i < sizeof(output); i++) {
    outputString[(i * 2) + 0] = (char)('A' + (char)(output[i] & 0xF));
    outputString[(i * 2) + 1] = (char)('A' + (char)((output[i] >> 4) & 0xF));
  }

  return outputString;
}

#endif
