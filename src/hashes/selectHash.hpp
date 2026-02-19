#ifndef HASHSELECTOR
#define HASHSELECTOR

#ifndef SELECTHASH
  #error "No hash algorithm specified"
#endif

#if SELECTHASH == 0
  #ifdef VERBOSE
    #pragma message("Selected generic hash")
  #endif

  #include "generic.hpp"
#elif SELECTHASH == 1
  #ifdef VERBOSE
    #pragma message("Selected AVX-512 + VAES hash")
  #endif

  #include "avxVaes.hpp"
#elif SELECTHASH == 2
  #ifdef VERBOSE
    #pragma message("Selected legacy generic hash")
  #endif

  #include "legacyGeneric.hpp"
#elif SELECTHASH == 3
  #ifdef VERBOSE
    #pragma message("Selected standard library hash")
  #endif

  #include "stdlib.hpp"
#else
  #error "Unknown hash algorithm specified"
#endif

#endif
