#define LOG

#define CM2M_SCALE 0.01f

#include <maya/MFnPlugin.h>
#include <maya/MComputation.h>
#include <maya/MPxCommand.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MPointArray.h>
#include <maya/MFloatArray.h>
#include <maya/MFnPhongShader.h>
#include <maya/MFnTransform.h>
#include <maya/MMatrix.h>
#include <maya/MEulerRotation.h>
#include <maya/MTime.h>
#include <maya/MAnimControl.h>
#include <maya/MFloatPointArray.h>

#include"AtlaExport.h"

constexpr auto R2Y = 0.299f;
constexpr auto G2Y = 0.587f;
constexpr auto B2Y = 0.114f;

#include <cmath>

#include <vector>
#include <format>
#include <maya/MDagPathArray.h>
#include <maya/MItGeometry.h>


bool MakePathForTraceMaker(char* buf)
{
	const char* cl = ::GetCommandLineA();
	if (cl[0] != '"') return false;
	long i;
	for (i = 1; cl[i] && cl[i] != '"'; i++) buf[i - 1] = cl[i];
	if (cl[i] != '"') return false;
	for (i--; buf[i] != '\\' && i >= 0; i--);
	i++;
	const char* fileName = "TraceMaker.exe";
	for (long j = 0; fileName[j]; i++, j++) buf[i] = fileName[j];
	buf[i] = 0;
	return true;
}


EXPORT_STATIC* rexp;
FILE* LogFile = 0;

void Log(const char* fmt, ...)
{
#ifdef LOG
	va_list args;
	va_start(args, fmt);
	vfprintf(LogFile, fmt, args);
	va_end(args);
#endif
}

MStatus status;
#include <maya/MItDependencyGraph.h>
#include <maya/MFnStringData.h>

bool Result(const std::string& s)
{
	MString command = "text -edit -label \"";
	command += s.c_str();
	command += "\" MyLable";
	MGlobal::executeCommand(command, true);
	//MGlobal::executeCommand("showWindow StatusWin",true);
	return true;
}

bool Error(const std::string& s)
{
	MString command = "text -edit -label \"";
	command += s.c_str();
	command += "\" MyLable";
	MGlobal::executeCommand(command, true);
	FILE* errfl = fopen("error.txt", "w+a");
	fprintf(errfl, "%s\n", s.c_str());
	fclose(errfl);
	return true;
}

#include <maya/MItDependencyNodes.h>
#include <maya/MFnSkinCluster.h>

struct BONEWEIGHT
{
	long idBone[4];
	float weight[4];
};

MDagPathArray mJointsList{};

void FillJoints(MDagPath& path)
{
	MItDag dagIterator(MItDag::kBreadthFirst, MFn::kJoint, &status);
	if (!status) return;

	for (; !dagIterator.isDone(); dagIterator.next())
	{
		MDagPath dagPath;
		status = dagIterator.getPath(dagPath); // Get DagPath
		if (!status) return;

		//MObject object = dagIterator.currentItem(&status); // Get MObject
		//if (!status) return;

		status = mJointsList.append(dagPath);
		//status = mJointsList.add(dagPath, object, false);
		//Log( "JOINT_ADDED: %s\n", dagPath.fullPathName().asChar());
	}
}

void SaveAnimation(BONEWEIGHT* ani, MDagPath& path, long nverts)
{
	memset(ani, 0, sizeof(BONEWEIGHT) * nverts);

	MStatus status;
	auto name = path.fullPathName();
	Log("\n\n\nobject: %s\n", name.asChar());
	FillJoints(path);

	MItDependencyNodes iter(MFn::kSkinClusterFilter); // Create SkinClusters iterator
	if (iter.isDone()) return; // No SkinClusters

	for (; !iter.isDone(); iter.next())
	{
#if MAYA_API_VERSION <= 500
		MObject object = iter.item();
#else
		MObject object = iter.thisNode();
#endif
		MFnSkinCluster fnSkinCluster;
		status = fnSkinCluster.setObject(object); // Get MFnSkinCluster interface
		if (!status) continue;

		int numConnections = fnSkinCluster.numOutputConnections(&status);
		if (numConnections == 0) continue;

		for (int con = 0; con < numConnections; con++) // Check Output connections
		{
			int PlugIndex = fnSkinCluster.indexForOutputConnection(con, &status);
			if (!status) continue;

			MDagPath SkinDagPath;
			status = fnSkinCluster.getPathAtIndex(PlugIndex, SkinDagPath); // Check connectoin to current shape
			SkinDagPath.pop();
			if (!(SkinDagPath == path)) continue;

			MDagPathArray InfsObjectsArray;
			fnSkinCluster.influenceObjects(InfsObjectsArray, &status);
			if (!status) return;

			MItGeometry gIter(SkinDagPath);
			for (; !gIter.isDone(); gIter.next()) // cycle for all vertexes in geometry --
			{
#if MAYA_API_VERSION <= 500
				MObject component = gIter.component(&status);
#else
				MObject component = gIter.currentItem(&status);
#endif
				if (!status) return;

				MFloatArray WeightArray;
				unsigned int numInfl;
				status = fnSkinCluster.getWeights(SkinDagPath, component, WeightArray, numInfl);
				if (!status) return;

				int idxVrt = gIter.index();
				int count = 0;
				for (long i = 0; i < numInfl; i++)
				{
					if (WeightArray[i] == 0.0f) continue;

					long J_Index = -1;
					for (long joint = 0; joint < mJointsList.length(); joint++)
					{
						MDagPath dagPath = mJointsList[joint];
						if (!(InfsObjectsArray[i] == dagPath)) continue;
						J_Index = joint;
						break;
					}
					if (J_Index < 0) return;

					if (count >= 2) // More then 4 objects
					{
						long j = 0;
						if (ani[idxVrt].weight[1] < ani[idxVrt].weight[0]) j = 1;

						if (WeightArray[i] > std::max(ani[idxVrt].weight[0], ani[idxVrt].weight[1]))
						{
							ani[idxVrt].weight[j] = WeightArray[i];
							ani[idxVrt].idBone[j] = J_Index;
						}
					}
					else // Less then 4 objects
					{
						ani[idxVrt].weight[count] = WeightArray[i];
						ani[idxVrt].idBone[count] = J_Index;
					}
					count++;
				}


				//normalize value
				float normw = 1.0f / (ani[idxVrt].weight[0] + ani[idxVrt].weight[1]);
				ani[idxVrt].weight[0] *= normw;
				ani[idxVrt].weight[1] *= normw;

				for (long w = 0; w < 4; w++)
					Log("%f, %d, ", ani[idxVrt].weight[w], ani[idxVrt].idBone[w]);
				Log("\n");
			} //all geometry
		} //connections
	}
};


AtlaExport::AtlaExport()
{
}

AtlaExport::~AtlaExport()
{
}

