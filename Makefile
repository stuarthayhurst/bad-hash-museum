CXXFLAGS += -Wall -Wextra -Werror -Wpedantic -std=c++23 -O3

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
else
  HASHNUM := 0
endif
CXXFLAGS += -DSELECTHASH=$(HASHNUM)

ifeq ($(DEBUG),true)
  CXXFLAGS += -g -fno-omit-frame-pointer -fsanitize=address,undefined -DVERBOSE
else ifeq ($(VERBOSE),true)
  CXXFLAGS += -DVERBOSE
endif

%: src/%.cpp
	$(CXX) "src/$@.cpp" -o "$@" $(CXXFLAGS)

.PHONY: clean all
all:
	$(MAKE) benchHash
clean:
	@rm -fv benchHash
