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
	LocatorShow.cpp
	LocatorShow.h
	SEA_AI.cpp
	SEA_AI.h
	SeaSaveLoad.cpp
)

target_include_directories(seaai PRIVATE ${DirectX_INCLUDE_DIR})