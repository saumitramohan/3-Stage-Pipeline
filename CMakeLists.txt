cmake_minimum_required(VERSION 3.8)
project(cs250p_mips_simulator_master)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        mips.dSYM/Contents/Resources/DWARF/mips
        mips.dSYM/Contents/Info.plist
        assembly.txt
        assembly1.txt
        info.txt
        main.cpp
        Makefile
        mips
        solution.cpp
        solution_pipeline.cpp)

add_executable(cs250p_mips_simulator_master ${SOURCE_FILES})