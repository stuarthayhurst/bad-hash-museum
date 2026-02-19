#ifndef AVX2HASH
#define AVX2HASH

#if defined(__AVX2__) && defined(__SSE4_1__) && defined(__SSE3__) && defined(__SSE2__)
#else
  #error "Attempting to compile unsupported hash"
#endif

#include <cstdint>
#include <string>

#include <immintrin.h>
#include <emmintrin.h>

/*
 - Use AVX2 and SSE4.1 - 2 to hash an array of input strings
 - For each 128-bit input chunk:
   - Redistribute the data through the block, to avoid data concentrated at the start
   - Perform 4 quarter rotations, each time:
     - Multiply by the current hash, then sum the intermediates
     - XOR the result with the current hash
  - While AVX-512 would speed up loading the remainder, it'd defeat the point of not using VAES
*/

static std::string hashStrings(const std::string* inputs, unsigned int inputCount) {
  const __m128i shuffleVec = _mm_setr_epi8(0, 15, 2, 13, 4, 11, 6, 9,
                                           8, 7, 10, 5, 12, 3, 14, 1);
  __m128i hash = _mm_setr_epi16(0xF - 1, 0xFF - 1, 0xFFA - 1, 0xFFF - 1,
                                0xF - 1, 0xFF - 1, 0xFFA - 1, 0xFFF - 1);

  for (unsigned int i = 0; i < inputCount; i++) {
    const uint8_t* input = (uint8_t*)inputs[i].data();
    unsigned int inputSize = inputs[i].length();

    while (inputSize >= 16) {
      //Load 128 bits of input
      __m128i data = _mm_loadu_si128((const __m128i*)input);

      //Spread input data out
      data = _mm_shuffle_epi8(data, shuffleVec);

      __m128i rotatedData = data;
      for (unsigned int i = 0; i < 4; i++) {
        //Rotate the whole vector right by 32 bits
        rotatedData = _mm_alignr_epi8(rotatedData, rotatedData, 4);

        //Multiply the current hash and the rotated data, then sum the intermediates
        __m128i influencedData = _mm_madd_epi16(hash, rotatedData);

        //XOR with the current hash
        hash = _mm_xor_si128(influencedData, hash);
      }

      inputSize -= 16;
      input += 16;
    }

    if (inputSize > 0) {
      //Load the bulk of the remainder
      __m128i inputMask = _mm_setr_epi32((inputSize >= 4) ? -1 : 0,
                                         (inputSize >= 8) ? -1 : 0,
                                         (inputSize >= 12) ? -1 : 0,
                                         0);
      __m128i data = _mm_maskload_epi32((const int32_t*)input, inputMask);

      //Load the final remainder
      switch (inputSize) {
      case 15:
        data = _mm_insert_epi8(data, input[14], 14);
        [[fallthrough]];
      case 14:
        data = _mm_insert_epi8(data, input[13], 13);
        [[fallthrough]];
      case 13:
        data = _mm_insert_epi8(data, input[12], 12);
        break;
      case 11:
        data = _mm_insert_epi8(data, input[10], 10);
        [[fallthrough]];
      case 10:
        data = _mm_insert_epi8(data, input[9], 9);
        [[fallthrough]];
      case 9:
        data = _mm_insert_epi8(data, input[8], 8);
        break;
      case 7:
        data = _mm_insert_epi8(data, input[6], 6);
        [[fallthrough]];
      case 6:
        data = _mm_insert_epi8(data, input[5], 5);
        [[fallthrough]];
      case 5:
        data = _mm_insert_epi8(data, input[4], 4);
        break;
      case 3:
        data = _mm_insert_epi8(data, input[2], 2);
        [[fallthrough]];
      case 2:
        data = _mm_insert_epi8(data, input[1], 1);
        [[fallthrough]];
      case 1:
        data = _mm_insert_epi8(data, input[0], 0);
        break;
      }

      //Spread input data out
      data = _mm_shuffle_epi8(data, shuffleVec);

      __m128i rotatedData = data;
      for (unsigned int i = 0; i < 4; i++) {
        //Rotate the whole vector right by 32 bits
        rotatedData = _mm_alignr_epi8(rotatedData, rotatedData, 4);

        //Multiply the current hash and the rotated data, then sum the intermediates
        __m128i influencedData = _mm_madd_epi16(hash, rotatedData);

        //XOR with the current hash
        hash = _mm_xor_si128(influencedData, hash);
      }
    }
  }

  //Spread the result out over 128 bits, adjust the range to ['A', 'P']
  std::string output(16, 0);

  //Load sum result and spread out over a vector
  const __m128i resultVec = _mm_add_epi64(hash, _mm_alignr_epi8(hash, hash, 8));

  //Shift the result 4 bits right
  const __m128i shift = _mm_set_epi64((__m64)0ull, (__m64)4ull);
  const __m128i shiftedResult = _mm_srlv_epi64(resultVec, shift);

  //Interleave the result and shifted result every 8 bits, clear the high 4 bits
  const __m128i spacedResult = _mm_unpacklo_epi8(resultVec, shiftedResult);
  const __m128i clearedResult = _mm_and_si128(spacedResult, _mm_set1_epi8(0xF));

  //Adjust every element to start at 'A', save the result
  const __m128i addedResult = _mm_add_epi8(clearedResult, _mm_set1_epi8('A'));
  _mm_storeu_si128((__m128i*)output.data(), addedResult);

  return output;
}

#endif
