macro(run_conan)
  # Download automatically, you can also just copy the conan.cmake file
  if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")

    message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/0.18.1/conan.cmake"
                "${CMAKE_BINARY_DIR}/conan.cmake"
                TLS_VERIFY ON)
  endif()

  include(${CMAKE_BINARY_DIR}/conan.cmake)

  # See https://github.com/conan-io/cmake-conan/blob/release/0.16/README.md for settings
  conan_cmake_run(
    CONANFILE conanfile.txt # Conanfile with libraries
    BASIC_SETUP CMAKE_TARGETS # individual targets to link to
    BUILD missing)
endmacro()