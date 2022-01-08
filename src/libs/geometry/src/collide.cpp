/******************************************************************************
File:  collide.cpp

Author:   Nick Chirkov
Copyright (C) 2000, 2001 Nick Chirkov

Comments:
trace and clip functions
******************************************************************************/
#include "geom.h"
#include <cstring>

//---------------------------------------------------------------------------
// Trace main procedure
//---------------------------------------------------------------------------
SAVAGE GEOM::_stack[256];

float GEOM::Trace(VERTEX &start, VERTEX &finish)
{
    if (!(rhead.flags & FLAGS_BSP_PRESENT))
        return 2.0f;
    src = DVECTOR(start.x, start.y, start.z);
    dst = DVECTOR(finish.x, finish.y, finish.z);

    double diss, dise, ssrc, sdst, dist;
    DVECTOR dirvec, tp, V, AV;
    BSP_NODE *second;
    BSP_NODE *node;
    SAVAGE *stack;
    unsigned char *pface;
    unsigned char t;

    diss = 0.0;
    dise = 1.0;
    dirvec = dst - src;
    node = sroot;
    stack = _stack - 1;

rec_loop:;

    ssrc = (src | node->norm) - node->pd;
    sdst = (dst | node->norm) - node->pd;

    // dist = ssrc/(ssrc - sdst);
    const auto d = ssrc - sdst;
    dist = ssrc / d;

    if ((diss > EPSILON && dist <= diss - EPSILON) || d == 0.0 || dist <= 0.0)
    // if(dist<=diss)
    {
        if (sdst < 0.0)
        {
            if (node->left == 0)
                goto rec_avoid;
            node = &sroot[node->node];
        }
        else if (node->right == 0)
            goto rec_avoid;
        else
            node = &sroot[node->node + node->right];
        goto rec_loop;
    }

    if ((dise < 1.0 - EPSILON && dist >= dise + EPSILON) || dist >= 1.0)
    // if(dist>=dise)
    {
        if (ssrc < 0.0) // left
        {
            if (node->left == 0)
                goto rec_avoid;
            node = &sroot[node->node];
        }
        else // right
            if (node->right == 0)
            goto rec_avoid;
        else
            node = &sroot[node->node + node->right];
        goto rec_loop;
    }
    //----------first test----------
    if (ssrc < 0.0f)
    {
        if (node->left != 0)
        {
            stack++;
            stack->node = node;
            stack->dise = dise;
            stack->dist = dist;
            if (node->right == 0)
                stack->second = nullptr;
            else
                stack->second = &sroot[node->node + node->right];
            dise = dist;
            node = &sroot[node->node];
            goto rec_loop;
        }
        if (node->right == 0)
            second = nullptr;
        else
            second = &sroot[node->node + node->right];
    }
    else if (node->right != 0)
    {
        stack++;
        stack->node = node;
        stack->dise = dise;
        stack->dist = dist;
        if (node->left == 0)
            stack->second = nullptr;
        else
            stack->second = &sroot[node->node];
        dise = dist;
        node = &sroot[node->node + node->right];
        goto rec_loop;
    }
    else if (node->left == 0)
        second = nullptr;
    else
        second = &sroot[node->node];

rec_return:;
    //----------middle test----------
    if (node->nfaces > 0)
    {
        t = node->nfaces;
        pface = (unsigned char *)&node->face;

    loop0:
        const auto face = (static_cast<int32_t>(*(pface + 2)) << 16) | (static_cast<int32_t>(*(pface + 1)) << 8) |
                          (static_cast<int32_t>(*(pface + 0)) << 0);
        int32_t vindex[3];
        vindex[0] = (btrg[face].vindex[0][0] << 0) | (btrg[face].vindex[0][1] << 8) | (btrg[face].vindex[0][2] << 16);
        vindex[1] = (btrg[face].vindex[1][0] << 0) | (btrg[face].vindex[1][1] << 8) | (btrg[face].vindex[1][2] << 16);
        vindex[2] = (btrg[face].vindex[2][0] << 0) | (btrg[face].vindex[2][1] << 8) | (btrg[face].vindex[2][2] << 16);

        // Tomas Moller and Ben Trumbore algorithm

        DVECTOR a = vrt[vindex[1]] - vrt[vindex[0]];
        DVECTOR b = vrt[vindex[2]] - vrt[vindex[0]];
        auto pvec = dirvec ^ b;
        const auto det = a | pvec;

        auto c = src - vrt[vindex[0]];
        const double U = c | pvec;
        const double V = dirvec | (c ^ a);

        if (det < 0.0)
        {
            if (U < 0.0f && U > det && V < 0.0f && U + V > det)
            {
                res_dist = static_cast<float>(dist);
                traceid = face;
                return res_dist;
            }
        }
        else if (U >= 0.0f && U <= det && V >= 0.0f && U + V <= det)
        {
            res_dist = static_cast<float>(dist);
            traceid = face;
            return res_dist;
        }

        if (--t > 0)
        {
            pface += 3;
            goto loop0;
        }
    }

    //----------last test----------
    if (second == nullptr)
    {
    rec_avoid:;
        if (stack < _stack)
        {
            traceid = -1;
            return 2.0f;
        }

        dise = stack->dise;
        node = stack->node;
        dist = stack->dist;
        second = stack->second;
        stack--;
        goto rec_return;
    }

    diss = dist;
    node = second;
    goto rec_loop;
}

