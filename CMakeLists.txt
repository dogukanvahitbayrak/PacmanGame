cmake_minimum_required(VERSION 3.16)
project(raylib-deneme C)

set(CMAKE_C_STANDARD 99)

# Raylib Homebrew ile kurulduğu için bu yolu kullanıyoruz
set(RAYLIB_PATH "/opt/homebrew/opt/raylib")

include_directories(${RAYLIB_PATH}/include)
link_directories(${RAYLIB_PATH}/lib)

add_executable(raylib-deneme src/main.c)
target_link_libraries(raylib-deneme raylib)
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/levels DESTINATION ${CMAKE_BINARY_DIR})