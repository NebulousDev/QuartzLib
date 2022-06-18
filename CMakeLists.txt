cmake_minimum_required(VERSION 3.20.0)

option(QUARTZLIB_GENERATE_CONFIGS "Enable generation of QuartzLibConfig.cmake" ON)

set(QUARTZLIB_INCLUDE_PREFIX "Quartz" CACHE STRING "Include prefix for installed headers")

project(QuartzLib 
    VERSION 1.0.7
    DESCRIPTION "A simple library of tools created for use with the Quartz Engine"
    HOMEPAGE_URL "https://github.com/NebulousDev/QuartzLib"
)

include(GNUInstallDirs)

add_library(${PROJECT_NAME} INTERFACE)

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)

target_include_directories(${PROJECT_NAME} 
	INTERFACE
		"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>"
		"$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${QUARTZLIB_INCLUDE_PREFIX}>"
)

# Generate QuartzLibConfig.cmake
if(QUARTZLIB_GENERATE_CONFIGS)

    include(CMakePackageConfigHelpers)

    # Header-only cmake configs: https://dominikberner.ch/cmake-interface-lib/

    write_basic_package_version_file(
        "${CMAKE_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
    )

    configure_package_config_file(
        "${PROJECT_SOURCE_DIR}/CMake/Config.cmake.in"
        "${CMAKE_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        INSTALL_DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}"
    )

    install(
        TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}Targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

    install(
        EXPORT ${PROJECT_NAME}Targets 
        FILE ${PROJECT_NAME}Targets.cmake
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}
        NAMESPACE Quartz::
    )

    install(
        FILES 
            "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
            "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}
    )

    install(
        DIRECTORY "${PROJECT_SOURCE_DIR}/Include/" 
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${QUARTZMATH_INCLUDE_PREFIX}
    )

    message("[${PROJECT_NAME}] Generating config files...")

endif()