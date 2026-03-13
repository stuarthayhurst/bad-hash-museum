#include <chrono>
#include <iostream>
#include <string>

#include "hashes/selectHash.hpp"
#include "sequentialString.hpp"

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
  if (argc < 2) {
    std::cerr << "Expected a string to hash" << std::endl;
    return 1;
  } else if (argc > 2) {
    std::cerr << "Expected 1 argument, got " << argc - 1 << std::endl;
    return 1;
  }

  std::string inputString(argv[1]);
  const std::string inputHash = hashStrings(&inputString, 1);
  std::cout << "'" << inputString << "' hashes to '" << inputHash \
            << "', searching for collision...\n" << std::endl;

  //Data required for progress messages
  unsigned int attempts = 0;
  std::string lastRateMessage = "";
  std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

  //Search for hash collisions with the given string
  std::string searchString = "a";
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
    searchString = nextString(searchString);
  }

  return 0;
}