void MulMtx(float* matrix, float* m1, float* m2)
{
	matrix[0] = m2[0] * m1[0] + m2[4] * m1[1] + m2[8] * m1[2] + m2[12] * m1[3];
	matrix[1] = m2[1] * m1[0] + m2[5] * m1[1] + m2[9] * m1[2] + m2[13] * m1[3];
	matrix[2] = m2[2] * m1[0] + m2[6] * m1[1] + m2[10] * m1[2] + m2[14] * m1[3];
	matrix[3] = m2[3] * m1[0] + m2[7] * m1[1] + m2[11] * m1[2] + m2[15] * m1[3];

	matrix[4] = m2[0] * m1[4] + m2[4] * m1[5] + m2[8] * m1[6] + m2[12] * m1[7];
	matrix[5] = m2[1] * m1[4] + m2[5] * m1[5] + m2[9] * m1[6] + m2[13] * m1[7];
	matrix[6] = m2[2] * m1[4] + m2[6] * m1[5] + m2[10] * m1[6] + m2[14] * m1[7];
	matrix[7] = m2[3] * m1[4] + m2[7] * m1[5] + m2[11] * m1[6] + m2[15] * m1[7];

	matrix[8] = m2[0] * m1[8] + m2[4] * m1[9] + m2[8] * m1[10] + m2[12] * m1[11];
	matrix[9] = m2[1] * m1[8] + m2[5] * m1[9] + m2[9] * m1[10] + m2[13] * m1[11];
	matrix[10] = m2[2] * m1[8] + m2[6] * m1[9] + m2[10] * m1[10] + m2[14] * m1[11];
	matrix[11] = m2[3] * m1[8] + m2[7] * m1[9] + m2[11] * m1[10] + m2[15] * m1[11];

	matrix[12] = m2[0] * m1[12] + m2[4] * m1[13] + m2[8] * m1[14] + m2[12] * m1[15];
	matrix[13] = m2[1] * m1[12] + m2[5] * m1[13] + m2[9] * m1[14] + m2[13] * m1[15];
	matrix[14] = m2[2] * m1[12] + m2[6] * m1[13] + m2[10] * m1[14] + m2[14] * m1[15];
	matrix[15] = m2[3] * m1[12] + m2[7] * m1[13] + m2[11] * m1[14] + m2[15] * m1[15];
}

bool GetFloatValues(MObject& obj, const char* name, float& f0, float& f1)
{
	MFnDependencyNode NodeFnDn;
	NodeFnDn.setObject(obj);
	MStatus status;
#if MAYA_API_VERSION <= 500
	MPlug rpuv = NodeFnDn.findPlug(name, &status);
#else
	MPlug rpuv = NodeFnDn.findPlug(name, false, &status);
#endif
	if (!status) return false;
	MObject val;
	rpuv.getValue(val);
	MFnNumericData numFn(val);
	numFn.getData(f0, f1);
	//Log( "%s, %f, %f\n", name, f0, f1);
	return true;
}

bool GetDoubleValue(MObject& obj, const char* name, double& d)
{
	MFnDependencyNode NodeFnDn;
	NodeFnDn.setObject(obj);
	MStatus status;
#if MAYA_API_VERSION <= 500
	MPlug rpuv = NodeFnDn.findPlug(name, &status);
#else
	MPlug rpuv = NodeFnDn.findPlug(name, false, &status);
#endif
	if (!status) return false;
	MObject val;
	rpuv.getValue(d);
	return true;
}

MMatrix GetLocator(MDagPath& path, MMatrix& localRotation)
{
	//adjust local position
	double dbvx, dbvy, dbvz;
	MDagPath shape = path;
	shape.push(path.child(0));
	auto obj = shape.node();
	GetDoubleValue(obj, "localPositionX", dbvx);
	GetDoubleValue(obj, "localPositionY", dbvy);
	GetDoubleValue(obj, "localPositionZ", dbvz);
	MVector translate;
	translate.x = dbvx;
	translate.y = dbvy;
	translate.z = dbvz;
	MTransformationMatrix mtrs;
	mtrs.setTranslation(translate, MSpace::kWorld);

	//rotation component will be stored
	obj = path.node();
	GetDoubleValue(obj, "rotateX", dbvx);
	GetDoubleValue(obj, "rotateY", dbvy);
	GetDoubleValue(obj, "rotateZ", dbvz);
	MEulerRotation rt;
	rt.x = dbvx;
	rt.y = dbvy;
	rt.z = dbvz;
	MTransformationMatrix mtrr;
	mtrr.rotateTo(rt);
	localRotation = mtrr.asMatrix();

	MMatrix mtx = mtrs.asMatrix() * path.inclusiveMatrix();
	return mtx;
}

bool ExportCharacterPatch;

MStatus AtlaExport::writer(const MFileObject& file, const MString& optionsString, FileAccessMode mode)
{
	MStringArray optionList;
	MStringArray theOption;
	optionsString.split(';', optionList);

	is_scaled_ = false;
	bool bsp = false;
	bool animation = false;
	bool product = false;
	ExportCharacterPatch = false;

	for (DWORD i = 0; i < optionList.length(); ++i)
	{
		theOption.clear();
		optionList[i].split('=', theOption);
		if (theOption.length() <= 1) continue;

		if (theOption[0] == MString("Check1"))
		{
			if (theOption[1].asInt()) is_scaled_ = true;
		}
		if (theOption[0] == MString("Check2"))
		{
			if (theOption[1].asInt()) bsp = true;
		}
		if (theOption[0] == MString("Check3"))
		{
			if (theOption[1].asInt()) animation = true;
		}
		if (theOption[0] == MString("ExportCharacterPatch"))
		{
			if (theOption[1].asInt()) ExportCharacterPatch = true;
		}
		/*if(theOption[0] == MString("radioGrp1"))
		{
		    if( theOption[1].asInt()==2 ) product = true;
		    else	product = false;
		}*/

		//----------------------------------//
	} // next i(element list)

	if (ExportCharacterPatch) bsp = true;
	GlobAnimation = animation;
	if (GlobAnimation) bsp = false;
	
#if MAYA_API_VERSION <= 500
	MString fname = file.fullName();
#else
	MString fname = file.expandedFullName();
#endif
	ExportAll(fname.asChar(), bsp);
	return (MS::kSuccess);
}

bool AtlaExport::haveWriteMethod() const
{
	return (true);
}

MString AtlaExport::defaultExtension() const
{
	return MString("gm");
}

MPxFileTranslator::MFileKind AtlaExport::identifyFile(const MFileObject& file, const char* buffer, short size) const
{
#if MAYA_API_VERSION <= 500
	const char* name = file.name().asChar();
#else
	const char* name = file.resolvedName().asChar();
#endif
	int nameLength = strlen(name);

	return (kIsMyFileType);
	//return (kNotMyFileType);
}

void* AtlaExport::creator()
{
	return new AtlaExport();
}

