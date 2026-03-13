#ifndef SEQUENTIALSTRING
#define SEQUENTIALSTRING

#include <string>

static std::string nextString(const std::string& currentString) {
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

#endif
