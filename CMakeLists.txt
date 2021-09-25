cmake_minimum_required(VERSION 3.0)

project(GaoBingFa)

include_directories(common)



add_executable(main_cmake main.cc server.cc common/common.cc common/config.cc)

# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/)