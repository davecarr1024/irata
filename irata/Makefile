.PHONY: all configure build test format clean tidy

all: format test

configure:
	cmake -S . -B build -DIRATA_ENABLE_SANITIZER=ON

build: configure
	cmake --build build

test: build
	ctest --test-dir build --output-on-failure

format: configure
	find sim \( -name '*.cpp' -o -name '*.hpp' \) -exec clang-format -i {} +

clean:
	rm -rf build

tidy: format
	find sim -name '*.cpp' | xargs -I{} clang-tidy {} -- -Isim/include -Iextern/googletest/googlemock/include/ -Iextern/googletest/googletest/include/

coverage:
	cmake -S . -B build -DIRATA_ENABLE_COVERAGE=ON -DIRATA_ENABLE_SANITIZER=ON
	cmake --build build
	LLVM_PROFILE_FILE="build/irata.profraw" ./build/sim/sim_tests
	@llvm-profdata merge -sparse build/irata.profraw -o build/irata.profdata
	@llvm-cov show ./build/sim/sim_tests -instr-profile=build/irata.profdata \
		-ignore-filename-regex='(gtest|gmock|_deps)' -format=text -Xdemangler=c++filt | awk '\
	/^[\\/].*\\.(cpp|cc|hpp|h):$$/ { \
	  file = $$0; \
	  has_output = 0; \
	  next; \
	} \
	/^[[:space:]]*[0-9]+[|][[:space:]]*0[|]/ { \
	  if (!has_output) { \
	    print ""; \
	    print "FILE: " file; \
	    print "--------------------------"; \
	    has_output = 1; \
	  } \
	  print $$0; \
	}'
