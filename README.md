## Bad Hash Museum
  - A museum of bad hashing algorithms, written for fun
    - Hashes were originally part of [Ammonite Engine](https://github.com/stuarthayhurst/ammonite-engine), but were copied here for archival
    - Includes some tools used to develop and test the hashes
  - Please don't ever use any of these anywhere near anything related to production

## Hashes
  - To select a hash specific, use `HASH=[key] make ...`
    - Defaults to `generic`
  - Generic - `generic`
    - Candidate for legacy-generic's replacement
  - Generic (legacy) - `legacy-generic`
    - Original hash algorithm for Ammonite's shader caching, slow and weak
  - Generic (simple) - `simple-generic`
    - An even more simple candidate for legacy-generic's replacement
  - AVX-512 + VAES - `avx-vaes`
    - Push blocks of data each through a single round of AES by abusing VAES instructions
  - AVX2 - `avx2`
    - An actual attempt at a hash using SIMD intrinsics
  - Standard library - `stdlib`
    - Use `std::hash`, as a reference point for the other hashes

## Tools
  - Benchmark
    - Benchmark the hash on a collection of strings
    - Compile with `make bench`
    - Run `./benchHash [iterations] [string count] [max string length]`
      - All arguments are optional, but previous arguments must be specified
      - Defaults to `10000000`, `100` and `100`
  - Hasher
    - Hash a single string with the chosen algorithm
    - Compile with `make hasher`
    - Run `./hasher [string]`
      - A string to hash must be supplied
  - Analyser
    - Check the distribution and differences of hash outputs, as well as checking for patterns
    - Compile with `make analyse`
    - Run `./analyser [distribution count] [sequence count] [show matches (true / false)]`
      - All arguments are optional, but previous arguments must be specified
      - Defaults to `10000000`, `10000000` and `false`
  - Collision tester
    - Attempt to find strings that have collisions with an input string
    - Compiler with `make collide`
    - Run `./collider [string] [random mode (true / false)]`
      - A string to hash must be supplied, random mode defaults to false

## Build configuration
  - The Makefile supports several environment variables to configure the build
  - `CXX`: Set the compiler
  - `CXXFLAGS`: Set additional compiler flags
  - `ARCH`: Force a specific architecture
    - Defaults to `native`
  - `DEBUG`: Enable verbose build output, debug symbols and sanitisers
  - `VERBOSE`: Enable verbose build output, including algorithm selection
  - `HASH`: Configure the hash algorithm, as described [earlier](#hashes)
    - Defaults to `generic`

## Performance
  - Benchmarked on a Ryzen 7 7700X using `g++-15` and `clang++-22`

    | Hash     | Hash rate (`g++`) | Hash rate (`clang++`) | Seq. diff (1) | Seq. diff (16) | Patterns found |
    |:---------|:------------------|:----------------------|:--------------|:---------------|:---------------|
    | Generic  | 1915 MB/s         | 2139 MB/s             | 28.767        | 51.0085        | 29             |
    | Simple   | 2060 MB/s         | 2399 MB/s             | 10.918        | 17.2013        | 1              |
    | Legacy   | 586 MB/s          | 1321 MB/s             | 4.93122       | 21.9216        | 1              |
    | AVX-VAES | 12560 MB/s        | 12959 MB/s            | 64.2497       | 63.9986        | 0              |
    | AVX2     | 4516 MB/s         | 4491 MB/s             | 35.0411       | 53.1389        | 0              |
    | stdlib   | 4559 MB/s         | 5359 MB/s             | 63.9982       | 63.9998        | 0              |
