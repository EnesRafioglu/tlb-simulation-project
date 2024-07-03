# Build the tests
cmake -S . -B build
cmake --build build

# Run the tests
./build/c_tests
./build/cpp_tests
