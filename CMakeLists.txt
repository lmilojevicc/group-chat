cmake_minimum_required(VERSION 3.10)
project(group-chat )

include_directories(utils)

add_executable(client client.c utils/socket.c)
add_executable(server server.c utils/socket.c)

