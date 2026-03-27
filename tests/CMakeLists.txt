add_executable(test_message test_message.cpp)
target_include_directories(test_message PUBLIC $<BUILD_INTERFACE:${INCLUDE}>)
target_link_libraries(test_message ${PROJECT_NAME} neutron)
