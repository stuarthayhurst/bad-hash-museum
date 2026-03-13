CXXFLAGS += -Wall -Wextra -Werror -Wpedantic -std=c++23 -O3 -fno-math-errno

ifndef ARCH
  CXXFLAGS += -march=native
else
  CXXFLAGS += -march=$(ARCH)
endif

HASHNUM := 0
ifeq ($(HASH),generic)
  HASHNUM := 0
else ifeq ($(HASH),avx-vaes)
  HASHNUM := 1
else ifeq ($(HASH),legacy-generic)
  HASHNUM := 2
else ifeq ($(HASH),stdlib)
  HASHNUM := 3
else ifeq ($(HASH),avx2)
  HASHNUM := 4
else ifeq ($(HASH),simple-generic)
  HASHNUM := 5
else
  HASHNUM := 0
endif
CXXFLAGS += -DSELECTHASH=$(HASHNUM)

ifeq ($(DEBUG),true)
  CXXFLAGS += -g -fno-omit-frame-pointer -fsanitize=address,undefined -DVERBOSE
else ifeq ($(VERBOSE),true)
  CXXFLAGS += -DVERBOSE
endif

%: src/%.cpp src/hashes/*.hpp src/strings/*.hpp
	$(CXX) "src/$@.cpp" -o "$@" $(CXXFLAGS)

.PHONY: clean all bench hash analyse collide
all: bench hash analyser collide
bench: benchHash
hash: hasher
analyse: analyser
collide: collider
clean:
	@rm -fv benchHash hasher analyser collider
