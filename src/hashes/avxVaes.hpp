#ifndef AVX512VAESHASH
#define AVX512VAESHASH

#if defined(__AVX512F__) && defined(__AVX512BW__) && defined(__AVX512VL__) && \
    defined(__VAES__) && defined(__AVX2__) && defined(__SSE2__) && \
    defined(__BMI2__) && defined(UINT64_MAX)
#else
  #error "Attempting to compile unsupported hash"
#endif

#include <cstdint>
#include <string>

#include <immintrin.h>
#include <emmintrin.h>

/*
 - Use various vector extensions to feed the inputs through AES rounds
   - This isn't even close to full AES, but the instructions get the job done
 - Requires AVX-512(F, BW, VL), VAES, AVX2, SSE2 and BMI2
*/

static std::string hashStrings(const std::string* inputs, unsigned int inputCount) {
  __m512i last = _mm512_setzero_epi32();
  for (unsigned int i = 0; i < inputCount; i++) {
    const uint8_t* input = (uint8_t*)inputs[i].c_str();
    unsigned int stringSize = inputs[i].length();

    while (stringSize >= 64) {
      const __m512i a = _mm512_loadu_epi8(input);
      last = _mm512_aesenc_epi128(last, a);

      stringSize -= 64;
      input += 64;
    }

    if (stringSize > 0) {
      const __mmask64 mask = _bzhi_u64(0xFFFFFFFFFFFFFFFF, stringSize);
      const __m512i a = _mm512_maskz_loadu_epi8(mask, input);
      last = _mm512_aesenc_epi128(last, a);
    }
  }

  /*
   - Rotate and sum until every element contains the sum of all elements
   - Effectively, a horizontal add
  */
  __m512i rotateOne = _mm512_alignr_epi64(last, last, 1);
  __m512i sumRotateOne = _mm512_add_epi64(last, rotateOne);
  __m512i rotateTwo = _mm512_alignr_epi64(sumRotateOne, sumRotateOne, 2);
  __m512i sumRotateTwo = _mm512_add_epi64(rotateTwo, sumRotateOne);
  __m512i rotateFour = _mm512_alignr_epi64(sumRotateTwo, sumRotateTwo, 4);
  __m512i sumRotateFour = _mm512_add_epi64(rotateFour, sumRotateTwo);

  //Spread the result out over 128 bits, adjust the range to ['A', 'P']
  std::string output(16, 0);

  //Load result into the vector
  const __m128i resultVec = _mm512_castsi512_si128(sumRotateFour);

  //Shift the result 4 bits right
  const __m128i shift = _mm_set_epi64((__m64)0ull, (__m64)4ull);
  const __m128i shiftedResult = _mm_srlv_epi64(resultVec, shift);

  //Interleave the result and shifted result every 8 bits, clear the high 4 bits
  const __m128i spacedResult = _mm_unpacklo_epi8(resultVec, shiftedResult);
  const __m128i clearedResult = _mm_and_si128(spacedResult, _mm_set1_epi8(0xF));

  //Adjust every element to start at 'A', save the result
  const __m128i addedResult = _mm_add_epi8(clearedResult, _mm_set1_epi8('A'));
  _mm_storeu_epi8((__m128i*)output.data(), addedResult);

  return output;
}
#endif
