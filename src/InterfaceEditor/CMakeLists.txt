project(interfaceeditor)

add_library(${PROJECT_NAME} MODULE
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

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)