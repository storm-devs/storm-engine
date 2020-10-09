cmake_minimum_required(VERSION 3.18)

project(worldmap)

add_library(worldmap MODULE 
	PtcData.cpp
	PtcData.h
	WdmCamera.cpp
	WdmCamera.h
	WdmCameraStdCtrl.cpp
	WdmCameraStdCtrl.h
	WdmCloud.cpp
	WdmCloud.h
	WdmClouds.cpp
	WdmClouds.h
	WdmCounter.cpp
	WdmCounter.h
	WdmDateLabel.cpp
	WdmDateLabel.h
	WdmEnemyShip.cpp
	WdmEnemyShip.h
	WdmEventWindow.cpp
	WdmEventWindow.h
	WdmFollowShip.cpp
	WdmFollowShip.h
	WdmIcon.cpp
	WdmIcon.h
	WdmInterfaceObject.cpp
	WdmInterfaceObject.h
	WdmIslands.cpp
	WdmIslands.h
	WdmMerchantShip.cpp
	WdmMerchantShip.h
	WdmObjects.cpp
	WdmObjects.h
	WdmObjectsWind.h
	WdmPlayerShip.cpp
	WdmPlayerShip.h
	WdmRenderModel.cpp
	WdmRenderModel.h
	WdmRenderObject.cpp
	WdmRenderObject.h
	WdmSea.cpp
	WdmSea.h
	WdmShip.cpp
	WdmShip.h
	WdmStorm.cpp
	WdmStorm.h
	WdmStormCloud.cpp
	WdmStormCloud.h
	WdmWarringShip.cpp
	WdmWarringShip.h
	WdmWindRose.cpp
	WdmWindRose.h
	WdmWindUI.cpp
	WdmWindUI.h
	WoldMapMain.cpp
	WorldMap.cpp
	WorldMap.h
	ptc.h
)

target_include_directories(worldmap PRIVATE ${DirectX_INCLUDE_DIR})