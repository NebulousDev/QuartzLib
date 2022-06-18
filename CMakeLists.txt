cmake_minimum_required(VERSION 3.20.0)

option(QUARTZLIB_GENERATE_CONFIGS "Enable generation of QuartzLibConfig.cmake" ON)

set(QUARTZLIB_INCLUDE_PREFIX "Quartz" CACHE STRING "Include prefix for installed headers")

project(QuartzLib VERSION 1.0.6)

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

    # Header-only cmake configs:
    # https://dominikberner.ch/cmake-interface-lib/
    # https://stackoverflow.com/questions/47718485/install-and-export-interface-only-library-cmake

    write_basic_package_version_file(
        "${CMAKE_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
    )

    install(
        TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}Targets
        LIBRARY DESTINATION lib COMPONENT Runtime
        ARCHIVE DESTINATION lib COMPONENT Development
        RUNTIME DESTINATION bin COMPONENT Runtime
        PUBLIC_HEADER DESTINATION include COMPONENT Development
        BUNDLE DESTINATION bin COMPONENT Runtime
    )

    configure_package_config_file(
        "${PROJECT_SOURCE_DIR}/CMake/Config.cmake.in"
        "${CMAKE_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        INSTALL_DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}"
    )

    install(
        EXPORT ${PROJECT_NAME}Targets 
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}
    )

    install(
        FILES 
            "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
            "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}
    )

    install(
        DIRECTORY "${PROJECT_SOURCE_DIR}/Include/" 
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${QUARTZLIB_INCLUDE_PREFIX}
    )

    message("[${PROJECT_NAME}] Generating ${PROJECT_NAME}Config.cmake...")

endif()