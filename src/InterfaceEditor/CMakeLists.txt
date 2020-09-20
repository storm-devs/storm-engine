cmake_minimum_required(VERSION 3.18)

project(interfaceeditor)

add_library(interfaceeditor MODULE 
	gui/utils.h
	gui/utils.cpp
	gui/node.h
	gui/node.cpp
	gui/mousepointer.h
	gui/mousepointer.cpp
	gui/defines.h
	gui/allnode.h

	InterfaceEditor.h
	InterfaceEditor.cpp
	EditorManager.h
	EditorManager.cpp
)
target_include_directories(interfaceeditor PRIVATE common_h)
target_include_directories(interfaceeditor PRIVATE SoundService)
target_include_directories(interfaceeditor PRIVATE extern/directx/include)
