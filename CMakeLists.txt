cmake_minimum_required(VERSION 3.5)
project(nexus LANGUAGES C CXX)

# dependencies
find_package(neutron REQUIRED)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC_FILES
	src/publisher.c
	src/subscriber.c
)

set(INCLUDE
	include
	src
)

add_subdirectory(protobuf)
get_property(PROTO_CC_HEADERS GLOBAL PROPERTY PROTO_CC_HEADERS)

list(APPEND INCLUDE ${PROTO_CC_HEADERS})

message(STATUS "include paths: ${INCLUDE}")

add_library(${PROJECT_NAME} SHARED ${SRC_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${INCLUDE}>)
target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}-pb-cc)

#======== Build examples ========#
if(${BUILD_EXAMPLES} STREQUAL "ON")
	add_subdirectory(examples)
else()
	message(STATUS "Build examples - skip")
endif()


#======== Install Support ========#

set(INSTALL_TARGETS
	${PROJECT_NAME}
	${PROJECT_NAME}-pb-c
	${PROJECT_NAME}-pb-cc
)

install(TARGETS ${INSTALL_TARGETS}
    EXPORT ${PROJECT_NAME}Targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include)

# Install the headers
install(DIRECTORY include/ DESTINATION include)
