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
