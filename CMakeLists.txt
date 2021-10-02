cmake_minimum_required(VERSION 3.0)

project(GaoBingFa)

include_directories(common)

set(SRC main.cc server.cc common/config.cc user.cc)

add_executable(${PROJECT_NAME} ${SRC})

target_link_libraries(${PROJECT_NAME} -lpthread)

add_definitions(-D _D)
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/)