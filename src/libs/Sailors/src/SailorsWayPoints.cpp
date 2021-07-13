#include "SailorsWayPoints.h"
#include <d3dx9math.h>

#include "core.h"

#include "vfile_service.h"
#include "vmodule_api.h"

//--------------------------------------------------------------------------------------------------------------

bool Point::IsMast() const
{
    return (pointType == PT_TYPE_MAST_1 || pointType == PT_TYPE_MAST_2 || pointType == PT_TYPE_MAST_3 ||
            pointType == PT_TYPE_MAST_4 || pointType == PT_TYPE_MAST_5);
};

//--------------------------------------------------------------------------------------------------------------

bool Point::IsCannon() const
{
    return (pointType == PT_TYPE_CANNON_L || pointType == PT_TYPE_CANNON_R || pointType == PT_TYPE_CANNON_F ||
            pointType == PT_TYPE_CANNON_B);
};

//--------------------------------------------------------------------------------------------------------------

bool Point::IsNormal() const
{
    return (pointType == PT_TYPE_NORMAL);
};

//--------------------------------------------------------------------------------------------------------------

void Links::Add()
{
    link.push_back(Link{});
    count++;
};

//--------------------------------------------------------------------------------------------------------------

void Links::Delete(int Index)
{
    if (Index < 0 || Index >= count)
        return;

    link.erase(link.begin() + Index);

    count--;
    if (selected >= count)
        selected--;
};

//--------------------------------------------------------------------------------------------------------------

void Points::Add()
{
    point.push_back(Point{});
    count++;
};

//--------------------------------------------------------------------------------------------------------------

void Points::Delete(int Index)
{
    if (Index < 0 || Index >= count)
        return;

    point.erase(point.begin() + Index);
    count--;

    if (selected >= count)
        selected--;
};

//--------------------------------------------------------------------------------------------------------------

void SailorsPoints::Draw(VDX9RENDER *rs, bool pointmode)
{
    // D3DXVec3TransformCoord(0,0,0);

    if (!points.count)
        return;

    RS_RECT pRSR;
    pRSR.fSize = 1;
    pRSR.fAngle = PI * 2.0f * rand() / RAND_MAX;

    for (auto i = 0; i < points.count; i++)
    {
        pRSR.vPos = CVECTOR(points.point[i].x, points.point[i].y, points.point[i].z);

        pRSR.fSize = 0.15f;
        if (pointmode)
        {
            pRSR.dwColor = COLOR_SHADOW;
        }
        else
            pRSR.dwColor = COLOR_SHADOW_SELECTED;

        rs->DrawRects(&pRSR, 1, "EditorBack");

        pRSR.fSize = 0.15f;
        pRSR.dwColor = COLOR_POINT;

        if (pointmode)
            rs->DrawRects(&pRSR, 1, "EditorFront");
    }

    if (points.selected >= 0 && pointmode)
    {
        pRSR.vPos =
            CVECTOR(points.point[points.selected].x, points.point[points.selected].y, points.point[points.selected].z);

        pRSR.fSize = 0.25f;
        pRSR.dwColor = COLOR_SHADOW;
        rs->DrawRects(&pRSR, 1, "EditorBack");

        pRSR.fSize = 0.20f;
        pRSR.dwColor = COLOR_SHADOW_SELECTED;
        rs->DrawRects(&pRSR, 1, "EditorBack");

        pRSR.fSize = 0.20f;
        pRSR.dwColor = COLOR_SELECTED;

        rs->DrawRects(&pRSR, 1, "EditorFront");

        //        rs->DrawSphere(CVECTOR(points.point[points.selected].x,points.point[points.selected].y,points.point[points.selected].z),0.5f,0xFFFFFFFF);
    }

    if (!pointmode)
        return;

    CVECTOR _v2, _v1;
    RS_LINE pRSL[2];

    rs->SetTransform(D3DTS_WORLD, CMatrix());

    for (auto m = 0; m < links.count; m++)
    {
        _v1 = CVECTOR(points.point[links.link[m].first].x, points.point[links.link[m].first].y,
                      points.point[links.link[m].first].z);
        _v2 = CVECTOR(points.point[links.link[m].next].x, points.point[links.link[m].next].y,
                      points.point[links.link[m].next].z);

        pRSL[0].vPos = _v1;
        pRSL[1].vPos = _v2;

        pRSL[0].dwColor = COLOR_SHADOW;
        pRSL[1].dwColor = COLOR_SHADOW;

        rs->DrawLines(&pRSL[0], 1, "EditorBack");

        pRSL[0].dwColor = COLOR_GRAY;
        pRSL[1].dwColor = COLOR_GRAY;

        rs->DrawLines(&pRSL[0], 1, "EditorFront");
    }
};

