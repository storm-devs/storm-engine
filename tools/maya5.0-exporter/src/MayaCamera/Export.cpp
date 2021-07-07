/******************************************************************************
File:  AnimationExport.cpp

Author:   Nick Chirkov
Copyright (C) 2001 Akella corp.

Comments:
Maya export
******************************************************************************/
#include "CameraExport.h"
#include <maya/MGlobal.h>
#include <maya/MFnPlugin.h>
#include <maya/MTime.h>
#include <maya/MAnimControl.h>
#include <maya/MDagPath.h>
#include <maya/MSelectionList.h>
#include <maya/MItSelectionList.h>
#include <maya/MFnDagNode.h>
#include <maya/MMatrix.h>
#include <maya/MFnMesh.h>
#include <maya/MItDag.h>
#include <maya/MFnTransform.h>

FILE *LogFile=0;
void Log(const char *fmt,...)
{
	va_list args;
	va_start(args,fmt);
	vfprintf(LogFile, fmt, args);
	va_end(args);
}

bool GetDoubleValue(MObject &obj, const char *name, double &d)
{
	MFnDependencyNode NodeFnDn;
	NodeFnDn.setObject(obj);
	MStatus status;
	MPlug rpuv = NodeFnDn.findPlug(name,&status);
	if(!status)	return false;
	MObject val;
	rpuv.getValue(d);
	return true;
}

FILE *anifile;
MStatus status;

CameraExport::CameraExport()
{
}

CameraExport::~CameraExport()
{
}

MStatus CameraExport::writer(const MFileObject &file,  const MString &optionsString, FileAccessMode mode)
{
  MString  fname = file.fullName();
	ExportAll(fname.asChar());
  return (MS::kSuccess);
}
bool CameraExport::haveWriteMethod () const
{
  return (true);
}
MString CameraExport::defaultExtension () const
{
  return MString("cam");
}

MPxFileTranslator::MFileKind   CameraExport::identifyFile( const MFileObject &file, const char *buffer,short size) const
{
  const char *name = file.name().asChar();
  int nameLength = strlen(name);
  return (kIsMyFileType);
}

void *CameraExport::creator()
{
	return new CameraExport();
}

//****************************************************************
MStatus initializePlugin (MObject obj)
{
	MStatus status;
	MFnPlugin plugin (obj, "Nick Chirkov (A), Akella (C) 2001", "1.0");
	status = plugin.registerFileTranslator("SD2CamExport", "", 	CameraExport::creator, 0);
	return status;
}

MStatus uninitializePlugin (MObject obj)
{
	MFnPlugin plugin (obj);
	plugin.deregisterFileTranslator ("SD2CamExport");
	return (MS::kSuccess);
}

//------------------------------------------------------------------------
//find root
//------------------------------------------------------------------------
void CameraExport::ExportAll(const char *fname)
{
	MGlobal::executeCommand("window -tlc 300 300 -w 300 -h 200 -title \"ani Export\" StatusWin",true);
	MGlobal::executeCommand("columnLayout -adjustableColumn true",true);
	MGlobal::executeCommand("text -label \"Default\" MyLable",true);
	MGlobal::executeCommand("showWindow StatusWin",true);
	MGlobal::executeCommand("text -edit -label \"Search root\" MyLable",true);
	MGlobal::executeCommand("showWindow StatusWin",true);

	char newname[256];
	sprintf(newname, "%s.txt", fname);
	LogFile = fopen(newname,"w+");
	char aniname[256];
	strcpy(aniname, newname);
	aniname[strlen(aniname)-4] = 0;
	anifile = fopen(aniname,"w+b");

	//get selected camera
	MSelectionList slist;
	MGlobal::getActiveSelectionList(slist);
	MItSelectionList iter(slist);
	if(iter.isDone())	return;

	MDagPath camera;
	iter.getDagPath(camera);
	if(!camera.hasFn(MFn::kCamera))	return;

	Log("%s\n", camera.fullPathName().asChar());
	//------------------------------------------------------------------------------
	//EXPORT FRAMES
	//------------------------------------------------------------------------------
  MTime startFrame( MAnimControl::minTime().value(), MTime::uiUnit() );
  float startTime = (float)startFrame.value();
  MTime endFrame( MAnimControl::maxTime().value(), MTime::uiUnit() );
  float endTime = (float)endFrame.value();

  for(float time = startTime; time<endTime; time += 1.0f)
  {
		if(time<0.0f)	continue;
		//set current time
		MTime Time(time, MTime::uiUnit());
		MAnimControl::setCurrentTime(Time);
		//iterate over all bones from first selected

		Log("-----------------%f--------------------\n", time);
		double dblv;
		float flv;
		//adjust local position
		GetDoubleValue(camera.node(), "translateX", dblv);
		flv = float(0.01f*dblv);
		Log("pos: %f, ", flv);
		fwrite(&flv,1, sizeof(float), anifile);

		GetDoubleValue(camera.node(), "translateY", dblv);
		flv = float(0.01f*dblv);
		Log("%f, ", flv);
		fwrite(&flv,1, sizeof(float), anifile);

		GetDoubleValue(camera.node(), "translateZ", dblv);
		flv = float(0.01f*dblv);
		Log("%f\n", flv);
		fwrite(&flv,1, sizeof(float), anifile);

		GetDoubleValue(camera.node(), "rotateX", dblv);
		flv = float(dblv);
		Log("ang: %f, ", flv*180.0f/3.1415f);
		fwrite(&flv,1, sizeof(float), anifile);

		GetDoubleValue(camera.node(), "rotateY", dblv);
		flv = float(dblv);
		Log("%f, ", flv*180.0f/3.1415f);
		fwrite(&flv,1, sizeof(float), anifile);

		GetDoubleValue(camera.node(), "rotateZ", dblv);
		flv = float(dblv);
		Log("%f\n", flv*180.0f/3.1415f);
		fwrite(&flv,1, sizeof(float), anifile);

	}
	//------------------------------------------------------------------------------
	fclose(LogFile);
	fclose(anifile);

  MGlobal::executeCommand("deleteUI -window StatusWin;");
}