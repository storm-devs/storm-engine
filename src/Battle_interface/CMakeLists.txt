cmake_minimum_required(VERSION 3.18)

project(battleinterface)

add_library(battleinterface MODULE 
	EnemyStatShower/enemystatshower.cpp
	EnemyStatShower/enemystatshower.h

	image/defines.h
	image/image.cpp
	image/image.h
	image/imgrender.cpp
	image/imgrender.h
	image/material.cpp
	image/material.h
	image/string.cpp
	image/string.h

	interfacemanager/interfacegroup/BaseGroup.cpp
	interfacemanager/interfacegroup/BaseGroup.h
	interfacemanager/interfacegroup/SeaGroup.cpp
	interfacemanager/interfacegroup/SeaGroup.h
	interfacemanager/interfacenode/AllNodes.h
	interfacemanager/interfacenode/BaseNode.cpp
	interfacemanager/interfacenode/BaseNode.h
	interfacemanager/interfacenode/ImageNode.cpp
	interfacemanager/interfacenode/ImageNode.h
	interfacemanager/interfacenode/StringNode.cpp
	interfacemanager/interfacenode/StringNode.h
	interfacemanager/BaseManager.h
	interfacemanager/InterfaceManager.cpp
	interfacemanager/InterfaceManager.h
	interfacemanager/MousePointer.cpp
	interfacemanager/MousePointer.h

	ItemEntity/itementity.cpp
	ItemEntity/itementity.h

	land/battle_land.cpp
	land/battle_land.h
	land/battle_mancommand.cpp
	land/battle_mancommand.h
	land/battle_mansign.cpp
	land/battle_mansign.h
	land/IBoardingStatus.cpp
	land/IBoardingStatus.h

	sea/battle_navigator.cpp
	sea/battle_navigator.h
	sea/battle_shipcommand.cpp
	sea/battle_shipcommand.h
	sea/battle_shipsign.cpp
	sea/battle_shipsign.h
	sea/ibattle.cpp
	sea/ibattle.h
	sea/island_descr.cpp
	sea/island_descr.h
	sea/ShipInfoImages.cpp
	sea/ShipInfoImages.h
	sea/ShipPointer.cpp
	sea/ShipPointer.h
	sea/ships_list.cpp
	sea/ships_list.h
	
	seainterface/defines.h
	seainterface/seadatabase.cpp
	seainterface/seadatabase.h
	seainterface/shipcommand.cpp
	seainterface/shipcommand.h
	seainterface/shipliga.cpp
	seainterface/shipliga.h
	seainterface/shipsign.cpp
	seainterface/shipsign.h
	
	spyglass/spyglass.cpp
	spyglass/spyglass.h
	
	timer/Timer.cpp
	timer/Timer.h
	
	WorldMapInterface/interface.cpp
	WorldMapInterface/interface.h
	WorldMapInterface/shipcommand.cpp
	WorldMapInterface/shipcommand.h
	WorldMapInterface/shipsign.cpp
	WorldMapInterface/shipsign.h
	
	ActivePerkShower.cpp
	ActivePerkShower.h
	battle_command.cpp
	battle_command.h
	battle_land.cpp
	battle_land.h
	battle_navigator.cpp
	battle_navigator.h
	battle_shipcommand.cpp
	battle_shipcommand.h
	battle_shipsign.cpp
	battle_shipsign.h
	battle_sign.cpp
	battle_sign.h
	defines.h
	ibattle.cpp
	ibattle.h
	IBoardingStatus.cpp
	IBoardingStatus.h
	island_descr.cpp
	island_descr.h
	LogAndAction.cpp
	LogAndAction.h
	MessageIcons.cpp
	MessageIcons.h
	ShipPointer.cpp
	ShipPointer.h
	ships_list.cpp
	ships_list.h
	spyglass.cpp
	spyglass.h
	Utils.cpp
	Utils.h
)
target_include_directories(battleinterface PRIVATE common_h)
target_include_directories(battleinterface PRIVATE SoundService)
target_include_directories(battleinterface PRIVATE extern/directx/include)
