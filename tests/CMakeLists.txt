add_executable(test_message test_message.cpp)
target_include_directories(test_message PRIVATE $<BUILD_INTERFACE:${INCLUDE}>)
target_link_libraries(test_message ${PROJECT_NAME} neutron)

add_executable(test_node test_node.cpp)
target_include_directories(test_node PRIVATE $<BUILD_INTERFACE:${INCLUDE}>)
target_link_libraries(test_node ${PROJECT_NAME} neutron)

# Install tests

set(TEST_INSTALL_TARGETS
	test_message
	test_node
)

install(TARGETS ${TEST_INSTALL_TARGETS}
    EXPORT ${PROJECT_NAME}Targets
	RUNTIME DESTINATION bin)