// command for adding extra parameters
class ExtraCommand : public MPxCommand
{
public:
	MStatus doIt(const MArgList&)
	{
		MItDag itDag;
		MDagPath dagPath;

		while (!itDag.isDone())
		{
			itDag.getPath(dagPath);
			itDag.next();
			if (!dagPath.hasFn(MFn::kMesh)) continue;

			MFnDependencyNode fnDep(dagPath.node());

			if (dagPath.hasFn(MFn::kTransform))
			{
				MFnNumericAttribute attr;

				MObject rdfattrib = attr.create("visible", "visible", MFnNumericData::kBoolean, 1);
				fnDep.addAttribute(rdfattrib);
				rdfattrib = attr.create("static_light", "static_light", MFnNumericData::kBoolean, 1);
				fnDep.addAttribute(rdfattrib);
				rdfattrib = attr.create("dynamic_light", "dynamic_light", MFnNumericData::kBoolean, 1);
				fnDep.addAttribute(rdfattrib);
				rdfattrib = attr.create("merge", "merge", MFnNumericData::kBoolean, 1);
				fnDep.addAttribute(rdfattrib);
				rdfattrib = attr.create("collision", "collision", MFnNumericData::kBoolean, 1);
				fnDep.addAttribute(rdfattrib);
				rdfattrib = attr.create("cast_shadows", "cast_shadows", MFnNumericData::kBoolean, 1);
				fnDep.addAttribute(rdfattrib);
			}
			/*else
			{
				MObject att;
				att = fnDep.attribute("visible", &status);	if(status==MS::kSuccess)	fnDep.removeAttribute(att);
				att = fnDep.attribute("static_light", &status);	if(status==MS::kSuccess)	fnDep.removeAttribute(att);
				att = fnDep.attribute("dynamic_light", &status);	if(status==MS::kSuccess)	fnDep.removeAttribute(att);
				att = fnDep.attribute("merge", &status);	if(status==MS::kSuccess)	fnDep.removeAttribute(att);
				att = fnDep.attribute("collision", &status);	if(status==MS::kSuccess)	fnDep.removeAttribute(att);
				att = fnDep.attribute("geometry_access", &status);	if(status==MS::kSuccess)	fnDep.removeAttribute(att);
				att = fnDep.attribute("cast_shadows", &status);	if(status==MS::kSuccess)	fnDep.removeAttribute(att);
			}//*/
		}
		return MS::kSuccess;
	}

	static void* creator()
	{
		return new ExtraCommand();
	}
};

//****************************************************************
__declspec(dllexport) MStatus initializePlugin(MObject obj)
{
	using namespace std::string_literals;
	MStatus status;
	MFnPlugin plugin(obj, "Nick Chirkov (C) 2001", "1.0");
	auto pixmapName = ""s;
	auto pluginName = "SD2GeoExportMel"s;
	status = plugin.registerFileTranslator("SD2GeoExport", &pixmapName[0], AtlaExport::creator, &pluginName[0]);
	plugin.registerCommand("addsex", ExtraCommand::creator);
	return (status);
}

__declspec(dllexport) MStatus uninitializePlugin(MObject obj)
{
	MFnPlugin plugin(obj);
	plugin.deregisterFileTranslator("SD2GeoExport");
	plugin.deregisterCommand("addsex");
	return (MS::kSuccess);
}

size_t AtlaExport::CountObjects(const MDagPath& path)
{
	auto object_count = 0ull;
	//geometry object
	if (path.hasFn(MFn::kMesh) && path.hasFn(MFn::kTransform))
	{
		object_count++;
	}

	//recurse all children
	const auto children_count = path.childCount(&status);
	for (auto i = 0u; i < children_count; i++)
	{
		auto child = path.child(i, &status);
		auto new_path = path;
		new_path.push(child);

		//export locator
		object_count += CountObjects(new_path);
	}

	return object_count;
}


float locator_mtx[4][4];
static long depth = -1;

void AtlaExport::ExportLocator(MDagPath& cl)
{
	//search if selected
	MSelectionList slist;
	MGlobal::getActiveSelectionList(slist);
	MItSelectionList iter(slist);
	MItDag dagIterator(MItDag::kDepthFirst, MFn::kInvalid, &status);
	for (; !iter.isDone(); iter.next())
	{
		MDagPath objectPath;
		status = iter.getDagPath(objectPath);
		if (objectPath == cl)
			break;
	}
	//if locator is not selected do not export
	if (iter.isDone() == true) return;

	MDagPath gpath(cl);
	MMatrix localRotation;
	MMatrix locator_m = GetLocator(gpath, localRotation);

	gpath.pop();
	MFnDagNode fdagnodeg(gpath);
	MFnDagNode fdagnode(cl);

	EXPORT_STATIC::LABEL loc;
	loc.group_name = std::string(fdagnodeg.name().asChar());
	loc.name = std::string(fdagnode.name().asChar());
	loc.flags = 0;
	memset(&loc.bones[0], 0, sizeof(loc.bones));
	memset(&loc.weight[0], 0, sizeof(loc.weight));

	long joint;
	for (joint = 0; joint < mJointsList.length(); joint++)
	{
		MDagPath dagPath = mJointsList[joint];
		if (gpath == dagPath) break;
	}
	//-----------------------------------------------
	//get matrix for locator
	float mtx[4][4];
	MMatrix glob_parent(locator_mtx);
	MMatrix local_loc;
	//joint is correct
	if (joint < mJointsList.length())
	{
		loc.bones[0] = joint;
		Log("locator_bone: %d\n", joint);
		/*		MDagPath jointPath;
		    mJointsList.getDagPath(joint, jointPath);
				MMatrix jm = jointPath.inclusiveMatrix().transpose();
				local_loc = locator_m * jm;
		Log( "bone Matrix: \n");
		for(long ry=0; ry<4; ry++)
		{
		for(long rx=0; rx<4; rx++)
		Log( "%f, ", jm[ry][rx]);
		Log( "\n\n");
		}*/
	} //else
	local_loc = locator_m * glob_parent;

	if (GlobAnimation)
	{
		local_loc[3][0] *= 0.01f;
		local_loc[3][1] *= 0.01f;
		local_loc[3][2] *= 0.01f;
	}
	else
	{
		local_loc[0][0] = local_loc[1][1] = local_loc[2][2] = local_loc[3][3] = 1.0f;
		local_loc[0][1] = local_loc[0][2] = local_loc[0][3] = 0.0f;
		local_loc[1][0] = local_loc[1][2] = local_loc[1][3] = 0.0f;
		local_loc[2][0] = local_loc[2][1] = local_loc[2][3] = 0.0f;
		local_loc[3][0] *= -0.01f;
		local_loc[3][1] *= 0.01f;
		local_loc[3][2] *= 0.01f;
		local_loc = localRotation.transpose() * local_loc;
	}

	//store to locator
	local_loc.get(mtx);
	memcpy(&loc.vector[0][0], &mtx[0][0], sizeof(loc.vector));

	//memset(loc.vector, 0, sizeof loc.vector);	loc.vector[0][0] = loc.vector[1][1] = loc.vector[2][2] = loc.vector[3][3] = 1.0f;

	Log("LOCATOR ADDED TO HIERARCHY: %s\n", loc.name.c_str());
	for (long ry = 0; ry < 4; ry++)
	{
		for (long rx = 0; rx < 4; rx++)
			Log("%f, ", loc.vector[ry][rx]);
		Log("\n");
	}
	for (long l = 0; l < locators_.size(); l++)
		if (locators_[l] == loc.name)
		{
			loc.group_name = std::string("geometry");
			break;
		}

	Log("group_name: %s\n", loc.group_name.c_str());
	Log("name: %s\n\n", loc.name.c_str());

	//------------------------------------------------------------------------
	//add locator
	//------------------------------------------------------------------------
	try
	{
		rexp->AddLabel(loc);
	}
	catch (char* errmsg)
	{
		MString command = "text -edit -label \"";
		command += errmsg;
		command += "\" MyLable";
		MGlobal::executeCommand(command, true);
		MGlobal::executeCommand("showWindow StatusWin", true);
		Sleep(2000);
	}
}

