/******************************************************************************
File:  animationExport.h

Author:   Nick Chirkov
Copyright (C) Akella corp.

Comments:
Maya interface for animation export
******************************************************************************/
#ifndef _AniExport_H_
#define _AniExport_H_

#pragma warning ( disable : 4786)

#include <maya/MPxFileTranslator.h>

class AniExport : public MPxFileTranslator
{
public:
  AniExport();
  ~AniExport();

  MStatus writer(const MFileObject &file,	const MString &optionsString,	FileAccessMode mode); 
  bool haveWriteMethod() const;
  MString defaultExtension() const;
  MFileKind	identifyFile (const MFileObject &, const char *buffer, short size) const;
  static void *creator();
protected:
	void ExportAll(const char *fname);

};

#endif