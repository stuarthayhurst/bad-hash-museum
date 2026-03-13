#include <chrono>
#include <exception>
#include <iostream>
#include <string>

#include "hashes/selectHash.hpp"
#include "strings/randomString.hpp"

int main(int argc, char* argv[]) {
  unsigned int inputCount = 100;
  unsigned int maxStringLength = 100;
  uintmax_t iterations = 10000000;

  if (argc >= 2) {
    try {
      iterations = std::stoull(argv[1], nullptr, 10);
    } catch (const std::exception&) {
      std::cerr << "Failed to process iterations" << std::endl;
      return 1;
    }
  }

  if (argc >= 3) {
    try {
      inputCount = std::stoi(argv[2], nullptr, 10);
    } catch (const std::exception&) {
      std::cerr << "Failed to process string count" << std::endl;
      return 1;
    }
  }

  if (argc >= 4) {
    try {
      maxStringLength = std::stoi(argv[3], nullptr, 10);
    } catch (const std::exception&) {
      std::cerr << "Failed to process maximum string length" << std::endl;
      return 1;
    }
  }

  std::cout << "Generating " << inputCount << " string(s), maximum length " \
            << maxStringLength << std::endl;
  std::string* inputs = new std::string[inputCount];

  //Generate random strings
  seedRandom();
  for (unsigned int i = 0; i < inputCount; i++) {
    //Create a string with a random length, up to maxStringLength
    const unsigned int stringLength = getRandom(1, maxStringLength);
    inputs[i] = generateRandomString(stringLength);
  }

  //Stop compilers removing the loop
  unsigned int sum = 0;

  //Run the hash on the strings
  const std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
  const unsigned int hashInputCount = 2;
  uintmax_t totalHashedBytes = 0;
  for (uintmax_t i = 0; i < iterations; i++) {
    //Hash the strings
    uintmax_t baseStringIndex = (i * hashInputCount) % inputCount;
    sum += hashStrings(&inputs[baseStringIndex], hashInputCount)[0];

    //Record the number of bytes processed
    for (unsigned int stringIndex = 0; stringIndex < hashInputCount; stringIndex++) {
      totalHashedBytes += inputs[baseStringIndex + stringIndex].length();
    }
  }

  //Calculate the time taken
  const std::chrono::system_clock::time_point finish = std::chrono::system_clock::now();
  auto deltaTime = finish - start;
  const auto deltaTimeSeconds = std::chrono::duration_cast<std::chrono::seconds>(deltaTime);
  deltaTime -= deltaTimeSeconds;
  const auto remainderMilli = std::chrono::duration_cast<std::chrono::milliseconds>(deltaTime);

  //Print metrics
  double totalTime = (double)deltaTimeSeconds.count() + ((double)remainderMilli.count() / 1000.0);
  std::cout << "\nHashed " << hashInputCount * iterations << " strings in " \
                           << totalTime << "s" << std::endl;

  //Print strings per second
  std::cout << "  " << (hashInputCount * iterations) / totalTime \
            << " strings per second " << std::endl;

  //Print megabytes per second
  std::cout << "  " << (totalHashedBytes / totalTime) / 1000000.0 << " MB/s " << std::endl;

  std::cout << "Magic number: " << (int)sum << std::endl;

  delete [] inputs;
  return 0;
}
