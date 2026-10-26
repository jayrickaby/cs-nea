# Widely supported version of CMake
cmake_minimum_required(VERSION 3.15)

project(CdRipper)

set(CMAKE_CXX_STANDARD 20)

# Needed to find libraries without direct support
find_package(PkgConfig REQUIRED)

# Needed to find libcdio
pkg_check_modules(LIBCDIO REQUIRED IMPORTED_TARGET libcdio)


find_package(Qt6 REQUIRED
    COMPONENTS
        Core
)

qt_standard_project_setup(REQUIRES 6.12.0)

# So our executable is called "CdRipper"
qt_add_executable(CdRipper)

# Compile this code into the executable
target_sources(CdRipper
    PRIVATE
        src/main.cc
)

# Compile these libraries into the executable
target_link_libraries(CdRipper
    PRIVATE
        PkgConfig::LIBCDIO
        Qt6::Core
)
