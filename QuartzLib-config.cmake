cmake_minimum_required(VERSION 3.20.0)

if(TARGET QuartzLib::QuartzLib)
	return()
endif()

add_library(QuartzLib::QuartzLib INTERFACE IMPORTED)

target_compile_features(QuartzLib::QuartzLib INTERFACE cxx_std_17)

target_include_directories(QuartzLib::QuartzLib INTERFACE "Include")
