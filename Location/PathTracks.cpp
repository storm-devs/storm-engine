//===========================================================================================================================
// Spirenkov Maxim, 2003
//===========================================================================================================================//	
// 
//===========================================================================================================================
// PathTracks	
//============================================================================================
			

#include "PathTracks.h"

//============================================================================================

PathTracks::PathTracks()
{
	point = null;
	numPoints = 0;
}

PathTracks::~PathTracks()
{
	if(point) delete point;
	point = null;
	numPoints = 0;
}


//============================================================================================

//Загрузить трек в PathTracks
bool PathTracks::Load(const char * fileName)
{
	//Загружаем файл в память
	Assert(sizeof(AntFileTrackElement) == sizeof(Point));

	if(point) delete point;
	point = 0;
	numPoints = 0;

	char* data = null;
	dword size = 0;
	if( api->fio->LoadFile(fileName,&data,&size)==FALSE || !data )
	{
		api->Trace("Camera tracks file %s not loaded...", fileName);
		return false;
	}
	//Проверяем заголовок
	if(((AntFileHeader *)data)->id != ANTFILE_ID)
	{
		api->Trace("Camera tracks file %s is invalidate...", fileName);
		delete data;
		return false;
	}
	if(((AntFileHeader *)data)->ver != ANTFILE_VER)
	{
		api->Trace("Camera tracks file %s have incorrect version...", fileName);
		delete data;
		return false;
	}
	long nPoints = ((AntFileHeader *)data)->framesCount;
	long nStringSize = ((AntFileHeader *)data)->stringsTableSize;
	long nBoneCount = ((AntFileHeader *)data)->bonesCount;
	//Проверяем размеры файла
	if(size < sizeof(AntFileHeader) + sizeof(char)*nStringSize + sizeof(AntFileBone)*nBoneCount + sizeof(AntFileTrackElement)*nPoints)
	{
		api->Trace("Camera tracks file %s is invalidate...", fileName);
		delete data;
		return false;
	}
	//Сохраняем данные
	point = NEW Point[nPoints];
	Assert( point );
	memcpy(point, (byte *)data + sizeof(AntFileHeader) + nStringSize + sizeof(AntFileBone)*nBoneCount, sizeof(AntFileTrackElement)*nPoints);
	numPoints = nPoints;

	return true;
}

//Нарисовать трек
void PathTracks::Draw(VDX8RENDER* render)
{
	/*render.FlushBufferedLines();
	for(long i = 0; i < numPoints - 1; i++)
	{
		render.DrawBufferedLine(point[i].p1, 0xff00ff00, point[i + 1].p1, 0xff00ffff, false);
		render.DrawBufferedLine(point[i].p2, 0xff00ff00, point[i + 1].p2, 0xff00ffff, false);
		render.DrawBufferedLine(point[i].p, 0xffff0000, point[i + 1].p, 0xffffff00, false);
		render.DrawBufferedLine(point[i].p1, 0xffffff00, point[i].p2, 0xffffff00, false);
	}
	render.FlushBufferedLines();*/
}

//Получить точку трека
bool PathTracks::GetPoint(float index, Vector & cp, Quaternion & cq)
{
	Assert(point);
	if(index < 0.0 || index >= 1.f) return false;
	index *= numPoints;
	long i1 = long(index);
	long i2 = i1 + 1;
	if(i1 >= numPoints) i1 = numPoints - 1;
	if(i2 >= numPoints) i2 = numPoints - 1;
	cp.Lerp(point[i1].p, point[i2].p, index - i1);
	cq.SLerp(point[i1].q, point[i2].q, index - i1);
	return true;
}
