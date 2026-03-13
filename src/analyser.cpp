#include <cmath>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <unordered_map>
#include <string>

#include "hashes/selectHash.hpp"
#include "strings/randomString.hpp"
#include "strings/sequentialString.hpp"

//Data
namespace {
  std::unordered_map<std::string, uintmax_t> hashValues;
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
      //Calculate the hash and increment the total for that element
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

namespace {
  unsigned int compareHashes(const std::string& a, const std::string& b) {
    unsigned int totalDifference = 0;
    for (unsigned int i = 0; i < a.length(); i++) {
      //Find the smallest absolute difference between the characters (A -> P)
      unsigned int difference = std::abs(a[i] - b[i]);
      if (difference > 8) {
        difference = 16 - difference;
      }

      totalDifference += difference;
    }

    return totalDifference;
  }

  void analyseSequence(uintmax_t stringCount, unsigned int stride) {
    //Check each pair of hashes for sequential strings
    std::string currentString = "a";
    std::string lastHash = hashStrings(&currentString, 1);
    uintmax_t totalHashDifference = 0;
    for (uintmax_t i = 0; i < stringCount; i++) {
      //Generate the next string and its hash
      for (unsigned int j = 0; j < stride; j++) {
        currentString = nextString(currentString);
      }
      const std::string currentHash = hashStrings(&currentString, 1);

      //Compare the hashes and record the current as the previous
      totalHashDifference += compareHashes(lastHash, currentHash);
      lastHash = currentHash;
    }

    const unsigned int outputCharacters = 16;
    const unsigned int maxDifference = 8;
    const unsigned int bestAverageDifference = outputCharacters * (maxDifference / 2);

    //Print the average difference
    std::cout << "Average sequential hash difference (stride " << stride << "): " \
              << (double)totalHashDifference / stringCount << " / " \
              << bestAverageDifference << std::endl;
  }
}

namespace {
  bool checkPattern(const std::string& hash, bool showMatches) {
    /*
     - Search for a pattern in the hash
     - Use the first byte as the pattern, and grown until one is found or
       the pattern size is half the hash
     - Use an offset and grow it up to half the string size
    */
    for (unsigned int offset = 0; offset < hash.length() / 2; offset++) {
      unsigned int remainingHashLength = hash.length() - offset;
      for (unsigned int patternSize = 1; patternSize < remainingHashLength / 2; patternSize++) {
        bool pattern = true;
        for (unsigned int index = 0; index < remainingHashLength; index++) {
          //Move on to the next pattern if any byte doesn't match
          if (hash[index + offset] != hash[(index % patternSize) + offset]) {
            pattern = false;
            break;
          }
        }

        if (pattern) {
          if (showMatches) {
            std::cout << "Y: " << hash << std::endl;
          }

          return true;
        }
      }
    }

    if (showMatches) {
      std::cout << "N: " << hash << std::endl;
    }
    return false;
  }

  void analyseConstantPattern(bool showMatches) {
    //Look for a pattern in every constant-byte string
    unsigned int totalPatterns = 0;
    const unsigned int stringLength = 8;
    for (unsigned int value = 0; value < std::numeric_limits<char>::max() + 1; value++) {
      const std::string inputString = std::string(stringLength, (char)value);

      //Hash the string and check for a pattern
      if (checkPattern(hashStrings(&inputString, 1), showMatches)) {
        totalPatterns++;
      }
    }

    if (showMatches) {
      std::cout << "\n";
    }

    std::cout << totalPatterns << " / " << std::numeric_limits<char>::max() + 1 \
              << " constant-byte inputs produce a pattern" << std::endl;
  }
}

int main(int argc, char* argv[]) {
  //Set up tool
  seedRandom();

  //Decide how many strings to try in the distribution
  uintmax_t distributionStringCount = 10000000;
  if (argc >= 2) {
    try {
      distributionStringCount = std::stoull(argv[1], nullptr, 10);
    } catch (const std::exception&) {
      std::cerr << "Failed to process distribution string count" << std::endl;
      return 1;
    }
  }

  //Decide how many strings to try in a sequence
  uintmax_t sequenceStringCount = 10000000;
  if (argc >= 3) {
    try {
      sequenceStringCount = std::stoull(argv[2], nullptr, 10);
    } catch (const std::exception&) {
      std::cerr << "Failed to process sequence string count" << std::endl;
      return 1;
    }
  }

  //Decide whether to show pattern matches
  bool showMatches = false;
  if (argc >= 4) {
    try {
      showMatches = (std::string(argv[3]) == "true");
    } catch (const std::exception&) {
      std::cerr << "Failed to process show matches toggle, expected 'true' or 'false'" << std::endl;
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
  std::cout << "\n";

  //Analyse the difference between each string in a sequence
  analyseSequence(sequenceStringCount, 1);
  analyseSequence(sequenceStringCount, 16);
  std::cout << "\n";

  analyseConstantPattern(showMatches);

  return 0;
}
