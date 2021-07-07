/******************************************************************************
File:  AnimationExport.cpp

Author:   Nick Chirkov
Copyright (C) 2001 Akella corp.

Comments:
Maya export
******************************************************************************/
#include <d3dx8.h>
#include "AnimationExport.h"
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

#include "anim_file.h"
#include <vector>
#include "cvector.h"
#include "matrix.h"

#define CM2M_SCALE 0.01f

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

AniExport::AniExport()
{
}

AniExport::~AniExport()
{
}

MStatus AniExport::writer(const MFileObject &file,  const MString &optionsString, FileAccessMode mode)
{
  MString  fname = file.fullName();
	ExportAll(fname.asChar());
  return (MS::kSuccess);
}
bool AniExport::haveWriteMethod () const
{
  return (true);
}
MString AniExport::defaultExtension () const
{
  return MString("an");
}

MPxFileTranslator::MFileKind   AniExport::identifyFile( const MFileObject &file, const char *buffer,short size) const
{
  const char *name = file.name().asChar();
  int nameLength = strlen(name);
  return (kIsMyFileType);
}

void *AniExport::creator()
{
	return new AniExport();
}

//****************************************************************
MStatus initializePlugin (MObject obj)
{
	MStatus status;
	MFnPlugin plugin (obj, "Nick Chirkov (C) 2001", "1.0");
	status = plugin.registerFileTranslator("SD2AniExport", "", 	AniExport::creator,"SD2AniExportMel");
	return status;
}

MStatus uninitializePlugin (MObject obj)
{
	MFnPlugin plugin (obj);
	plugin.deregisterFileTranslator ("SD2AniExport");
	return (MS::kSuccess);
}

