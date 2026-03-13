#ifndef RANDOMSTRING
#define RANDOMSTRING

#include <cstdlib>
#include <ctime>
#include <string>

static void seedRandom() {
  std::srand((unsigned)std::time(nullptr));
}

static unsigned int getRandom(unsigned int lower, unsigned int upper) {
  return lower + (std::rand() % ((upper - lower) + 1));
}

//Return a random string of at least length 1
static std::string generateRandomString(unsigned int stringLength) {
  std::string output = std::string(stringLength, 0);

  //Fill the string
  for (unsigned int stringIndex = 0; stringIndex < stringLength; stringIndex++) {
    switch (getRandom(0, 2)) {
    case 0:
      output[stringIndex] = getRandom('a', 'z');
      break;
    case 1:
      output[stringIndex] = getRandom('A', 'Z');
      break;
    case 2:
      output[stringIndex] = getRandom('0', '9');
      break;
    }
  }

  return output;
}

#endif
