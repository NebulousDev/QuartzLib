cmake_minimum_required(VERSION 3.20.0)

project(QuartzLib)

include(GNUInstallDirs)

add_library(${PROJECT_NAME} INTERFACE 
	"Include/QuartzLib.h"
	"Include/Types/Types.h"
	"Include/Types/Array.h"
	"Include/Types/Table.h"
	"Include/Types/Map.h"
	"Include/Types/Set.h"
	"Include/Types/Stack.h"
	"Include/Types/String.h"
	"Include/Types/List.h"
	"Include/Utility/Iterator.h"
	"Include/Utility/InitializerList.h"
	"Include/Utility/Hash.h"
	"Include/Utility/Move.h"
	"Include/Utility/Swap.h"
	"Include/Memory/Memory.h"
)

target_include_directories(${PROJECT_NAME} INTERFACE "Include")

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)

install(DIRECTORY "Include/" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES "${PROJECT_NAME}-config.cmake" DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/quartzlib/")