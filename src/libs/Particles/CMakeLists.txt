project(particles)

add_library(${PROJECT_NAME} STATIC
	DataCache/DataCache.cpp
	DataCache/DataCache.h

	GeomCache/GeomCache.cpp
	GeomCache/GeomCache.h

	ICommon/BillBoard.h
	ICommon/ColorVertex.h
	ICommon/EditorTypes.h
	ICommon/GraphTime.h
	ICommon/GraphVertex.h
	ICommon/IEmitter.h
	ICommon/MemFile.h
	ICommon/Names.h
	ICommon/Particle.h
	ICommon/Types.h

	K2Wrapper/particles.cpp
	K2Wrapper/particles.h
	K2Wrapper/psystem.cpp
	K2Wrapper/psystem.h

	Manager/particlemanager.cpp
	Manager/particlemanager.h

	Service/particleservice.cpp
	Service/particleservice.h

	System/DataDesc/data_desc.cpp
	System/DataDesc/data_desc.h

	System/DataSource/DataBool.cpp
	System/DataSource/DataBool.h
	System/DataSource/DataColor.cpp
	System/DataSource/DataColor.h
	System/DataSource/DataFloat.cpp
	System/DataSource/DataFloat.h
	System/DataSource/DataGraph.cpp
	System/DataSource/DataGraph.h
	System/DataSource/DataPosition.cpp
	System/DataSource/DataPosition.h
	System/DataSource/DataSource.cpp
	System/DataSource/DataSource.h
	System/DataSource/DataString.cpp
	System/DataSource/DataString.h
	System/DataSource/DataUV.cpp
	System/DataSource/DataUV.h
	System/DataSource/FieldList.cpp
	System/DataSource/FieldList.h

	System/Emitters/base.cpp
	System/Emitters/base.h
	System/Emitters/point.cpp
	System/Emitters/point.h

	System/ParticleProcessor/bb_processor.cpp
	System/ParticleProcessor/bb_processor.h
	System/ParticleProcessor/mdl_processor.cpp
	System/ParticleProcessor/mdl_processor.h
	System/ParticleProcessor/nameparser.h
	System/ParticleProcessor/physic.h

	System/ParticleSystem/particlesystem.cpp
	System/ParticleSystem/particlesystem.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)