//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
static CVECTOR poly[256];

int32_t ClipByPlane(const GEOS::PLANE &plane, int32_t n)
{
    int32_t inside = 0;
    CVECTOR cr0, cr1;
    int32_t i;
    for (i = 0; i < n; i++)
        if (plane.nrm.x * poly[i].x + plane.nrm.y * poly[i].y + plane.nrm.z * poly[i].z - plane.d < 0.0)
            inside++;
    if (inside == n || inside == 0)
        return inside;

    // needs to be clipped
    float sign[4];
    int32_t ii, i3, i4;
    for (i = 0; i < n; i++)
    {
        sign[0] = plane.nrm.x * poly[i].x + plane.nrm.y * poly[i].y + plane.nrm.z * poly[i].z - plane.d;
        if (sign[0] > 0.0) // outside!
        {
            i3 = i - 1;
            if (i3 < 0)
                i3 = n - 1;
            sign[1] = plane.nrm.x * poly[i3].x + plane.nrm.y * poly[i3].y + plane.nrm.z * poly[i3].z - plane.d;
            if (sign[1] > 0.0)
                continue; // outside!
            double k = sign[0] / (sign[0] - sign[1]);
            cr0.x = static_cast<float>(poly[i].x + k * (poly[i3].x - poly[i].x));
            cr0.y = static_cast<float>(poly[i].y + k * (poly[i3].y - poly[i].y));
            cr0.z = static_cast<float>(poly[i].z + k * (poly[i3].z - poly[i].z));

            ii = i + 1;
            if (ii >= n)
                ii = 0;
            if (plane.nrm.x * poly[ii].x + plane.nrm.y * poly[ii].y + plane.nrm.z * poly[ii].z - plane.d <= 0.0)
            // inside!
            {
                for (i3 = n; i3 > ii; i3--)
                    poly[i3] = poly[i3 - 1];
                if (i > ii)
                    i++;
                n++;
                goto ClipEPH;
            }
        ClipBPH:
            i3 = ii;
            ii++;
            if (ii >= n)
                ii = 0;
            if (plane.nrm.x * poly[ii].x + plane.nrm.y * poly[ii].y + plane.nrm.z * poly[ii].z - plane.d <= 0)
                goto ClipEPH; // inside!
            for (i4 = i3; i4 < n - 1; i4++)
                poly[i4] = poly[i4 + 1];
            n--;
            if (i3 < i)
                i--;
            if (i3 < ii)
                ii--;
            goto ClipBPH;
        ClipEPH:
            i3 = ii - 1;
            if (i3 < 0)
                i3 = n - 1;

            sign[0] = plane.nrm.x * poly[ii].x + plane.nrm.y * poly[ii].y + plane.nrm.z * poly[ii].z - plane.d;
            sign[1] = plane.nrm.x * poly[i3].x + plane.nrm.y * poly[i3].y + plane.nrm.z * poly[i3].z - plane.d;
            k = sign[0] / (sign[0] - sign[1]);
            cr1.x = static_cast<float>(poly[ii].x + k * (poly[i3].x - poly[ii].x));
            cr1.y = static_cast<float>(poly[ii].y + k * (poly[i3].y - poly[ii].y));
            cr1.z = static_cast<float>(poly[ii].z + k * (poly[i3].z - poly[ii].z));

            poly[i] = cr0;
            i3 = i + 1;
            if (i3 >= n)
                i3 = 0;
            poly[i3] = cr1;
            break;
        }
    }
    return n;
}

