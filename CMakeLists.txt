cmake_minimum_required(VERSION 3.0)
project(semestralka)

set(CMAKE_CXX_STANDARD 17)

# Create an executable for the server
add_executable(server server.cpp database.cpp)

# Create an executable for the client
add_executable(client client.cpp)

# Find pthread package
find_package(Threads REQUIRED)

# Linking pthread to both targets
target_link_libraries(server PRIVATE Threads::Threads)
target_link_libraries(client PRIVATE Threads::Threads)