//--------------------------------------------------------------------------------------------------------------
void SailorsPoints::Draw_(VDX9RENDER *rs, bool pointmode)
{
    if (!points.count || !links.count)
        return;

    RS_RECT pRSR;
    pRSR.fSize = 1;
    pRSR.fAngle = 0; // PI*2.0f*rand()/RAND_MAX;

    for (auto i = 0; i < points.count; i++)
    {
        pRSR.vPos = CVECTOR(points.point[i].x, points.point[i].y, points.point[i].z);

        pRSR.fSize = 0.15f;
        pRSR.dwColor = COLOR_SHADOW;

        rs->DrawRects(&pRSR, 1, "EditorBack");

        pRSR.fSize = 0.15f;
        pRSR.dwColor = COLOR_GRAY;

        rs->DrawRects(&pRSR, 1, "EditorFront");
    }

    CVECTOR _v2, _v1;
    RS_LINE pRSL[2];

    rs->SetTransform(D3DTS_WORLD, CMatrix());

    for (auto m = 0; m < links.count; m++)
    {
        _v1 = CVECTOR(points.point[links.link[m].first].x, points.point[links.link[m].first].y,
                      points.point[links.link[m].first].z);
        _v2 = CVECTOR(points.point[links.link[m].next].x, points.point[links.link[m].next].y,
                      points.point[links.link[m].next].z);

        pRSL[0].vPos = _v1;
        pRSL[1].vPos = _v2;

        pRSL[0].dwColor = COLOR_SHADOW;
        pRSL[1].dwColor = COLOR_SHADOW;

        rs->DrawLines(&pRSL[0], 1, "EditorBack");

        pRSL[0].dwColor = COLOR_GRAY;
        pRSL[1].dwColor = COLOR_GRAY;

        rs->DrawLines(&pRSL[0], 1, "EditorFront");
    }
};

//--------------------------------------------------------------------------------------------------------------
void SailorsPoints::DrawLinks(VDX9RENDER *rs)
{
    CVECTOR _v2, _v1;
    RS_LINE pRSL[2];

    rs->SetTransform(D3DTS_WORLD, CMatrix());

    for (auto m = 0; m < links.count; m++)
    {
        _v1 = CVECTOR(points.point[links.link[m].first].x, points.point[links.link[m].first].y,
                      points.point[links.link[m].first].z);
        _v2 = CVECTOR(points.point[links.link[m].next].x, points.point[links.link[m].next].y,
                      points.point[links.link[m].next].z);

        pRSL[0].vPos = _v1;
        pRSL[1].vPos = _v2;

        pRSL[0].dwColor = COLOR_SHADOW;
        pRSL[1].dwColor = COLOR_SHADOW;

        rs->DrawLines(&pRSL[0], 1, "EditorBack");

        if (links.selected == m)
        {
            pRSL[0].dwColor = COLOR_SELECTED;
            pRSL[1].dwColor = COLOR_SELECTED;
            rs->DrawLines(&pRSL[0], 1, "EditorBack");

            pRSL[0].vPos.x += 0.1f;
            pRSL[1].vPos.x += 0.1f;
            pRSL[0].vPos.z += 0.1f;
            pRSL[1].vPos.z += 0.1f;

            rs->DrawLines(&pRSL[0], 1, "EditorBack");

            pRSL[0].vPos.y += 0.1f;
            pRSL[1].vPos.y += 0.1f;
            rs->DrawLines(&pRSL[0], 1, "EditorBack");
        }
        else
        {
            pRSL[0].dwColor = COLOR_POINT;
            pRSL[1].dwColor = COLOR_POINT;
            rs->DrawLines(&pRSL[0], 1, "EditorFront");
        }
    }
};

//--------------------------------------------------------------------------------------------------------------

Path SailorsPoints::getPath(int src, int dst, int l)
{
    Path mPath;
    Path x;

    mPath.min = -1;
    mPath.point[l] = src;

    if (src == dst)
    {
        mPath.length = l;
        mPath.min = 0;
        return mPath;
    }

    if (PointsPassed[src] == 1)
        return mPath;
    PointsPassed[src] = true;

    for (auto i = 0; i < points.count; i++)
    {
        if (matrix[src][i] == 0)
            continue;

        x = getPath(i, dst, l + 1);

        if (x.min == -1)
            continue;

        x.min += matrix[src][i];

        x.point[l] = src;

        if (mPath.min == -1 || x.min < mPath.min)
            mPath = x;
    }

    PointsPassed[src] = false;

    return mPath;
};

