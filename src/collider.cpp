#include <chrono>
#include <iostream>
#include <string>

#include "hashes/selectHash.hpp"
#include "strings/randomString.hpp"
#include "strings/sequentialString.hpp"

namespace {
  std::string padLineClear(unsigned int lineLength, unsigned int messageLength) {
    const int requiredPadding = lineLength - messageLength;
    if (requiredPadding > 0) {
      return std::string(requiredPadding, ' ');
    }

    return std::string("");
  }
}

int main(int argc, char* argv[]) {
  //Set up tool
  seedRandom();
  const unsigned int maxStringLength = 100;

  //Check for a string input
  if (argc < 2) {
    std::cerr << "Expected a string to hash" << std::endl;
    return 1;
  }

  //Decide which string mode to use
  bool randomMode = false;
  if (argc >= 3) {
    try {
      randomMode = (std::string(argv[2]) == "true");
    } catch (const std::exception&) {
      std::cerr << "Failed to process random string toggle, expected 'true' or 'false'" << std::endl;
      return 1;
    }
  }

  //Take the input and calculate its hash
  std::string inputString(argv[1]);
  const std::string inputHash = hashStrings(&inputString, 1);
  std::cout << "'" << inputString << "' hashes to '" << inputHash \
            << "', searching for collisions..." << std::endl;

  //Data required for progress messages
  unsigned int attempts = 0;
  std::string lastRateMessage = "";
  std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

  //Initialise the string according to the mode
  std::string searchString;
  if (randomMode) {
    std::cout << "Running in random mode, max length " << maxStringLength << "\n" << std::endl;
    searchString = generateRandomString(getRandom(1, maxStringLength));
  } else {
    std::cout << "Running in sequential mode\n" << std::endl;
    searchString = "a";
  }

  //Search for hash collisions with the given string
  while (true) {
    //Calculate the hash and check for a match
    const std::string searchStringHash = hashStrings(&searchString, 1);
    if (searchStringHash == inputHash) {
      if (searchString != inputString) {
        //Replace the progress line with the collision string
        const std::string message = "Found collision with '" + searchString + "'";
        std::cout << message << padLineClear(lastRateMessage.size(), message.size()) \
                  << std::endl;

        //Replace the progress line
        std::cout << lastRateMessage << "\r" << std::flush;
      }
    }

    //Send a progress message every 10,000,000 attempts
    attempts++;
    if (attempts >= 10000000) {
      //Calculate the time taken
      const auto deltaTime = std::chrono::system_clock::now() - start;
      const unsigned int deltaTimeMilli =
        std::chrono::duration_cast<std::chrono::milliseconds>(deltaTime).count();

      //Calculate the hash rate and format
      const double hashRate = ((double)attempts / (double)deltaTimeMilli) * 1000.0;
      const std::string message = "String length " + std::to_string(searchString.size()) \
                                  + ", " + std::to_string((unsigned int)(hashRate)) \
                                  + " hashes per second";

      //Clear the line and update it
      std::cout << message << padLineClear(lastRateMessage.size(), message.size()) \
                << "\r" << std::flush;

      //Reset the timer and attempts
      lastRateMessage = message;
      start = std::chrono::system_clock::now();
      attempts = 0;
    }

    //Generate the next string
    if (randomMode) {
      searchString = generateRandomString(getRandom(1, maxStringLength));
    } else {
      searchString = nextString(searchString);
    }
  }

  return 0;
}
