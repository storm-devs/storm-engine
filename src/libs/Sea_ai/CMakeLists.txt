project(sea_ai)

add_library(${PROJECT_NAME} STATIC
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
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)