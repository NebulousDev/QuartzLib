cmake_minimum_required(VERSION 3.18.0)

project(QuartzLib)

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

target_include_directories(${PROJECT_NAME} INTERFACE ./Include)