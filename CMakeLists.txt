cmake_minimum_required(VERSION 3.0)
project(semestralka)

set(CMAKE_CXX_STANDARD 17)

#Create an executable for the server
add_executable(server server.cpp DatabaseLogic.cpp)  # Assuming database.cpp is used by the server

#Create an executable for the client
add_executable(client client.cpp)  # Assuming the client does not use database.cpp

#Find pthread package
find_package(Threads REQUIRED)

#Link pthread to both your targets
target_link_libraries(server PRIVATE Threads::Threads)
target_link_libraries(client PRIVATE Threads::Threads)