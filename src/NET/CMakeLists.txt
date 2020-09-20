cmake_minimum_required(VERSION 3.18)

project(net)

add_library(net MODULE 
	CRC.h
	CRC16.cpp
	CRC16.h
	CRC32.cpp
	Client.cpp
	Client.h
	LocalClient.cpp
	LocalClient.h
	NetMessage.h
	Nurbs.h
	Server.cpp
	Server.h
	common.h
	net.cpp
	net.h

	Entities/BallSplash/BallSplash.h
	Entities/BallSplash/BallSplash.cpp
	Entities/BallSplash/BallSplashDefines.cpp
	Entities/BallSplash/BallSplashDefines.h
	Entities/BallSplash/TSplash.cpp
	Entities/BallSplash/TSplash.h

	Entities/Balls/NetBalls.cpp
	Entities/Balls/NetBalls.h

	Entities/Camera/Common_Camera.h
	Entities/Camera/DeckCamera.cpp
	Entities/Camera/DeckCamera.h
	Entities/Camera/ShipCamera.cpp
	Entities/Camera/ShipCamera.h

	Entities/Cannons/Cannon.cpp
	Entities/Cannons/Cannon.h

	Entities/Chat/Say.cpp
	Entities/Chat/Say.h

	Entities/Fort/Fort.cpp
	Entities/Fort/Fort.h

	Entities/Island/Foam.cpp
	Entities/Island/Foam.h
	Entities/Island/ISLAND.cpp
	Entities/Island/ISLAND.h
	Entities/Island/MapZipper.cpp
	Entities/Island/ZapSuperGenerator.cpp

	Entities/Rigging/Flag.cpp
	Entities/Rigging/Flag.h
	Entities/Rigging/Rope.cpp
	Entities/Rigging/Rope.h
	Entities/Rigging/SAILONE.cpp
	Entities/Rigging/SAILONE.h
	Entities/Rigging/Vant.cpp
	Entities/Rigging/Vant.h
	Entities/Rigging/mast.cpp
	Entities/Rigging/mast.h
	Entities/Rigging/rigging_define.h
	Entities/Rigging/sail.cpp
	Entities/Rigging/sail.h

	Entities/Sea/BumpMap.cpp
	Entities/Sea/CubeMap.cpp
	Entities/Sea/EnvMap.cpp
	Entities/Sea/SEA.cpp
	Entities/Sea/SEA.h
	Entities/Sea/ShipTrail.cpp
	Entities/Sea/ShipTrail.h
	Entities/Sea/sea_defines.h
	Entities/Sea/sea_struct.h

	Entities/Ship/CannonController.cpp
	Entities/Ship/CannonController.h
	Entities/Ship/FirePlace.cpp
	Entities/Ship/FirePlace.h
	Entities/Ship/ShipLights.cpp
	Entities/Ship/ShipLights.h
	Entities/Ship/Track.cpp
	Entities/Ship/Track.h
	Entities/Ship/ship.cpp
	Entities/Ship/ship.h
	Entities/Ship/ship_utils.cpp

	Entities/Touch/touch.cpp
	Entities/Touch/touch.h

	Entities/Weather/Lightning.cpp
	Entities/Weather/Lightning.h
	Entities/Weather/Rain.cpp
	Entities/Weather/Rain.h
	Entities/Weather/Sky.cpp
	Entities/Weather/Sky.h
	Entities/Weather/SunGlow.cpp
	Entities/Weather/SunGlow.h
	Entities/Weather/Weather.cpp
	Entities/Weather/Weather.h

	Entities/lightpillar/pillar.cpp
	Entities/lightpillar/pillar.h

	NetFile/NetFile.h
	NetFile/NetFileRecv.cpp
	NetFile/NetFileSend.cpp

	Script/NetFile.inl
	Script/NetInet.inl
	Script/NetMessage.inl
	Script/NetSocket.inl
)
target_include_directories(net PRIVATE common_h)
target_include_directories(net PRIVATE SoundService)
target_include_directories(net PRIVATE extern/directx/include)
