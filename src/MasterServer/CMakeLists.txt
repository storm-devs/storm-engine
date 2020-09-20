cmake_minimum_required(VERSION 3.18)

project(masterserver)

add_library(masterserver MODULE 
	Account.cpp
	Account.h
	Client.cpp
	Client.h
	Common.h
	MasterServer.cpp
	MasterServer.h
	Server.cpp
	Server.h
)
target_include_directories(masterserver PRIVATE common_h)
target_include_directories(masterserver PRIVATE SoundService)
target_include_directories(masterserver PRIVATE extern/directx/include)
