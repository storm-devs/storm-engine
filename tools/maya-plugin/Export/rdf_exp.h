/******************************************************************************
File:  rdf_exp.h

Author:   Nick Chirkov
Copyright (C) 2000-2002 Nick Chirkov

Comments:
******************************************************************************/
#ifndef _RDFLIB_I_H_
#define _RDFLIB_I_H_

#include <windows.h>
#include <string>
#include <vector>

//texture callback
typedef void (*TEXTURE_FUNC)(long id);
//result callback
typedef bool (*RESULT_FUNC)(const std::string &s);

class EXPORT_STATIC
{
public:

	//--------------------------------------------------------------
	//texture
	//--------------------------------------------------------------
	enum TEXTURE_TYPE
	{
		NONE = 0,
		BASE,
		NORMAL,
		LIGHTMAP,
		TEXTURE_FORCEDWORD = 0x7FFFFFFF
	};
	struct TEXTURE
	{
		std::string name;
		TEXTURE_TYPE type;
		long id;	//will be returned
	};

	//--------------------------------------------------------------
	//material
	//--------------------------------------------------------------
	enum BRDF_TYPE
	{
		PHONG = 0,
		BLINN,
		METALL,
		BRDF_FORCEDWORD = 0x7FFFFFFF
	};
	struct MATERIAL
	{
		std::string group_name;
		std::string name;
		BRDF_TYPE brdf;
		float diffuse;	//0 - no diffuse material
		float specular, gloss;	//spec=0 - no specular, gloss is a power of cosine
		float selfIllum;	//for area light sources
		float transparency, refraction_id;
		float reflection;	//0 for non-mirrors
		TEXTURE texture[4];
	};

	//--------------------------------------------------------------
	//vertex
	//--------------------------------------------------------------
	struct VERTEX
	{
		float x,y,z;
		float nx,ny,nz;
		float tu[4], tv[4];
		//skinned animation
		float weight[4];
		long boneid[4];
		long color;
	};

	//--------------------------------------------------------------
	//triangle
	//--------------------------------------------------------------
	struct TRIANGLE
	{
		long material;
		VERTEX vrt[3];
	};

	//--------------------------------------------------------------
	//object
	//--------------------------------------------------------------
	enum OBJECT_FLAGS
	{
		VISIBLE = (1<<0),
		STATIC_LIGHT_ENABLE = (1<<1),	//if no - object will not be lited
		DINAMIC_LIGHT_ENABLE = (1<<2),	//if no - no dynamic lighting perfomed
		CAST_SHADOWS_ENABLE = (1<<3),	//if material makes shadows
		COLLISION_ENABLE = (1<<4),	//if yes - object will be stored to BSP

		//geometry access flags
		GEO_VERTEX = (1<<5),	//vertices will be stored to geoaccess
		GEO_VERTEX_TRIANGLE = (1<<6),	//triangles and vertices will be stored to geoaccess
		GEO_EDGE = (1<<7),	//edges and vertices will be stored to geoaccess
		GEO_EDGE_TRIANGLE = (1<<8),	//triangles, edges and vertices will be stored to geoaccess

		VERTEX_WEIGHT = (1<<9),	//animation
		MERGE = (1<<10),	//object is "merged"-type
		CULLENABLE = (1<<11),	//single-sided object
		ZSORTED = (1<<12),	//insert triangles into BSP for z-sorting during render
		OBJECT_FORCEDWORD = 0x7FFFFFFF

	};
	struct OBJECT
	{
		long flags;	//combination of OBJECT_FLAGS
		std::string group_name;
		std::string name;

		std::vector<MATERIAL> mtl;
		std::vector<TRIANGLE> trg;
		float disp;
	};
	//add a triangle by the vertex's index to the current sub-object
	virtual void AddObject(const OBJECT &obj) = 0;

	//--------------------------------------------------------------
	//light source
	//--------------------------------------------------------------
	enum LIGHT_TYPE
	{
		POINT = 0,
		SPOT,
		DIRECTIONAL,
		LIGHT_FORCEDWORD = 0x7FFFFFFF
	};
	enum LIGHT_FLAGS
	{
		SHADOW = 1,
		LIGHT_FORCED_WORD = 0x7FFFFFFF
	};
	struct LIGHT
	{
		std::string group_name;
		std::string name;
		long flags;
		LIGHT_TYPE type;
		float r,g,b;
		float range;
		float x,y,z;
		float atten[3];
		float nx,ny,nz;
		float inner, outer, falloff;
	};
	//add a light sources
	virtual void AddLight(const LIGHT &l) = 0;

	//--------------------------------------------------------------
	//label
	//--------------------------------------------------------------
	enum LABEL_FLAGS
	{
		LABEL_FORCEDWORD = 0x7FFFFFFF
	};
	struct LABEL
	{
		std::string group_name;
		std::string name;

		long flags;	//combination of LABEL_FLAGS
		float vector[4][4];
		long bones[4];
		float weight[4];
	};
	//add a label
	virtual void AddLabel(const LABEL &l) = 0;

	//--------------------------------------------------------------
	//
	//--------------------------------------------------------------
	virtual ~EXPORT_STATIC(){};

	//write file function
	virtual void WriteFile() = 0;

};

enum EXP_FLAGS
{
	BUILD_BSP=1,
	PRODUCTION=2,
	ANIMATION=4,
	GEOACCESS=8,
	EXP_FORCE_DWORD = 0x7FFFFFFF
};

typedef EXPORT_STATIC * (__cdecl *CREATE_EXPORT_FUNC)(const std::string &fname,
																		 TEXTURE_FUNC Textures, 
																		 RESULT_FUNC statistic, RESULT_FUNC errors,
																		 float maxedge, float merge_size, long flags);

#endif