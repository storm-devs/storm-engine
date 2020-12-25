project(common_h)

find_package(DirectX REQUIRED)
find_package(Spdlog REQUIRED)

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(${PROJECT_NAME} INTERFACE .)

target_include_directories(${PROJECT_NAME}
	INTERFACE
	${DirectX_INCLUDE_DIR}
	${Spdlog_INCLUDE_DIR}
	${STORM_SOURCE_DIR}/shared
)

target_link_libraries(${PROJECT_NAME} 
	INTERFACE 
	${DirectX_D3DX9_LIBRARY}
	${Spdlog_LIBRARY}
)

set(PROJECT_SOURCES
	math3d/Color.h
	math3d/Matrix.h
	math3d/Plane.h
	math3d/Quaternion.h
	math3d/Sphere.h
	math3d/Triangle.h
	math3d/Vector.h
	math3d/Vector4.h

	particles/gmx_QSort.h
	particles/iparticlemanager.h
	particles/iparticlesservice.h
	particles/iparticlesystem.h

	Sd2_h/CannonTrace.h
	Sd2_h/SaveLoad.h
	Sd2_h/VAI_ObjBase.h

	achievements.h
	Animation.h
	attributes.h
	Character.h
	collide.h
	controls.h
	Cvector.h
	CVector4.h
	defines.h
	dtimer.h
	dx9render.h
	Entity.h
	entity_state.h
	EntityManager.h
	filesystem.h
	geometry.h
	geos.h
	inlines.h
	Intel.h	
	Island_Base.h
	math3D.h	
	Matrix.h
	message.h
	model.h
	object.h
	particles.h
	PtcData.h
	rands.h
	s_import_func.h
	sail_base.h
	script_libriary.h
	sea_base.h
	service.h
	ship_base.h
	ship_msg.h	
	ShipLights.h
	storm_assert.h
	stringService.h
	strutils.h
	tga.h
	IVBufferManager.h
	triangle.h
	types3d.h
	utf8.h
	v_s_stack.h
	vapi.h	
	vdata.h
	vfile_service.h
	VideoTexture.h
	vmodule_api.h
	vparticle_system.h
	vstring_codec.h
	Weather_Base.h
)

add_custom_target(${PROJECT_NAME}_ SOURCES ${PROJECT_SOURCES})