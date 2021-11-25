#include <iostream>
#include <fstream>
#include <vector>

#include "Rdf.h"
#include "Cvector.h"
#include "collide.h"

using namespace std;

extern long nvrts, ntrgs;
extern BTRG* trg;
extern CVECTOR* vrt;
extern long ssize;
extern BSP_NODE* sroot;

struct VerticesBuffer {
    vector <RDF_VERTEX0> vrt0;
    vector <RDF_VERTEX1> vrt1;
};

void ReadFile(ifstream& fileS, void* s, streamsize count)
{
    fileS.read(reinterpret_cast<char*>(s), count);
}

int main(int argc, char** argv)
{
    if (argc >= 2) {
        cout << "BSP patch started" << '\n';

        string path = argv[1];

        ifstream file(path, std::ios::binary | std::ios::ate);
        streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        if (file.good())
        {
            auto col = COLLIDE();

            long atriangles = 0;

            RDF_HEAD rhead;
            ReadFile(file, &rhead, sizeof(RDF_HEAD));
            rhead.flags |= FLAGS_BSP_PRESENT;

            vector<char> globname(rhead.name_size);
            ReadFile(file, globname.data(), rhead.name_size);

            vector<long> names(rhead.names);
            ReadFile(file, names.data(), rhead.names * sizeof(long));

            vector<long> tname(rhead.ntextures);
            ReadFile(file, tname.data(), rhead.ntextures * sizeof(long));

            vector<RDF_MATERIAL> rmat(rhead.nmaterials);
            ReadFile(file, rmat.data(), sizeof(RDF_MATERIAL) * rhead.nmaterials);

            vector<RDF_LABEL> lab(rhead.nlabels);
            ReadFile(file, lab.data(), sizeof(RDF_LABEL) * rhead.nlabels);

            vector<RDF_OBJECT> obj(rhead.nobjects);
            ReadFile(file, obj.data(), sizeof(RDF_OBJECT) * rhead.nobjects);

            for (long i = 0; i < obj.size(); i++) {
                atriangles += obj[i].ntriangles;
                obj[i].atriangles = atriangles;
            }

            vector<RDF_TRIANGLE> triangles(rhead.ntriangles);
            ReadFile(file, triangles.data(), sizeof(RDF_TRIANGLE) * rhead.ntriangles);

            vector<RDF_VERTEXBUFF> rvb(rhead.nvrtbuffs);
            ReadFile(file, rvb.data(), rhead.nvrtbuffs * sizeof(RDF_VERTEXBUFF));

            vector<VerticesBuffer> vertices(rhead.nvrtbuffs);
            for (long v = 0; v < rhead.nvrtbuffs; v++)
            {
                auto currentBuffer = rvb[v];

                auto type = currentBuffer.type;
                auto stride = sizeof(RDF_VERTEX0) + (currentBuffer.type & 3) * 8 + (currentBuffer.type >> 2) * 8;
                auto size = currentBuffer.size;
                auto nverts = size / stride;

                if (type == 4) {
                    cout << "BSP not needed on animated models" << '\n';
                    return 0;
                }

                switch (type)
                {
                case 0: {
                    vector<RDF_VERTEX0> vrt(nverts);
                    ReadFile(file, vrt.data(), nverts * sizeof(RDF_VERTEX0));
                    vertices[v].vrt0 = vrt;
                    break;
                }
                case 1: {
                    vector<RDF_VERTEX1> vrt(nverts);
                    ReadFile(file, vrt.data(), nverts * sizeof(RDF_VERTEX1));
                    vertices[v].vrt1 = vrt;
                    break;
                }
                default:
                    break;
                }
            }

            for (long i = 0; i < rhead.nobjects; i++) {
                auto currentObj = obj[i];
                auto currentBuffer = rvb[currentObj.vertex_buff];

                auto currentFaces = vector<RDF_TRIANGLE>(triangles.begin() + currentObj.striangle, triangles.begin() + currentObj.striangle + currentObj.ntriangles);

                if (currentBuffer.type == 0) {
                    auto buf = vertices[currentObj.vertex_buff].vrt0;
                    auto currentVertices = vector<RDF_VERTEX0>(buf.begin() + currentObj.svertex, buf.begin() + currentObj.svertex + currentObj.nvertices);
                    col.AddMesh(currentVertices, currentVertices.size(), currentFaces.data(), currentFaces.size());
                }
                else {
                    auto buf = vertices[currentObj.vertex_buff].vrt1;
                    auto currentVertices = vector<RDF_VERTEX1>(buf.begin() + currentObj.svertex, buf.begin() + currentObj.svertex + currentObj.nvertices);
                    col.AddMesh(currentVertices, currentVertices.size(), currentFaces.data(), currentFaces.size());
                }
            }

            col.BuildBSP(true);

            ofstream resultfile;
            resultfile.open(path, ios::binary);

            resultfile.write((char*)&rhead, sizeof(RDF_HEAD));
            resultfile.write(globname.data(), rhead.name_size);
            resultfile.write((char*)names.data(), rhead.names * sizeof(long));
            resultfile.write((char*)tname.data(), rhead.ntextures * sizeof(long));
            resultfile.write((char*)rmat.data(), sizeof(RDF_MATERIAL) * rhead.nmaterials);
            resultfile.write((char*)lab.data(), sizeof(RDF_LABEL) * rhead.nlabels);
            resultfile.write((char*)obj.data(), sizeof(RDF_OBJECT) * rhead.nobjects);
            resultfile.write((char*)triangles.data(), sizeof(RDF_TRIANGLE) * rhead.ntriangles);

            resultfile.write((char*)rvb.data(), rhead.nvrtbuffs * sizeof(RDF_VERTEXBUFF));

            for (long v = 0; v < rhead.nvrtbuffs; v++)
            {
                auto currentBuffer = rvb[v];
                auto type = currentBuffer.type;
                auto size = currentBuffer.size;
                auto currentVerticesBuffer = vertices[v];

                if (type == 0) {
                    resultfile.write((char*)currentVerticesBuffer.vrt0.data(), size);
                }
                else {
                    resultfile.write((char*)currentVerticesBuffer.vrt1.data(), size);
                }
            }

            RDF_BSPHEAD bhead;
            bhead.nnodes = ssize;
            bhead.nvertices = nvrts;
            bhead.ntriangles = ntrgs;

            resultfile.write((char*)&bhead, sizeof(RDF_BSPHEAD));
            resultfile.write((char*)sroot, sizeof(BSP_NODE) * bhead.nnodes);
            resultfile.write((char*)&vrt[0], sizeof(RDF_BSPVERTEX) * nvrts);
            for (long t = 0; t < ntrgs; t++)
            {
                RDF_BSPTRIANGLE btr;
                for (long v = 0; v < 3; v++)
                {
                    btr.vindex[v][0] = (trg[t].vindex[v] >> 0) & 0xFF;
                    btr.vindex[v][1] = (trg[t].vindex[v] >> 8) & 0xFF;
                    btr.vindex[v][2] = (trg[t].vindex[v] >> 16) & 0xFF;
                }
                resultfile.write((char*)&btr, sizeof RDF_BSPTRIANGLE);
            }

            resultfile.close();

            cout << "BSP patch finished succesfully!" << '\n';
        }
        else {
            cout << "Error: file not good !" << '\n';
        }
    }
    else {
        cout << "Error: Path to model not set!" << '\n';
    }
}