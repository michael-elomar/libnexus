cmake_minimum_required(VERSION 3.5)
project(nexus LANGUAGES C CXX)

# dependencies
find_package(neutron REQUIRED)
find_package(Protobuf REQUIRED)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC_FILES
	src/node.c
	src/node.cpp
	src/message.c
	src/message.cpp
)

set(INCLUDE
	include
	src
)

add_subdirectory(protobuf)
get_property(PROTO_CC_HEADERS GLOBAL PROPERTY PROTO_CC_HEADERS)
list(APPEND INCLUDE ${PROTO_CC_HEADERS})

add_library(${PROJECT_NAME} SHARED ${SRC_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${INCLUDE}>)
target_link_libraries(${PROJECT_NAME}
	protobuf-c
	protobuf::libprotobuf
	${PROJECT_NAME}-pb-c
	${PROJECT_NAME}-pb-cc
)

add_subdirectory(tools)

#======== Build examples ========#
if(${BUILD_EXAMPLES} STREQUAL "ON")
	add_subdirectory(examples)
else()
	message(STATUS "Build examples - skip")
endif()

#======== Build tests ========#
if(${BUILD_TESTS} STREQUAL "ON")
	add_subdirectory(tests)
else()
	message(STATUS "Build tests - skip")
endif()

#======== Install Support ========#

install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include)

# Install the headers
install(DIRECTORY include/ DESTINATION include)

#======== Package Support ========#

# Create a package configuration file
include(CMakePackageConfigHelpers)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION lib/cmake/${PROJECT_NAME}
)

# Install the package configuration files
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    DESTINATION lib/cmake/${PROJECT_NAME}
)

# Export targets for use by other projects
install(EXPORT ${PROJECT_NAME}Targets
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION lib/cmake/${PROJECT_NAME}
)