void AtlaExport::Traverse(MDagPath& path)
{
	depth++;
	//--------------------------------------------------------
	//geometry object
	//--------------------------------------------------------
	if (path.hasFn(MFn::kMesh) && path.hasFn(MFn::kTransform))
	{
		MString command = "text -edit -label \"Export object ";
		char ee[256];
		sprintf(ee, "%d / %d", curobject + 1, total_objects);
		command += ee;
		command += "\" MyLable";
		MGlobal::executeCommand(command, true);
		MGlobal::executeCommand("showWindow StatusWin", true);
		curobject++;

		EXPORT_STATIC::OBJECT exp_obj;
		EXPORT_STATIC::MATERIAL exp_mtl;
		EXPORT_STATIC::TRIANGLE exp_trg;
		//------------------------------------------------------------------------
		//object
		MFnMesh fnMesh(path, &status);
		if (!status) goto SkipObj; //throw "error fnMesh";

		//group & object name
		MDagPath gpath(path);
		gpath.pop();
		MFnDagNode fdagnodeg(gpath);
		MFnDagNode fdagnode(path);
		exp_obj.group_name = std::string(fdagnodeg.name(&status).asChar());
		exp_obj.name = std::string(fdagnode.name(&status).asChar());

		//------------------------------------------------------------------------
		//extra attributes
		exp_obj.flags = 0;

		bool vis, stlt, dnlt, mg, col, cs;
#if MAYA_API_VERSION <= 500
		fdagnode.findPlug("visible", &status).getValue(vis);
#else
		fdagnode.findPlug("visible", false, &status).getValue(vis);
#endif
		if (!status)
		{
			exp_obj.flags |= EXPORT_STATIC::OBJECT_FLAGS::VISIBLE;
			exp_obj.flags |= EXPORT_STATIC::OBJECT_FLAGS::STATIC_LIGHT_ENABLE;
			exp_obj.flags |= EXPORT_STATIC::OBJECT_FLAGS::DINAMIC_LIGHT_ENABLE;
			exp_obj.flags |= EXPORT_STATIC::OBJECT_FLAGS::MERGE;
			exp_obj.flags |= EXPORT_STATIC::OBJECT_FLAGS::COLLISION_ENABLE;
			exp_obj.flags |= EXPORT_STATIC::OBJECT_FLAGS::CAST_SHADOWS_ENABLE;
		}
		else
		{
#if MAYA_API_VERSION <= 500
			fdagnode.findPlug("static_light", nullptr).getValue(stlt);
			fdagnode.findPlug("dynamic_light", nullptr).getValue(dnlt);
			fdagnode.findPlug("merge", nullptr).getValue(mg);
			fdagnode.findPlug("collision", nullptr).getValue(col);
			fdagnode.findPlug("cast_shadows", nullptr).getValue(cs);
#else
			fdagnode.findPlug("static_light", false, nullptr).getValue(stlt);
			fdagnode.findPlug("dynamic_light", false, nullptr).getValue(dnlt);
			fdagnode.findPlug("merge", false, nullptr).getValue(mg);
			fdagnode.findPlug("collision", false, nullptr).getValue(col);
			fdagnode.findPlug("cast_shadows", false, nullptr).getValue(cs);
#endif

			if (vis) exp_obj.flags |= EXPORT_STATIC::OBJECT_FLAGS::VISIBLE;
			if (stlt) exp_obj.flags |= EXPORT_STATIC::OBJECT_FLAGS::STATIC_LIGHT_ENABLE;
			if (dnlt) exp_obj.flags |= EXPORT_STATIC::OBJECT_FLAGS::DINAMIC_LIGHT_ENABLE;
			if (mg) exp_obj.flags |= EXPORT_STATIC::OBJECT_FLAGS::MERGE;
			if (col) exp_obj.flags |= EXPORT_STATIC::OBJECT_FLAGS::COLLISION_ENABLE;
			if (cs) exp_obj.flags |= EXPORT_STATIC::OBJECT_FLAGS::CAST_SHADOWS_ENABLE;
		}

		//------------------------------------------------------------------------
		//double side
		bool doublesided = false;
#if MAYA_API_VERSION <= 500
		MPlug pdside = fnMesh.findPlug("doubleSided", &status);
#else
		MPlug pdside = fnMesh.findPlug("doubleSided", false, &status);
#endif
		if (status) pdside.getValue(doublesided);
		if (!doublesided) exp_obj.flags |= EXPORT_STATIC::OBJECT_FLAGS::CULLENABLE;

		//------------------------------------------------------------------------
		//opposite
		bool opposite = false;
#if MAYA_API_VERSION <= 500
		MPlug poppos = fnMesh.findPlug("opposite", &status);
#else
		MPlug poppos = fnMesh.findPlug("opposite", false, &status);
#endif
		if (status) poppos.getValue(opposite);

		//------------------------------------------------------------------------
		//object info

		//get uvset names
		long uvsets = fnMesh.numUVSets();
		MStringArray msArray;
		fnMesh.getUVSetNames(msArray);
		long uvsettype[8];
		//calculate UVsets
		bool color_present = false;
		long s;
		for (s = 0; s < uvsets; s++)
			if (msArray[s] == "color")
				color_present = true;

		for (s = 0; s < uvsets; s++)
		{
			uvsettype[s] = -1;

			//base texture
			if (msArray[s] == "bump") uvsettype[s] = 1;
			else if (msArray[s] == "color") uvsettype[s] = 0;
			else
				if (!color_present && msArray[s] == "map1") uvsettype[s] = 0;
		}

		long InstaceNumber = path.instanceNumber(&status);
		if (!status)
		{
			goto SkipObj; //throw "error instanceNumber";
		}

		MObjectArray ArrayOfSharers;
		MIntArray ArrayOfIndex;
		status = fnMesh.getConnectedShaders(InstaceNumber, ArrayOfSharers, ArrayOfIndex);
		if (!status)
		{
			Error(std::string("error getConnectedShaders"));
			goto SkipObj; //throw "error getConnectedShaders";
		}

		//------------------------------------------------------------------------
		//material
		//------------------------------------------------------------------------
		//reference
		for (unsigned long i = 0; i < ArrayOfSharers.length(); i++)
		{
			long m;
			for (m = 0; m < nummat; m++)
				//if(ArrayOfSharers[i]==material[m].MO)	break;// && uvsets==material[m].ntex)	break;
				if (ArrayOfSharers[i] == *material[m].MO) break;

			if (m == nummat)
			{
				//optical params
				MFnDependencyNode fnNode;
				fnNode.setObject(ArrayOfSharers[i]);
#if MAYA_API_VERSION <= 500
				MPlug shaderPlug = fnNode.findPlug("surfaceShader", &status);
#else
				MPlug shaderPlug = fnNode.findPlug("surfaceShader", false, &status);
#endif
				if (status)
				{
					MPlugArray connectedPlugs;
					shaderPlug.connectedTo(connectedPlugs, true, false);
					if (connectedPlugs.length() == 1)
					{
						MObject ShaderObject = connectedPlugs[0].node();
						switch (ShaderObject.apiType())
						{
						case MFn::kPhong:
							{
								MFnPhongShader fnShader(ShaderObject);
								material[m].gloss = fnShader.cosPower();
								material[m].specular = fnShader.reflectivity();
								MColor sp_color = fnShader.specularColor(&status);
								if (status) material[m].specular = sp_color.r * R2Y + sp_color.g * G2Y + sp_color.b *
									B2Y;
								material[m].brdf = EXPORT_STATIC::PHONG; //brdf
								material[m].diffuse = fnShader.diffuseCoeff(); //diffuse
								MColor si_color = fnShader.incandescence(&status); //selfillumination
								if (status) material[m].selfIllum = R2Y * si_color.r + G2Y * si_color.g + B2Y * si_color
									.b;
								MColor tr_color = fnShader.transparency(&status); //transparency
								if (status) material[m].transparency = R2Y * tr_color.r + G2Y * tr_color.g + B2Y *
									tr_color.b;
								material[m].refraction_id = fnShader.refractiveIndex(&status); //refraction
#if MAYA_API_VERSION <= 500
								MPlug reflectPlug = fnShader.findPlug("reflectionSpecularity", &status);
#else
								MPlug reflectPlug = fnShader.findPlug("reflectionSpecularity", false, &status);
#endif
								//reflection
								if (status) reflectPlug.getValue(material[m].reflection);
							}
							break;
						case MFn::kLambert:
							{
								MFnLambertShader fnShader(ShaderObject);
								material[m].brdf = EXPORT_STATIC::PHONG; //brdf
								material[m].diffuse = fnShader.diffuseCoeff(); //diffuse
								MColor si_color = fnShader.incandescence(&status); //selfillumination
								if (status) material[m].selfIllum = R2Y * si_color.r + G2Y * si_color.g + B2Y * si_color
									.b;
								MColor tr_color = fnShader.transparency(&status); //transparency
								if (status) material[m].transparency = R2Y * tr_color.r + G2Y * tr_color.g + B2Y *
									tr_color.b;
								material[m].refraction_id = fnShader.refractiveIndex(&status); //refraction
#if MAYA_API_VERSION <= 500
								MPlug reflectPlug = fnShader.findPlug("reflectionSpecularity", &status);
#else
								MPlug reflectPlug = fnShader.findPlug("reflectionSpecularity", false, &status);
#endif
								//reflection
								if (status) reflectPlug.getValue(material[m].reflection);
								material[m].gloss = 2.0f;
								material[m].specular = 0.0f;
								material[m].specular = 0.0f;
							}
							break;
						}
					}
				}


				material[m].MO = new MObject(ArrayOfSharers[i]);
				material[m].ntex = 0;
				//shader's name
				MFnDependencyNode fndn(ArrayOfSharers[i]);
				sprintf(material[m].matname, "%s", fndn.name().asChar());
				//material[m].texname[0][0] = 0;
				sprintf(material[m].texname[0], "%s", "checker.tga");
				material[m].texname[1][0] = 0;
				material[m].texname[2][0] = 0;
				material[m].texname[3][0] = 0;

				//------------------------------------------------------------------------
				//find lambert
				MItDependencyGraph DpGraph(ArrayOfSharers[i],
				                           MFn::kLambert,
				                           MItDependencyGraph::kUpstream,
				                           MItDependencyGraph::kBreadthFirst,
				                           MItDependencyGraph::kNodeLevel, &status);
				DpGraph.disablePruningOnFilter();
#if MAYA_API_VERSION <= 500
				MObject LambertNode = DpGraph.thisNode();
#else
				MObject LambertNode = DpGraph.currentItem();
#endif

				//------------------------------------------------------------------------
				status = DpGraph.resetTo(LambertNode, MFn::kLayeredTexture,
				                         MItDependencyGraph::kUpstream,
				                         MItDependencyGraph::kBreadthFirst,
				                         MItDependencyGraph::kNodeLevel);
				DpGraph.enablePruningOnFilter();

				//if layered texture found
				if (!DpGraph.isDone())
				{
#if MAYA_API_VERSION <= 500
					auto obj = DpGraph.thisNode();
#else
					auto obj = DpGraph.currentItem();
#endif
					status = DpGraph.resetTo(obj, MFn::kFileTexture,
					                         MItDependencyGraph::kUpstream,
					                         MItDependencyGraph::kBreadthFirst,
					                         MItDependencyGraph::kNodeLevel);
					//all layers
					if (status)
					{
						while (!DpGraph.isDone())
						{
							long curlayer = -1;

							//search associated textures
							for (long s = 0; s < uvsets; s++)
							{
								MObjectArray texArr;
								fnMesh.getAssociatedUVSetTextures(msArray[s], texArr);

								//curlayer = -1 if no textures for this set
								//if(texArr.length()==0)	break;

								//if map1 uvset and 2 textures associated so
								if (uvsets == 1 && texArr.length() == 2) DpGraph.next();

								//for all textures in this uvset
								long t;
								for (t = 0; t < texArr.length(); t++)
#if MAYA_API_VERSION <= 500
									if (DpGraph.thisNode() == texArr[t]) break;
#else
									if (DpGraph.currentItem() == texArr[t]) break;
#endif

								if (t < texArr.length())
								{
									curlayer = uvsettype[s];
									break;
								}
							}

							//if color or bump
							if (curlayer >= 0)
							{
								MFnDependencyNode NodeFnDn;
#if MAYA_API_VERSION <= 500
								NodeFnDn.setObject(DpGraph.thisNode());
								MPlug plugToFile = NodeFnDn.findPlug("fileTextureName", &status);
#else
								NodeFnDn.setObject(DpGraph.currentItem());
								MPlug plugToFile = NodeFnDn.findPlug("fileTextureName", false, &status);
#endif
								MObject fnameValue;
								plugToFile.getValue(fnameValue);
								MFnStringData stringFn(fnameValue);
								MString nm = stringFn.string();

								strcpy(material[m].texname[curlayer], stringFn.string().asChar());
#if MAYA_API_VERSION <= 500
								obj = DpGraph.thisNode();
#else
								obj = DpGraph.currentItem();
#endif
								GetFloatValues(obj, "repeatUV", material[m].repeat[curlayer][0],
								               material[m].repeat[curlayer][1]);
								GetFloatValues(obj, "offset", material[m].offset[curlayer][0],
								               material[m].offset[curlayer][1]);
								GetDoubleValue(obj, "rotateFrame", material[m].rotate[curlayer]);

								material[m].ntex++;
							}

							DpGraph.next();
						}
					}
				}
				else //try to find file texture
				{
					//base texture---------------------------------------
					status = DpGraph.resetTo(LambertNode, MFn::kFileTexture,
					                         MItDependencyGraph::kUpstream,
					                         MItDependencyGraph::kBreadthFirst,
					                         MItDependencyGraph::kNodeLevel);
					DpGraph.enablePruningOnFilter();
					if (status && !DpGraph.isDone())
					{
						MFnDependencyNode NodeFnDn;
#if MAYA_API_VERSION <= 500
						NodeFnDn.setObject(DpGraph.thisNode());
						MPlug plugToFile = NodeFnDn.findPlug("fileTextureName", &status);
#else
						NodeFnDn.setObject(DpGraph.currentItem());
						MPlug plugToFile = NodeFnDn.findPlug("fileTextureName", false, &status);
#endif
						MObject fnameValue;
						plugToFile.getValue(fnameValue);
						MFnStringData stringFn(fnameValue);
						MString nm = stringFn.string();

						strcpy(material[m].texname[0], stringFn.string().asChar());
#if MAYA_API_VERSION <= 500
						auto obj = DpGraph.thisNode();
#else
						auto obj = DpGraph.currentItem();
#endif
						GetFloatValues(obj, "repeatUV", material[m].repeat[0][0], material[m].repeat[0][1]);
						GetFloatValues(obj, "offset", material[m].offset[0][0], material[m].offset[0][1]);
						GetDoubleValue(obj, "rotateFrame", material[m].rotate[0]);

						material[m].ntex++;
					}

					//bump texture---------------------------------------
					status = DpGraph.resetTo(LambertNode, MFn::kBump,
					                         MItDependencyGraph::kUpstream,
					                         MItDependencyGraph::kBreadthFirst,
					                         MItDependencyGraph::kNodeLevel);

					DpGraph.disablePruningOnFilter();
					if (status && !DpGraph.isDone())
					{
#if MAYA_API_VERSION <= 500
						auto obj = DpGraph.thisNode();
#else
						auto obj = DpGraph.currentItem();
#endif
						status = DpGraph.resetTo(obj, MFn::kFileTexture,
						                         MItDependencyGraph::kUpstream,
						                         MItDependencyGraph::kBreadthFirst,
						                         MItDependencyGraph::kNodeLevel);
						//texture file
						DpGraph.enablePruningOnFilter();
						if (status && !DpGraph.isDone())
						{
							MFnDependencyNode NodeFnDn;
#if MAYA_API_VERSION <= 500
							NodeFnDn.setObject(DpGraph.thisNode());
							MPlug plugToFile = NodeFnDn.findPlug("fileTextureName", &status);
#else
							NodeFnDn.setObject(DpGraph.currentItem());
							MPlug plugToFile = NodeFnDn.findPlug("fileTextureName", false, &status);
#endif
							MObject fnameValue;
							plugToFile.getValue(fnameValue);
							MFnStringData stringFn(fnameValue);
							MString nm = stringFn.string();
							strcpy(material[m].texname[1], stringFn.string().asChar());
#if MAYA_API_VERSION <= 500
							obj = DpGraph.thisNode();
#else
							obj = DpGraph.currentItem();
#endif
							GetFloatValues(obj, "repeatUV", material[m].repeat[1][0], material[m].repeat[1][1]);
							GetFloatValues(obj, "offset", material[m].offset[1][0], material[m].offset[1][1]);
							GetDoubleValue(obj, "rotateFrame", material[m].rotate[1]);

							material[m].ntex++;
						}
					}
				}
				if (material[m].ntex > 0)
				{
					nummat++;
					if (nummat == 1024)
					{
						Error(std::string("Internal error: too much materials"));
						depth--;
						return;
					}
				}

				Log("%s\n", material[m].matname);
				Log("%f\n", material[m].selfIllum);
				Log("%f\n", material[m].diffuse);
				Log("%f\n", material[m].gloss);
				Log("%f\n", material[m].specular);
				Log("%d\n", material[m].brdf);
				Log("%f\n", material[m].reflection);
				Log("%f\n", material[m].refraction_id);
				Log("%f\n", material[m].transparency);
				for (long ti = 0; ti < 4; ti++)
				{
					if (material[m].texname[ti][0] == 0) continue;
					Log("%s\n", material[m].texname[ti]);
				}
				Log("\n\n"); //*/
			}

			exp_mtl.brdf = material[m].brdf;
			exp_mtl.diffuse = material[m].diffuse;
			exp_mtl.specular = material[m].specular;
			exp_mtl.gloss = material[m].gloss;
			exp_mtl.selfIllum = material[m].selfIllum;
			exp_mtl.transparency = material[m].transparency;
			exp_mtl.refraction_id = material[m].refraction_id;
			exp_mtl.reflection = material[m].reflection;
			exp_mtl.group_name = std::string("unknown material group");
			exp_mtl.name = std::string(material[m].matname);

			//fill material
			for (long ti = 0; ti < 4; ti++)
			{
				exp_mtl.texture[ti].id = 0;
				exp_mtl.texture[ti].type = EXPORT_STATIC::NONE;
				if (material[m].texname[ti][0] == 0) continue;

				switch (ti)
				{
				case 0:
					exp_mtl.texture[0].type = EXPORT_STATIC::BASE;
					break;
				case 1:
					exp_mtl.texture[1].type = EXPORT_STATIC::NORMAL;
					break;
				case 2:
					exp_mtl.texture[2].type = EXPORT_STATIC::BASE;
					break;
				case 3:
					exp_mtl.texture[3].type = EXPORT_STATIC::BASE;
					break;
				}

				char* ctemp = strrchr(material[m].texname[ti], '\\');
				if (ctemp == 0) ctemp = strrchr(material[m].texname[ti], '/');
				if (ctemp == 0) ctemp = material[m].texname[ti];
				else ctemp++;

				exp_mtl.texture[ti].name = std::string(ctemp);
			}

			//add material
			//if(material[m].ntex>0)//NOTE_IT
			{
				exp_obj.mtl.push_back(exp_mtl);
				matref[i] = m;
			}
		}

		//------------------------------------------------------------------------
		//transformation
		//------------------------------------------------------------------------
		//mtx * path_mtx * INVERSE(locator_mtx)
		//vrt = v*mtx -> vertex is reletive to locator
		MMatrix locator_pos(locator_mtx);
		MMatrix m = path.inclusiveMatrix() * locator_pos;
		float mtx[4][4];
		status = m.get(mtx);


		MTransformationMatrix mtrans = MTransformationMatrix(m);
		double scale[3];
		mtrans.getScale(scale, MSpace::kWorld);
		if (scale[0] * scale[1] * scale[2] < 0.0f) opposite = !opposite;
		/*Log( "OBJECT_TRANSFORM: %s::%s\n", fdagnodeg.name(&status).asChar(), fdagnode.name(&status).asChar());
		for(long ry=0; ry<4; ry++)
		{
			for(long rx=0; rx<4; rx++)
				Log( "%f, ", mtx[ry][rx]);
			Log( "\n");
		}
		Log( "\n");//*/

		//------------------------------------------------------------------------
		//polygon
		//------------------------------------------------------------------------
		//get vertex coord
		MPointArray mesh_vrt;
		fnMesh.getPoints(mesh_vrt);

		MFloatVectorArray meshNorm;
		fnMesh.getNormals(meshNorm, MSpace::kWorld);

		BONEWEIGHT* ani = new BONEWEIGHT[fnMesh.numVertices()];
		if (GlobAnimation) SaveAnimation(ani, path, fnMesh.numVertices());
		//get colors
		MColorArray mesh_col;
		fnMesh.getFaceVertexColors(mesh_col);

		//get points
		MFloatPointArray vertexArray;
		fnMesh.getPoints(vertexArray);

		//polygon iterator
		MItMeshPolygon itPoly(path);

		for (long p = 0; p < fnMesh.numPolygons(); p++)
		{
			//get triangles
			if (itPoly.hasValidTriangulation() == false)
			{
				itPoly.next();
				continue;
			}
			exp_trg.material = ArrayOfIndex[p];
			long mmt = matref[exp_trg.material];

			MIntArray vrtId;
			itPoly.getVertices(vrtId);

			MPointArray points;
			MIntArray vertexList;
			int nTrgs;
			itPoly.numTriangles(nTrgs);
			for (int nt = 0; nt < nTrgs; nt++)
			{
				itPoly.getTriangle(nt, points, vertexList);
				for (long v = 0; v < 3; v++)
				{
					int faceRelIndex;
					for (faceRelIndex = 0; faceRelIndex < vrtId.length(); faceRelIndex++)
						if (vrtId[faceRelIndex] == vertexList[v])
							break;

					//color-per-vertex
					int colidx;
					fnMesh.getFaceVertexColorIndex(p, v, colidx);
					if (colidx < 0 || colidx >= mesh_col.length() || mesh_col[colidx].a < 0.0f)
						//no colors here
						exp_trg.vrt[v].color = 0xFF7F7F7F;
					else
					{
						long a = long(255.0f * mesh_col[colidx].a);
						long r = std::min(127l, long(127.0f * mesh_col[colidx].r));
						long g = std::min(127l, long(127.0f * mesh_col[colidx].g));
						long b = std::min(127l, long(127.0f * mesh_col[colidx].b));
						exp_trg.vrt[v].color = (a << 24) | (r << 16) | (g << 8) | b;
						//Log( "%lu\n", exp_trg.vrt[v].color);
					}

					//bones and weights
					exp_trg.vrt[v].boneid[0] = ani[vertexList[v]].idBone[0];
					exp_trg.vrt[v].boneid[1] = ani[vertexList[v]].idBone[1];
					exp_trg.vrt[v].boneid[2] = ani[vertexList[v]].idBone[2];
					exp_trg.vrt[v].boneid[3] = ani[vertexList[v]].idBone[3];
					exp_trg.vrt[v].weight[0] = ani[vertexList[v]].weight[0];
					exp_trg.vrt[v].weight[1] = ani[vertexList[v]].weight[1];
					exp_trg.vrt[v].weight[2] = ani[vertexList[v]].weight[2];
					exp_trg.vrt[v].weight[3] = ani[vertexList[v]].weight[3];
					//Log( "%f, %f, %f, %f\n", exp_trg.vrt[v].weight[0], exp_trg.vrt[v].weight[1], exp_trg.vrt[v].weight[2], exp_trg.vrt[v].weight[3]);

					//position
					float dx = mesh_vrt[vertexList[v]].x;
					float dy = mesh_vrt[vertexList[v]].y;
					float dz = mesh_vrt[vertexList[v]].z;
					exp_trg.vrt[v].x = dx * mtx[0][0] + dy * mtx[1][0] + dz * mtx[2][0] + mtx[3][0];
					exp_trg.vrt[v].y = dx * mtx[0][1] + dy * mtx[1][1] + dz * mtx[2][1] + mtx[3][1];
					exp_trg.vrt[v].z = dx * mtx[0][2] + dy * mtx[1][2] + dz * mtx[2][2] + mtx[3][2];

					if (is_scaled_)
					{
						exp_trg.vrt[v].x *= CM2M_SCALE;
						exp_trg.vrt[v].y *= CM2M_SCALE;
						exp_trg.vrt[v].z *= CM2M_SCALE;
					}

					//normal
					MVector norm;
					itPoly.getNormal(faceRelIndex, norm, MSpace::kWorld);
					exp_trg.vrt[v].nx = norm.x;
					exp_trg.vrt[v].ny = norm.y;
					exp_trg.vrt[v].nz = norm.z;

					if (!GlobAnimation)
					{
						exp_trg.vrt[v].x *= -1.0f;
						exp_trg.vrt[v].nx *= -1.0f;
					}

					/*if(opposite)	//reverce normal
					{
						exp_trg.vrt[v].nx *= -1.0f;
						exp_trg.vrt[v].ny *= -1.0f;
						exp_trg.vrt[v].nz *= -1.0f;
					}//*/

					// UV set
					for (long s = 0; s < uvsets; s++)
						if (uvsettype[s] != -1)
						{
							float U, V;
							float uvp[2];
							status = itPoly.getUV(faceRelIndex, uvp, &msArray[s]);
							if (!status)
							{
								Log("can't get UV[%d] for uvSet: %d\n", uvsettype[s], s);
								exp_trg.vrt[v].tu[uvsettype[s]] = 0.62109375f;
								exp_trg.vrt[v].tv[uvsettype[s]] = 0.62109375f;
								continue;
							}
							U = uvp[0];
							V = uvp[1];

							float ang = -material[mmt].rotate[uvsettype[s]];
							float tU, tV;
							tU = (U - 0.5f) * cosf(ang) + (V - 0.5f) * sinf(ang) + 0.5f;
							tV = (V - 0.5f) * cosf(ang) - (U - 0.5f) * sinf(ang) + 0.5f;
							tU *= material[mmt].repeat[uvsettype[s]][0];
							tV *= material[mmt].repeat[uvsettype[s]][1];
							tU += material[mmt].offset[uvsettype[s]][0];
							tV += material[mmt].offset[uvsettype[s]][1];

							switch (uvsettype[s])
							{
							case 0:
								exp_trg.vrt[v].tu[0] = tU;
								exp_trg.vrt[v].tv[0] = -tV;
								break;
							case 1:
								exp_trg.vrt[v].tu[1] = tU;
								exp_trg.vrt[v].tv[1] = -tV;
								break;
							}

							if (uvsets == 1)
								for (long nt = 1; nt < material[mmt].ntex; nt++)
								{
									float ang = material[mmt].rotate[nt];
									tU = (U - 0.5f) * cosf(ang) + (V - 0.5f) * sinf(ang) + 0.5f;
									tV = (V - 0.5f) * cosf(ang) - (U - 0.5f) * sinf(ang) + 0.5f;
									tU *= material[mmt].repeat[nt][0];
									tV *= material[mmt].repeat[nt][1];
									tU += material[mmt].offset[nt][0];
									tV += material[mmt].offset[nt][1];

									exp_trg.vrt[v].tu[nt] = tU;
									exp_trg.vrt[v].tv[nt] = -tV;
								}
						} //*/
				}
				//we use CCW, Maya uses CW back-face culling
				if (!opposite)
				{
					EXPORT_STATIC::VERTEX tvrt = exp_trg.vrt[0];
					exp_trg.vrt[0] = exp_trg.vrt[1];
					exp_trg.vrt[1] = tvrt;
				}
				exp_obj.trg.push_back(exp_trg);
			}

			itPoly.next();
		}
		delete ani;
		//------------------------------------------------------------------------
		//add object
		//------------------------------------------------------------------------
		try
		{
			rexp->AddObject(exp_obj);
		}
		catch (char* errmsg)
		{
			MString command = "text -edit -label \"";
			command += errmsg;
			command += "\" MyLable";
			MGlobal::executeCommand(command, true);
			MGlobal::executeCommand("showWindow StatusWin", true);
			Sleep(2000);
		}
	}
SkipObj:;
	//--------------------------------------------------------
	//recurse all children
	//--------------------------------------------------------
	long nc = path.childCount(&status);
	for (long c = 0; c < nc; c++)
	{
		MObject chlo = path.child(c, &status);
		MDagPath cl = path;
		cl.push(chlo);

		//export locator
		if (cl.hasFn(MFn::kLocator) && cl.hasFn(MFn::kTransform))
		{
			ExportLocator(cl);
		}
		else Traverse(cl);
	}

	//if animation export we needs to link all other locators to the root
	if (GlobAnimation && path.hasFn(MFn::kLocator) && path.hasFn(MFn::kTransform))
	{
		MDagPath gpath(path);
		gpath.pop();

		//if parent of this locator is a joint - skip this
		//if(!gpath.hasFn(MFn::kJoint))
		MFnDagNode fdagnodeg(gpath);
		if (_strcmpi(fdagnodeg.name().asChar(), "world") == 0)
		{
			Log("#######: %s\n", fdagnodeg.name().asChar());

			MSelectionList slist;
			MGlobal::getActiveSelectionList(slist);
			MItSelectionList iter(slist);

			for (; !iter.isDone(); iter.next())
			{
				MDagPath exLoc;
				status = iter.getDagPath(exLoc);

				//if the node is a root
				if (path == exLoc) continue;

				//only locators 
				if (exLoc.hasFn(MFn::kLocator) && exLoc.hasFn(MFn::kTransform))
				{
					ExportLocator(exLoc);
				}
			}
		}
	}
	depth--;
}

