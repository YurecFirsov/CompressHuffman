cmake_minimum_required(VERSION 3.15)
project(CompressAlgHuffman)

set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -g -S -fverbose-asm")

add_executable(CompressAlgHuffman main.cpp Huffman.cpp)
