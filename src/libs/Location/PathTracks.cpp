//===========================================================================================================================
// Spirenkov Maxim, 2003
//===========================================================================================================================//
//
//===========================================================================================================================
// PathTracks
//============================================================================================

#include "PathTracks.h"

#include "core.h"

#include "CameraTracksFile.h"
#include "vfile_service.h"
#include "vmodule_api.h"

//============================================================================================

PathTracks::PathTracks()
{
    point = nullptr;
    numPoints = 0;
}

PathTracks::~PathTracks()
{
    delete point;
    point = nullptr;
    numPoints = 0;
}

//============================================================================================

// Upload track to PathTracks
bool PathTracks::Load(const char *fileName)
{
    // Loading the file into memory
    Assert(sizeof(AntFileTrackElement) == sizeof(Point));

    delete point;
    point = nullptr;
    numPoints = 0;

    char *data = nullptr;
    uint32_t size = 0;
    if (fio->LoadFile(fileName, &data, &size) == FALSE || !data)
    {
        core.Trace("Camera tracks file %s not loaded...", fileName);
        return false;
    }
    // Checking the title
    if (((AntFileHeader *)data)->id != ANTFILE_ID)
    {
        core.Trace("Camera tracks file %s is invalidate...", fileName);
        delete data;
        return false;
    }
    if (((AntFileHeader *)data)->ver != ANTFILE_VER)
    {
        core.Trace("Camera tracks file %s have incorrect version...", fileName);
        delete data;
        return false;
    }
    const long nPoints = ((AntFileHeader *)data)->framesCount;
    const long nStringSize = ((AntFileHeader *)data)->stringsTableSize;
    const long nBoneCount = ((AntFileHeader *)data)->bonesCount;
    // Checking file sizes
    if (size < sizeof(AntFileHeader) + sizeof(char) * nStringSize + sizeof(AntFileBone) * nBoneCount +
                   sizeof(AntFileTrackElement) * nPoints)
    {
        core.Trace("Camera tracks file %s is invalidate...", fileName);
        delete data;
        return false;
    }
    // save the data
    point = new Point[nPoints];
    Assert(point);
    memcpy(point, (uint8_t *)data + sizeof(AntFileHeader) + nStringSize + sizeof(AntFileBone) * nBoneCount,
           sizeof(AntFileTrackElement) * nPoints);
    numPoints = nPoints;

    return true;
}

// Draw track
void PathTracks::Draw(VDX9RENDER *render)
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

// Get track point
bool PathTracks::GetPoint(float index, Vector &cp, Quaternion &cq) const
{
    Assert(point);
    if (index < 0.0 || index >= 1.f)
        return false;
    index *= numPoints;
    auto i1 = static_cast<long>(index);
    auto i2 = i1 + 1;
    if (i1 >= numPoints)
        i1 = numPoints - 1;
    if (i2 >= numPoints)
        i2 = numPoints - 1;
    cp.Lerp(point[i1].p, point[i2].p, index - i1);
    cq.SLerp(point[i1].q, point[i2].q, index - i1);
    return true;
}
