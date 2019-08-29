cmake_minimum_required(VERSION 3.10)
project(mdx-optimizer CXX)

add_executable(mdx-optimizer
	src/main.cpp
	src/common.cpp
	src/m3.cpp
	src/mdx.cpp
)

target_include_directories(mdx-optimizer PRIVATE "include")