void SaveRTXtexture(long id)
{
	//printf(" SaveRTXtexture(%d) \n",id);
}

void AtlaExport::ExportAll(const std::string& file_name, bool bsp)
{
	curobject = total_objects = 0;

	auto path = file_name;
	if (auto extension_offset = path.find(".gm"); extension_offset != std::string::npos)
	{
		path.erase(extension_offset);
	}

	auto selection_list = MSelectionList();
	MGlobal::getActiveSelectionList(selection_list);

	auto dag_iterator = MItDag(MItDag::kDepthFirst, MFn::kInvalid, &status);

	MGlobal::executeCommand("window -tlc 300 300 -w 300 -h 200 -title \"RDF Export\" StatusWin", true);
	MGlobal::executeCommand("columnLayout -adjustableColumn true", true);
	MGlobal::executeCommand("text -label \"Default\" MyLable", true);
	MGlobal::executeCommand("showWindow StatusWin", true);

	MGlobal::executeCommand("text -edit -label \"Search root\" MyLable", true);
	MGlobal::executeCommand("showWindow StatusWin", true);

	auto start_frame = MTime(MAnimControl::minTime().value(), MTime::uiUnit());
	auto time = MTime(start_frame.value(), MTime::uiUnit());
	MAnimControl::setCurrentTime(time);

	//search for many roots
	auto roots = std::vector<std::string>();
	for (auto it = MItSelectionList(selection_list); !it.isDone(); it.next())
	{
		auto object_path = MDagPath();
		status = it.getDagPath(object_path);

		//only locators 
		if (object_path.hasFn(MFn::kLocator) && object_path.hasFn(MFn::kTransform))
		{
			auto name = std::string(object_path.fullPathName().asChar());
			//search for parent or child and remove children
			size_t root_index;
			for (root_index = 0; root_index < roots.size(); ++root_index)
			{
				auto& other_name = roots[root_index];
				if (name.starts_with(other_name))
				{
					break;
				}

				if (other_name.starts_with(name))
				{
					roots.erase(roots.begin() + root_index);
				}
			}

			//add new entry
			if (root_index == roots.size())
			{
				roots.emplace_back(std::move(name));
			}

			//only one root for animated model
			//if(GlobAnimation)	break;
		}
	}

	//-------------------------------------------------------------------
	//calculate number of objects
	locators_.clear();
	for (auto it = MItSelectionList(selection_list); !it.isDone(); it.next())
	{
		auto object_path = MDagPath();
		status = it.getDagPath(object_path);

		//only locators 
		if (object_path.hasFn(MFn::kLocator) && object_path.hasFn(MFn::kTransform))
		{
			//this locator has geometry data and must be marked under "geometry" group
			if (auto objects_num = CountObjects(object_path); objects_num > 0)
			{
				auto node = MFnDagNode(object_path);
				locators_.emplace_back(node.name().asChar());
				total_objects += objects_num;
			}
		}
	}

	//-------------------------------------------------------------------
	//export all selected locators
	for (auto it = MItSelectionList(selection_list); !it.isDone(); it.next())
	{
		auto object_path = MDagPath();
		status = it.getDagPath(object_path);

		//only locators 
		if (object_path.hasFn(MFn::kLocator) && object_path.hasFn(MFn::kTransform))
		{
			auto node = MFnDagNode(object_path);
			auto node_name = std::string(node.name().asChar());

			//skip locators without any objects attached
			if (std::ranges::find(locators_, node_name) == locators_.end())
			{
				continue;
			}

			auto new_path = std::format("{}_{}", path, node_name);
			//auto new_name = std::format("{}_{}.atg", partial_name, pname);

			//find root
			auto object_name = std::string(object_path.fullPathName().asChar());
			if (GlobAnimation || roots.size() == 1 && roots[0] == object_name)
			{
				new_path = path;
			}

#ifdef LOG
			LogFile = fopen((new_path + ".atg").c_str(), "w");
#endif
			nummat = 0;

			//------------------------------------------------------------------------------
			auto gm_path = new_path + ".gm";
			
			long flags = 0;
			if (bsp) flags |= BUILD_BSP;
			//if(product)	flags |= PRODUCTION;
			if (GlobAnimation) flags |= ANIMATION;
			rexp = new GEOM_STATIC(gm_path, SaveRTXtexture, Result, Error, 1e10f, 1.0f, flags);
			//------------------------------------------------------------------------------
			//unsigned long time = GetTickCount();

			//create locator matrix
			auto local_rotation = MMatrix();
			auto locator_m = GetLocator(object_path, local_rotation);

			locator_m.inverse();

			locator_m[0][0] = locator_m[1][1] = locator_m[2][2] = locator_m[3][3] = 1.0f;
			locator_m[0][1] = locator_m[0][2] = locator_m[0][3] = 0.0f;
			locator_m[1][0] = locator_m[1][2] = locator_m[1][3] = 0.0f;
			locator_m[2][0] = locator_m[2][1] = locator_m[2][3] = 0.0f;
			locator_m[3][0] *= -1.0f;
			locator_m[3][1] *= -1.0f;
			locator_m[3][2] *= -1.0f;

			locator_m = local_rotation.transpose() * locator_m;
			//rotation component will be stored
			locator_m.get(locator_mtx);

			//set only position

			Log("LOCATOR\n");
			for (long ry = 0; ry < 4; ry++)
			{
				for (long rx = 0; rx < 4; rx++)
					Log("%f, ", locator_mtx[ry][rx]);
				Log("\n");
			}
			Log("\n");


			for (; nummat > 0; nummat--)
				material[nummat - 1].MO = 0;

			mJointsList.clear();
			Traverse(object_path);

			//------------------------------------------------------------------------------
			rexp->WriteFile();

			STARTUPINFOA si;
			memset(&si, 0, sizeof si);
			si.cb = sizeof si;

			PROCESS_INFORMATION pi;
			memset(&pi, 0, sizeof pi);


			BOOL created = false;
			DWORD err;
			if (ExportCharacterPatch)
			{
				char cd[_MAX_PATH];
				MakePathForTraceMaker(cd);

				auto arg = std::string(cd) + " " + gm_path;
				created = CreateProcessA(cd, arg.data(), 0, 0, FALSE, /*CREATE_NEW_CONSOLE*/
				                         DETACHED_PROCESS | NORMAL_PRIORITY_CLASS, 0, 0, &si, &pi);
				err = GetLastError();
			}
			delete rexp;
			//------------------------------------------------------------------------------

#ifdef LOG
			fclose(LogFile);
#endif
		}
	}
	//fclose(trace);
	MGlobal::executeCommand("deleteUI -window StatusWin;");
}
