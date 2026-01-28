cmake_minimum_required(VERSION 3.14)
project(libhelion LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE HELION_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_library(helion ${HELION_SOURCES})
target_include_directories(helion PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

option(HELION_BUILD_FUZZERS "Build libFuzzer harnesses" OFF)

if(HELION_BUILD_FUZZERS)
  if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(WARNING "libFuzzer targets are best supported with Clang.")
  endif()

  set(HELION_FUZZERS
    parser_fuzzer
    decoder_fuzzer
    state_machine_fuzzer
  )

  foreach(fuzz_target IN LISTS HELION_FUZZERS)
    add_executable(${fuzz_target} "fuzz/${fuzz_target}.cc")
    target_link_libraries(${fuzz_target} PRIVATE helion)
    target_compile_options(${fuzz_target} PRIVATE -fsanitize=fuzzer-no-link,address)
    target_link_options(${fuzz_target} PRIVATE -fsanitize=fuzzer,address)
  endforeach()
endif()
