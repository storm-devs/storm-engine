#pragma once

#include <maya/MGlobal.h>
#include <maya/MPxFileTranslator.h>
#include <maya/MSelectionList.h>
#include <maya/MItSelectionList.h>
#include <maya/MDagPath.h>
#include <maya/MItDag.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnMesh.h>
#include <maya/MItMeshVertex.h>

#include <stdio.h>
#include <vector>

#include "../Export/rdf_exp.h"
#include "../Export/export.h"

struct MATERIAL
{
	long ntex;
	MObject* MO;
	char matname[256];
	char texname[4][256];
	float offset[4][2], repeat[4][2];
	double rotate[4];

	EXPORT_STATIC::BRDF_TYPE brdf;
	float diffuse, specular, gloss, selfIllum, transparency, refraction_id, reflection;
};

class AtlaExport : public MPxFileTranslator
{
public:
	AtlaExport();
	~AtlaExport();

	MStatus writer(const MFileObject& file, const MString& optionsString, FileAccessMode mode) override;
	bool haveWriteMethod() const override;
	MString defaultExtension() const override;
	MFileKind identifyFile(const MFileObject&, const char* buffer, short size) const override;
	static void* creator();
private:
	void ExportAll(const std::string& file_name, bool bsp);
	void Traverse(MDagPath& path);
	void ExportLocator(MDagPath& cl);
	size_t CountObjects(const MDagPath& path);
	
	std::vector<std::string> locators_;
	bool GlobAnimation;
	long nummat;
	MATERIAL material[1024];
	long matref[1024];

	long curobject, total_objects;

	bool is_scaled_;
};
