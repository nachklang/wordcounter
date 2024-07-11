cmake_minimum_required(VERSION 3.5)

project(word_counter LANGUAGES CXX)

option(USE_TIDY "Turn on clang-tidy" OFF)
option(SANITIZE "Turn on address sanitizer" OFF)
option(BUILD_TESTS "Build tests (Unit on GCC / Unit + Fuzzing on clang)" OFF)

set(CLANG_BUILD false)
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CLANG_BUILD true)
endif()

if (CLANG_BUILD AND BUILD_TESTS)
    set(FUZZTEST_FUZZING_MODE ON)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(USE_TIDY)
    find_program(CLANG_TIDY NAMES clang-tidy)
    if(CLANG_TIDY)
        message(INFO "clang-tidy has been detected: ${CLANG_TIDY}. Static analysis is available.")
        set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY}")
        set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
        set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES 
        ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
    else()
        message(WARNING "Not able to detect clang-tidy. Static analysis is not available.")
    endif()
endif()

if(SANITIZE)
    if (NOT DEFINED SANITIZERS)
        set(SANITIZERS "address,undefined,leak")
    endif()
    message("Sanitizer is ON: ${SANITIZERS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=${SANITIZERS} -fno-omit-frame-pointer")
    set(CMAKE_SHARED_LINKER_FLAGS "-fsanitize=${SANITIZERS}")
    set(CMAKE_EXE_LINKER_FLAGS "-fsanitize=${SANITIZERS}")
endif()

add_executable(word_counter main.cxx)
install(TARGETS word_counter
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(BUILD_TESTS)
    include(FetchContent)
    if (CLANG_BUILD)
        FetchContent_Declare(
        fuzztest
        URL https://github.com/google/fuzztest/archive/32eb84a95951fa3a0148fb3e6a1a02f830ded136.zip
        )
        FetchContent_MakeAvailable(fuzztest)
    else ()
        FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/33af80a883ddc33d9c0fac0a5b4578301efb18de.zip
        )
        FetchContent_MakeAvailable(googletest)
    endif()
    
    enable_testing()

    add_executable(
    unit_tests
    unit_tests.cxx
    )
    target_link_libraries(
    unit_tests
    GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(unit_tests)

    if (CLANG_BUILD)
        fuzztest_setup_fuzzing_flags()
        add_executable(
        fuzz_tests
        fuzz_tests.cxx
        )
        link_fuzztest(fuzz_tests)
        gtest_discover_tests(fuzz_tests)
    endif()
endif()