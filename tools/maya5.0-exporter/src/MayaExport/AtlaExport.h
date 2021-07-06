/******************************************************************************
File:  AtlaExport.h

Author:   Nick Chirkov
Copyright (C) 2001 Nick Chirkov

Comments:
Maya interface for export
******************************************************************************/
#ifndef _ATLAEXPORT_H_
#define _ATLAEXPORT_H_

#pragma warning ( disable : 4786)


#include <maya/MGlobal.h>
#include <maya/MPxFileTranslator.h>
#include <maya/MSelectionList.h>
#include <maya/MItSelectionList.h>
#include <maya/MDagPath.h>
#include <maya/MItDag.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnMesh.h>
#include <maya/MItMeshVertex.h>

#include <windows.h>
#include <stdio.h>


class AtlaExport : public MPxFileTranslator
{
public:
  AtlaExport();
  ~AtlaExport();

  MStatus writer(const MFileObject &file,	const MString &optionsString,	FileAccessMode mode); 
  bool haveWriteMethod() const;
  MString defaultExtension() const;
  MFileKind	identifyFile (const MFileObject &, const char *buffer, short size) const;
  static void *creator();
protected:
	void ExportAll(const char *fname, bool scale, bool bsp, bool product);

};

#endif