cmake_minimum_required(VERSION 3.0)

project(GaoBingFa)

include_directories(common)

set(SRC main.cc server.cc user.cc message.cc common/config.cc common/mutex.cc common/sem.cc)

add_executable(${PROJECT_NAME} ${SRC})

target_link_libraries(${PROJECT_NAME} -lpthread)

# add_definitions(-D _D)
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/)


add_compile_options("-fsanitize=address-fno-omit-frame-pointer" "-O0" -Wall -Wextra -g)

link_libraries("-fsanitize=address")