//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
static int32_t VOL[256];
static unsigned short attempt = 65535;
static unsigned short trgclip[65536 * 4];

bool GEOM::Clip(const PLANE *planes, int32_t nplanes, const VERTEX &center, float radius, ADD_POLYGON_FUNC addpoly)
{
    if (!(rhead.flags & FLAGS_BSP_PRESENT))
        return false;
    const DVECTOR src(center.x, center.y, center.z);

    attempt++;
    if (attempt == 0)
    {
        memset(trgclip, 0, sizeof(trgclip));
        attempt++;
    }

    BSP_NODE *node = sroot;
    int32_t rec_level = -1;

rec_loop:;

    const double ssrc = (src | node->norm) - node->pd;

    // trace all faces
    if (ssrc * ssrc < radius * radius)
    {
        auto *pface = (unsigned char *)&node->face;
        for (uint32_t f = 0; f < node->nfaces; f++)
        {
            const int32_t face = (static_cast<int32_t>(*(pface + 2)) << 16) | (static_cast<int32_t>(*(pface + 1)) << 8) |
                              (static_cast<int32_t>(*(pface + 0)) << 0);
            // "fix" for broken models
            if (face < 0 || face >= sizeof(trgclip) / sizeof(*trgclip))
            {
                continue;
            }

            // this triangle was added before
            if (trgclip[face] != attempt)
            {
                // mark the triangle added
                trgclip[face] = attempt;

                // copy vertices to poly container
                for (int32_t v = 0; v < 3; v++)
                {
                    const int32_t vindex = (btrg[face].vindex[v][0] << 0) | (btrg[face].vindex[v][1] << 8) |
                                        (btrg[face].vindex[v][2] << 16);
                    memcpy(&poly[v], &vrt[vindex], sizeof(CVECTOR));
                }

                // clip polygon by planes
                int32_t nverts = 3;
                for (int32_t p = 0; p < nplanes; p++)
                {
                    nverts = ClipByPlane(planes[p], nverts);
                    if (nverts == 0)
                        break;
                }

                // add polygon
                if (nverts > 0)
                {
                    traceid = face;
                    // if test only
                    if (addpoly == nullptr)
                        return true;
                    // if clipping
                    if (addpoly((VERTEX *)&poly[0], nverts) == false)
                        return true;
                }
            }

            pface += 3;
        }

        // trace to side
        if (node->left != 0)
            VOL[++rec_level] = node->node;
        if (node->right == 0)
            goto rec_continue;
        node = &sroot[node->node + node->right];
    }
    else
    {
        if (ssrc < 0.0f)
        {
            if (node->left == 0)
                goto rec_continue;
            node = &sroot[node->node];
        }
        else if (node->right == 0)
            goto rec_continue;
        else
            node = &sroot[node->node + node->right];
    }

    goto rec_loop;
rec_continue:;
    if (rec_level > -1)
    {
        node = &sroot[VOL[rec_level--]];
        goto rec_loop;
    }
    if (rec_level > -1)
        goto rec_continue;

    return false;
}
