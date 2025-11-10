.PHONY: all configure build test format clean tidy

all: format test

configure:
	cmake -S . -B build -DIRATA_ENABLE_SANITIZER=ON

build: configure
	cmake --build build

# Run tests with LeakSanitizer disabled to avoid crashes from intentionally leaked singletons
# See irata_instruction_set.cpp and irata_decl.cpp for details on why singletons are intentionally leaked
test: build
	LSAN_OPTIONS=detect_leaks=0 ctest --test-dir build --output-on-failure
                                
format: configure
	find sim \( -name '*.cpp' -o -name '*.hpp' \) -exec clang-format -i {} +

clean:
	rm -rf build
