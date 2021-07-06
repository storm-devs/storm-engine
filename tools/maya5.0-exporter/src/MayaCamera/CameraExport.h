/******************************************************************************
File:  animationExport.h

Author:   Nick Chirkov
Copyright (C) Akella corp.

Comments:
Maya interface for animation export
******************************************************************************/
#pragma once

#pragma warning ( disable : 4786)

#include <maya/MPxFileTranslator.h>

class CameraExport : public MPxFileTranslator
{
public:
  CameraExport();
  ~CameraExport();

  MStatus writer(const MFileObject &file,	const MString &optionsString,	FileAccessMode mode); 
  bool haveWriteMethod() const;
  MString defaultExtension() const;
  MFileKind	identifyFile (const MFileObject &, const char *buffer, short size) const;
  static void *creator();
protected:
	void ExportAll(const char *fname);

};
