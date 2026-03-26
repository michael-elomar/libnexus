add_executable(${PROJECT_NAME}-cli nexus-cli.c)
target_include_directories(${PROJECT_NAME}-cli PUBLIC $<BUILD_INTERFACE:${INCLUDE}>)
target_link_libraries(${PROJECT_NAME}-cli ${PROJECT_NAME} neutron)

add_executable(${PROJECT_NAME}-cpp-cli nexus-cli.cpp)
target_include_directories(${PROJECT_NAME}-cpp-cli PUBLIC $<BUILD_INTERFACE:${INCLUDE}>)
target_link_libraries(${PROJECT_NAME}-cpp-cli ${PROJECT_NAME} neutron)
