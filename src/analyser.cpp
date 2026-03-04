#include <cstdint>
#include <iostream>
#include <unordered_map>
#include <string>

#include "hashes/selectHash.hpp"
#include "randomString.hpp"

//Data
namespace {
  std::unordered_map<std::string, uintmax_t> hashValues;
}

//String generation helpers
namespace {
  std::string nextString(const std::string& currentString) {
    std::string nextString = currentString;
    const unsigned int stringLength = currentString.size();

    /*
     - Iterate over the string, incrementing each character
     - Stop as soon as a character doesn't roll over (most of the time)
    */
    for (unsigned int stringIndex = 0; stringIndex < stringLength; stringIndex++) {
      const char currentChar = nextString[stringIndex];
      if (currentChar == 'z') {
        //Finished the lower-case characters, move to upper-case and stop
        nextString[stringIndex] = 'A';
        break;
      } else if (currentChar == 'Z') {
        //Finished the upper-case characters, move to numerical and stop
        nextString[stringIndex] = '0';
        break;
      } else if (currentChar == '9') {
        //Resize the string if required
        if (stringIndex == stringLength - 1) {
          nextString.resize(stringLength + 1, 'a');
        }

        //Reset the character
        nextString[stringIndex] = 'a';
      } else {
        //Within a valid range, increment and stop
        nextString[stringIndex]++;
        break;
      }
    }

    return nextString;
  }
}

//Distribution helpers
namespace {
  //Fill hashValues with totals from hashing random strings
  void generateDistributionRandom(uintmax_t stringCount) {
    const unsigned int maxStringLength = 100;
    for (uintmax_t i = 0; i < stringCount; i++) {
      //Generate a random string of a random length
      const unsigned int stringLength = getRandom(1, maxStringLength);
      const std::string hashInput = generateRandomString(stringLength);

      //Calculate the hash and increment to total for that element
      const std::string hashOutput = hashStrings(&hashInput, 1);
      if (hashValues.contains(hashOutput)) {
        hashValues[hashOutput]++;
      } else {
        hashValues[hashOutput] = 1;
      }
    }
  }

  //Fill hashValues with totals from hashing consecutive strings
  void generateDistributionLinear(uintmax_t stringCount) {
    std::string currentString = "a";
    for (uintmax_t i = 0; i < stringCount; i++) {
      //Calculate the hash and increment to total for that element
      const std::string hashOutput = hashStrings(&currentString, 1);
      if (hashValues.contains(hashOutput)) {
        hashValues[hashOutput]++;
      } else {
        hashValues[hashOutput] = 1;
      }

      //Generate the next string
      currentString = nextString(currentString);
    }
  }

  void analyseDistribution(uintmax_t stringCount) {
    const uintmax_t totalCollisions = stringCount - hashValues.size();
    std::cout << "  Total collisions: " << totalCollisions << std::endl;
    std::cout << "  Collision rate: " << (double)totalCollisions / (double)(stringCount - 1) \
              << std::endl;
  }

  //Reset hashValues
  void resetDistribution() {
    hashValues.clear();
  }
}

int main(int argc, char* argv[]) {
  //Set up tool
  seedRandom();

  //Decide how many strings to try
  uintmax_t distributionStringCount = 10000000;
  if (argc >= 2) {
    try {
      distributionStringCount = std::stoull(argv[1], nullptr, 10);
    } catch (const std::exception&) {
      std::cerr << "Failed to process string count" << std::endl;
      return 1;
    }
  }

  //Generate and analyse the hash distribution with random inputs
  std::cout << "Random hash inputs:" << std::endl;
  generateDistributionRandom(distributionStringCount);
  analyseDistribution(distributionStringCount);
  resetDistribution();
  std::cout << "\n";

  //Generate and analyse the hash distribution with sequential inputs
  std::cout << "Sequential hash inputs:" << std::endl;
  generateDistributionLinear(distributionStringCount);
  analyseDistribution(distributionStringCount);
  resetDistribution();

  return 0;
}
