add_executable(${PROJECT_NAME}-cli nexus-cli.c)
target_include_directories(${PROJECT_NAME}-cli PRIVATE $<BUILD_INTERFACE:${INCLUDE}>)
target_link_libraries(${PROJECT_NAME}-cli ${PROJECT_NAME} neutron)

add_executable(${PROJECT_NAME}-cpp-cli nexus-cli.cpp)
target_include_directories(${PROJECT_NAME}-cpp-cli PRIVATE $<BUILD_INTERFACE:${INCLUDE}>)
target_link_libraries(${PROJECT_NAME}-cpp-cli ${PROJECT_NAME} neutron)

set(TOOLS_INSTALL_TARGETS
	${PROJECT_NAME}-cli
	${PROJECT_NAME}-cpp-cli
)

install(TARGETS ${TOOLS_INSTALL_TARGETS}
    EXPORT ${PROJECT_NAME}Targets
	RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include)
