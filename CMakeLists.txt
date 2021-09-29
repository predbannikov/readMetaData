cmake_minimum_required(VERSION 3.5)

project(readMetaData LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(readMetaData main.cpp InfoRAR5.cpp InfoRAR5.h InfoRAR4x.h InfoArch.h InfoRAR4x.cpp baserar.h keyboard.h keyboard.cpp)