//------------------------------------------------------------------------
//find root
//------------------------------------------------------------------------
void AniExport::ExportAll(const char *fname)
{
	MGlobal::executeCommand("window -tlc 300 300 -w 300 -h 200 -title \"ani Export\" StatusWin",true);
	MGlobal::executeCommand("columnLayout -adjustableColumn true",true);
	MGlobal::executeCommand("text -label \"Default\" MyLable",true);
	MGlobal::executeCommand("showWindow StatusWin",true);
	MGlobal::executeCommand("text -edit -label \"Search root\" MyLable",true);
	MGlobal::executeCommand("showWindow StatusWin",true);

	char newname[256];
	sprintf(newname, "%s.ant", fname);
	LogFile = fopen(newname,"w+");
	char aniname[256];
	strcpy(aniname, newname);
	aniname[strlen(aniname)-4] = 0;
	anifile = fopen(aniname,"w+b");

	//get selected locator
	MSelectionList slist;
	MGlobal::getActiveSelectionList(slist);
	MItSelectionList iter(slist);
	if(iter.isDone())	return;
	MDagPath locator;
	iter.getDagPath(locator);
	if(!locator.hasFn(MFn::kLocator))	return;

	//------------------------------------------------------------------------------
	//EXPORT FRAMES
	//------------------------------------------------------------------------------
  MTime startFrame( MAnimControl::minTime().value(), MTime::uiUnit() );
  float startTime = (float)startFrame.value();

  MTime endFrame( MAnimControl::maxTime().value(), MTime::uiUnit() );
  float endTime = (float)endFrame.value();

	ANIFILE::HEADER ah;
	fwrite(&ah, sizeof ah, 1, anifile);
	ah.nJoints = 0;
	ah.nFrames = 0;
	ah.framesPerSec = 15.0f;

	//--------------------------------------------------------
	//all joints
	//--------------------------------------------------------
	//find all joints
	MSelectionList mJointsList;
	MItDag dagIterator( MItDag::kBreadthFirst, MFn::kJoint, &status);
	if(!status)	return;
	for ( ; !dagIterator.isDone(); dagIterator.next() ) 
	{
		MDagPath dagPath;
		status = dagIterator.getPath(dagPath);  // Get DagPath
		if(!status) continue;

		MObject object = dagIterator.item(&status);     // Get MObject
		if(!status) continue;

		mJointsList.add(dagPath, object, false);
		ah.nJoints++;
	}

	//-------------------------------------------------------------
	//parent index to handle hierarchy
	//-------------------------------------------------------------
	long *parIdx = new long[ah.nJoints];
	for(long j=0; j<ah.nJoints; j++)
	{
		MDagPath jp;
		mJointsList.getDagPath(j, jp);
		//go to the parent
		jp.pop();

		parIdx[j] = -1;
		for(long i=0; i<ah.nJoints; i++)
		{
			MDagPath gp;
			mJointsList.getDagPath(i, gp);
			if(gp==jp)
			{
				parIdx[j] = i;
				break;
			}
		}
	}
	fwrite(parIdx, sizeof(*parIdx), ah.nJoints, anifile);

	MTime Time(startTime, MTime::uiUnit());
	MAnimControl::setCurrentTime(Time);

	//-------------------------------------------------------------
	//export frames
	//-------------------------------------------------------------
	std::vector<CVECTOR> begPos;
	std::vector<CVECTOR> rootPos;
	std::vector<D3DXQUATERNION> jointAngle;

  for(float time = startTime; time<endTime; time += 1.0f)
  {
		Log("------------------------------------------------------------------\n");
		Log("%f\n", time);
		//set current time
		MTime Time(time, MTime::uiUnit());
		MAnimControl::setCurrentTime(Time);

		//--------------------------------------------------------
		//frames
		//--------------------------------------------------------
		for(j=0; j<ah.nJoints; j++)
		{
			MDagPath jp;
			mJointsList.getDagPath(j, jp);
			double dblv;

			//----------------------------------
			CVECTOR jPos;
			GetDoubleValue(jp.node(), "translateX", dblv);	jPos.x = float(CM2M_SCALE*dblv);
			GetDoubleValue(jp.node(), "translateY", dblv);	jPos.y = float(CM2M_SCALE*dblv);
			GetDoubleValue(jp.node(), "translateZ", dblv);	jPos.z = float(CM2M_SCALE*dblv);

			CVECTOR rotAng;
			GetDoubleValue(jp.node(), "rotateX", dblv);	rotAng.x = float(dblv);
			GetDoubleValue(jp.node(), "rotateY", dblv);	rotAng.y = float(dblv);
			GetDoubleValue(jp.node(), "rotateZ", dblv);	rotAng.z = float(dblv);
			CMatrix rotate;
			rotate.BuildMatrixXYZ(rotAng.x, rotAng.y, rotAng.z, 0.0f, 0.0f, 0.0f);

			CVECTOR orAng;
			GetDoubleValue(jp.node(), "jointOrientX", dblv);	orAng.x = float(dblv);
			GetDoubleValue(jp.node(), "jointOrientY", dblv);	orAng.y = float(dblv);
			GetDoubleValue(jp.node(), "jointOrientZ", dblv);	orAng.z = float(dblv);
			CMatrix orient;
			orient.BuildMatrixXYZ(orAng.x, orAng.y, orAng.z, jPos.x, jPos.y, jPos.z);

			CMatrix mtxGlob = rotate*orient;
			D3DXQUATERNION rp;
			D3DXQuaternionRotationMatrix(&rp, mtxGlob);
			jointAngle.push_back(rp);

			if(j==0)	rootPos.push_back(mtxGlob.Pos());

			//joints' begin positions
			if(ah.nFrames==0)	begPos.push_back(mtxGlob.Pos());

		}
		ah.nFrames++;
	}

	fwrite(&begPos[0], sizeof CVECTOR, ah.nJoints, anifile);
	fwrite(&rootPos[0], sizeof CVECTOR, ah.nFrames, anifile);

	for(j=0; j<ah.nJoints; j++)
		for(long f=0; f<ah.nFrames; f++)
			fwrite(&jointAngle[f*ah.nJoints + j], sizeof jointAngle[0], 1, anifile);

	Log("%d\n", ah.nFrames);
	fseek(anifile, 0, SEEK_SET);
	fwrite(&ah, sizeof ah, 1, anifile);
	//------------------------------------------------------------------------------
	fclose(LogFile);
	fclose(anifile);

  MGlobal::executeCommand("deleteUI -window StatusWin;");
}
