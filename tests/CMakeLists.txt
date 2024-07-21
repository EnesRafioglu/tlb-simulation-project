#* Setup GoogleTest
cmake_minimum_required(VERSION 3.21)
project(tla_tests)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Latest version of GoogleTest as of this writing (live at head)
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/34ad51b3dc4f922d8ab622491dd44fc2c39afee9.zip
)
FetchContent_MakeAvailable(googletest)

# Configure SystemC
get_filename_component(
  SYSTEMC_HOME
  "../../../systemc"
  REALPATH
  BASE_DIR
  "${CMAKE_BINARY_DIR}"
)
set(SYSTEMC_INCLUDE "${SYSTEMC_HOME}/include")
set(SYSTEMC_LIB "${SYSTEMC_HOME}/lib")

add_library(systemc SHARED IMPORTED)
set_target_properties(systemc PROPERTIES
  IMPORTED_LOCATION "${SYSTEMC_LIB}/${CMAKE_SHARED_LIBRARY_PREFIX}systemc${CMAKE_SHARED_LIBRARY_SUFFIX}"
  INTERFACE_INCLUDE_DIRECTORIES "${SYSTEMC_INCLUDE}"
)

#* Configure tests
enable_testing()

# Set C and C++ standard
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

# Find source and header files
file (GLOB_RECURSE TEST_SOURCES_C CONFIGURE_DEPENDS "src/c/*.cpp")
file (GLOB_RECURSE TEST_SOURCES_CPP CONFIGURE_DEPENDS "src/cpp/*.cpp")

file (GLOB_RECURSE TEST_SOURCES_SHARED CONFIGURE_DEPENDS "src/shared/*.cpp")
file (GLOB_RECURSE TEST_HEADERS_SHARED CONFIGURE_DEPENDS "src/shared/*.hpp")

file (GLOB_RECURSE APP_SOURCES_C CONFIGURE_DEPENDS "../src/*.c")
file (GLOB_RECURSE APP_SOURCES_CPP CONFIGURE_DEPENDS "../src/*.cpp")
file (GLOB_RECURSE APP_HEADERS_C CONFIGURE_DEPENDS "../src/*.h")
file (GLOB_RECURSE APP_HEADERS_CPP CONFIGURE_DEPENDS "../src/*.hpp")

# Convert the list of headers to a list of include directories
set (TEST_SHARED_INCLUDE_DIRS "")
foreach(HEADER ${TEST_HEADERS_SHARED})
  get_filename_component(DIR ${HEADER} DIRECTORY)
  list(APPEND TEST_SHARED_INCLUDE_DIRS ${DIR})
endforeach()

set (APP_INCLUDE_DIRS_C "")
foreach(HEADER ${APP_HEADERS_C})
  get_filename_component(DIR ${HEADER} DIRECTORY)
  list(APPEND APP_INCLUDE_DIRS_C ${DIR})
endforeach()

set (APP_INCLUDE_DIRS_CPP "")
foreach(HEADER ${APP_HEADERS_CPP})
  get_filename_component(DIR ${HEADER} DIRECTORY)
  list(APPEND APP_INCLUDE_DIRS_CPP ${DIR})
endforeach()

list(REMOVE_DUPLICATES TEST_SHARED_INCLUDE_DIRS)
list(REMOVE_DUPLICATES APP_INCLUDE_DIRS_C)
list(REMOVE_DUPLICATES APP_INCLUDE_DIRS_CPP)

# Remove the main file from the test sources
list(FILTER APP_SOURCES_C EXCLUDE REGEX "main.c")

# Prepare the test executables
add_executable(
  c_tests
  ${TEST_SOURCES_C}
  ${TEST_SOURCES_SHARED}
  ${APP_SOURCES_C}
)
target_include_directories(
  c_tests
  PRIVATE
  ${TEST_SHARED_INCLUDE_DIRS}
  ${APP_INCLUDE_DIRS_C}
)
target_link_libraries(
  c_tests
  GTest::gtest_main
)
target_compile_definitions(
  c_tests
  PRIVATE
  IS_TESTING
  DEBUG
  LANG_C
)

add_executable(
  cpp_tests
  ${TEST_SOURCES_CPP}
  ${TEST_SOURCES_SHARED}
  ${APP_SOURCES_CPP}
)
target_include_directories(
  cpp_tests
  PRIVATE
  ${TEST_SHARED_INCLUDE_DIRS}
  ${APP_INCLUDE_DIRS_CPP}
)
target_link_libraries(
  cpp_tests
  GTest::gtest_main
  systemc
)
target_compile_definitions(
  cpp_tests
  PRIVATE
  IS_TESTING
  DEBUG
  LANG_CPP
)

include(GoogleTest)
gtest_discover_tests(c_tests)
gtest_discover_tests(cpp_tests)