//--------------------------------------------------------------------------------------------------------------

Path SailorsPoints::findPath(Path &path, int from, int to)
{
    path = getPath(from, to, 0);

    if (path.min == -1)
    {
        path.length = 0;
    }
    else
    {
        path.length++;
        path.point[path.length - 1] = to;
    }

    return path;
};

//--------------------------------------------------------------------------------------------------------------

void SailorsPoints::UpdateLinks()
{
    for (auto m = 0; m < points.count; m++)
        for (auto i = 0; i < points.count; i++)
            matrix[i][m] = false;

    for (auto m = 0; m < points.count; m++)
        for (auto i = 0; i < points.count; i++)

            for (auto _l = 0; _l < links.count; _l++)
                if ( //! links.link[_l].disabled &&
                    ((links.link[_l].first == m && links.link[_l].next == i) ||
                     (links.link[_l].first == i && links.link[_l].next == m)))
                {
                    matrix[i][m] = Dest(CVECTOR(points.point[i].x, points.point[i].y, points.point[i].z),
                                        CVECTOR(points.point[m].x, points.point[m].y, points.point[m].z));
                    break;
                }
};
//--------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------
int SailorsPoints::WriteToFile(std::string fileName)
{
    // GUARD(SailorsPoints :: WriteToFile);

    auto pIni = fio->OpenIniFile(fileName.c_str());
    if (!pIni)
        pIni = fio->CreateIniFile(fileName.c_str(), false);

    if (!pIni)
    {
        core.Trace("Warning! Can`t open '%s' for write", fileName.c_str());
        return 0;
    }

    pIni->DeleteSection("SIZE");
    pIni->DeleteSection("POINT_DATA");
    pIni->DeleteSection("LINK_DATA");

    pIni->WriteLong("SIZE", "points", points.count);
    pIni->WriteLong("SIZE", "links", links.count);

    char buffer[256];
    char str[64];

    for (auto i = 0; i < points.count; i++)
    {
        sprintf_s(str, sizeof(str), "%s%d", "point ", i);
        sprintf_s(buffer, sizeof(buffer), "%f,%f,%f,%d", points.point[i].x, points.point[i].y, points.point[i].z,
                  points.point[i].pointType);
        pIni->WriteString("POINT_DATA", str, buffer);
    }

    for (auto i = 0; i < links.count; i++)
    {
        sprintf_s(str, sizeof(str), "%s%d", "link ", i);
        sprintf_s(buffer, sizeof(buffer), "%d,%d", links.link[i].first, links.link[i].next);
        pIni->WriteString("LINK_DATA", str, buffer);
    }

    return 0;
    // UNGUARD
};

//--------------------------------------------------------------------------------------------------------------

int SailorsPoints::ReadFromFile(std::string fileName)
{
    // GUARD(SailorsPoints :: ReadFromFile);

    char param[256];
    char str[64];

    auto pIni = fio->OpenIniFile(fileName.c_str());

    if (!pIni)
    {
        core.Trace("Sailors : Can`t open '%s'", fileName.c_str());
        return 0;
    }

    pIni->ReadString("SIZE", "points", param, sizeof(param) - 1);
    sscanf(param, "%d", &points.count);

    float x, y, z;
    uint32_t type;

    points.point.resize(points.count);
    for (auto i = 0; i < points.count; i++)
    {
        // points.point.Add();
        sprintf_s(str, sizeof(str), "%s%d", "point ", i);
        pIni->ReadString("POINT_DATA", str, param, sizeof(param) - 1);

        sscanf(param, "%f,%f,%f,%lu", &x, &y, &z, &type);

        points.point[i].x = x;
        points.point[i].y = y;
        points.point[i].z = z;
        points.point[i].pointType = static_cast<PointType>(type);
    }

    pIni->ReadString("SIZE", "links", param, sizeof(param) - 1);
    sscanf(param, "%d", &links.count);

    int first, next;

    links.link.resize(links.count);
    for (auto i = 0; i < links.count; i++)
    {
        // links.link.Add();

        sprintf_s(str, sizeof(str), "%s%d", "link ", i);
        pIni->ReadString("LINK_DATA", str, param, sizeof(param) - 1);

        sscanf(param, "%d,%d", &first, &next);

        links.link[i].first = first;
        links.link[i].next = next;
    }

    UpdateLinks();

    return 0;
    // UNGUARD
};

//--------------------------------------------------------------------------------------------------------------
