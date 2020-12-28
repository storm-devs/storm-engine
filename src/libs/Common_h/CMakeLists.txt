project(common_h)

find_package(DirectX REQUIRED)

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(${PROJECT_NAME} INTERFACE .)
target_include_directories(${PROJECT_NAME} INTERFACE ${DirectX_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} INTERFACE ${DirectX_D3DX9_LIBRARY})

set(PROJECT_SOURCES
	math3d/Box.h
	math3d/Capsule.h
	math3d/Color.h
	math3d/Line.h
	math3d/Plane.h
	math3d/Quaternion.h
	math3d/Sphere.h
	math3d/Triangle.h

	particles/gmx_QSort.h
	particles/iparticlemanager.h
	particles/iparticlesservice.h
	particles/iparticlesystem.h

	Sd2_h/AIMessages.h
	Sd2_h/CannonTrace.h
	Sd2_h/SaveLoad.h
	Sd2_h/VAI_ObjBase.h

	templates/Array.h
	templates/HTable.h
	templates/dsmooth.h
	templates/dtimer.h
	templates/map.h
	templates/stack.h
	templates/string.h
	templates/tcommon.h

	Animation.h
	CVector4.h
	Character.h
	Cvector.h
	Entity_x.h
	Exs.h
	Intel.h
	Island_Base.h
	Matrix.h
	IShipLights.h
	VideoTexture.h
	Weather_Base.h
	achievements.h
	attributes.h
	collide.h
	controls.h
	d_types.h
	common_defines.h
	dx8render.h
	entity.h
	entity_id.h
	entity_state.h
	geometry.h
	geos.h
	inlines.h
	input.h
	key_codes.h
	light.h
	mast_msg.h
	math3D.h
	memop.h
	message.h
	model.h
	object.h
	particles.h
	rands.cpp
	rands.h
	s_import_func.h
	sail_base.h
	sail_msg.h
	script_libriary.h
	sea_base.h
	service.h
	ship_base.h
	ship_msg.h
	stringService.h
	templates.h
	tga.h
	types3d.h
	utf8.h
	v_s_stack.h
	v_udp_transport.h
	vapi.h
	vblast.h
	vcontrols.h
	vcube_service.h
	vdata.h
	vevent.h
	vfile_service.h
	vidwalker.h
	vmapi.h
	vmemory_service.h
	vmodule_api.h
	vparticle_system.h
	vparticles_system.h
	vsail.h
	vship.h
	vstring_codec.h
	vsystem_api.h

	#copied from scripts
	messages.h
	Script_defines.h
)

add_custom_target(${PROJECT_NAME}_ SOURCES ${PROJECT_SOURCES})