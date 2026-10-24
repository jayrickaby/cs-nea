# Widely supported version of CMake
cmake_minimum_required(VERSION 3.15)

project(CdRipper)

set(CMAKE_CXX_STANDARD 20)

# Needed to find libraries without direct support
find_package(PkgConfig REQUIRED)

# Needed to find libcdio
pkg_check_modules(LIBCDIO REQUIRED IMPORTED_TARGET libcdio)

# So our executable is called "CdRipper"
add_executable(CdRipper)

# Compile this code into the executable
target_sources(CdRipper
    PRIVATE
        src/main.cc
)

# Compile these libraries into the executable
target_link_libraries(CdRipper
    PRIVATE
        PkgConfig::LIBCDIO
)
