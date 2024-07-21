cmake_minimum_required(VERSION 3.22)
project(rendering_engine C)

set(CMAKE_C_STANDARD 99)

file(GLOB SRC_FILES src/*.c)
add_executable(${PROJECT_NAME} ${SRC_FILES})

install(TARGETS ${PROJECT_NAME} DESTINATION bin)

find_package(SDL2 REQUIRED)
target_link_libraries(${PROJECT_NAME} SDL2::SDL2 m)

add_custom_target(build
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target ${PROJECT_NAME}
    COMMENT "Building the renderer"
)

# Add a custom target for debugging
add_custom_target(gdb
    COMMAND ${CMAKE_COMMAND} --build -d ${CMAKE_BINARY_DIR} --target ${PROJECT_NAME}
    COMMAND gdb ./${PROJECT_NAME}
    COMMENT "Building and debugging the renderer"
)

# Add a custom target for running the executable
add_custom_target(run
    COMMAND ./${PROJECT_NAME}
    COMMENT "Running the renderer"
)

