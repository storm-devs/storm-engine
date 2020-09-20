cmake_minimum_required(VERSION 3.18)

project(seaai)

add_library(seaai MODULE 
	AIBalls.cpp
	AIBalls.h
	AICannon.cpp
	AICannon.h
	AIFlowGraph.h
	AIFort.cpp
	AIFort.h
	AIGroup.cpp
	AIGroup.h
	AIHelper.cpp
	AIHelper.h
	AISeaGoods.cpp
	AISeaGoods.h
	AIShip.cpp
	AIShip.h
	AIShipBoat.cpp
	AIShipCameraController.cpp
	AIShipCameraController.h
	AIShipCannonController.cpp
	AIShipCannonController.h
	AIShipMoveController.cpp
	AIShipMoveController.h
	AIShipRotateController.cpp
	AIShipRotateController.h
	AIShipSpeedController.cpp
	AIShipSpeedController.h
	AIShipTaskController.cpp
	AIShipTaskController.h
	AIShipTouchController.cpp
	AIShipTouchController.h
	AIShipTrade.cpp
	AIShipWar.cpp
	AITemplate.cpp
	AITemplate.h
	LocatorShow.cpp
	LocatorShow.h
	SEA_AI.cpp
	SEA_AI.h
	SeaSaveLoad.cpp

	templates/Array.h
	templates/HTable.h
	templates/dtimer.h
	templates/map.h
	templates/stack.h
	templates/string.h
	templates/tcommon.h
)
target_include_directories(seaai PRIVATE common_h)
target_include_directories(seaai PRIVATE SoundService)
target_include_directories(seaai PRIVATE extern/directx/include)
