/******************************************************************************
File:  rdf_exp.cpp

Author:   Nick Chirkov
Copyright (C) 2000, 2001 Nick Chirkov

Comments:
geometry file format exporter
******************************************************************************/
#include "export.h"
#include "collide.h"

extern long nvrts, ntrgs;
extern BTRG *trg;
extern CVECTOR *vrt;
extern long ssize;
extern std::vector<CVECTOR> lvert;
extern std::vector<GEOMLINK> geolink;
extern BSP_NODE *sroot;
//---------------------------------------------------------------------------------
//create exporter
//---------------------------------------------------------------------------------

extern double straverse;
extern long smax_depth;
//---------------------------------------------------------------------------------
//init exporter
//---------------------------------------------------------------------------------
GEOM_STATIC::GEOM_STATIC(const std::string &fname,
						 TEXTURE_FUNC _tfn, RESULT_FUNC fnr, RESULT_FUNC fne,
						 float maxedge, float msize, long _flags)
						 : tfn(_tfn), fnresult(fnr), fnerror(fne), flags(_flags), max_elength(maxedge), merge_size(msize)
{
	if(_flags&ANIMATION)	animation=true;
	else
	{
		animation=false;
		(_flags&BUILD_BSP)!=0 ?	build_bsp=true : build_bsp=false;
	}

	//(_flags&PRODUCTION)!=0 ?	production=true : production=false;
	production=false;

	//(_flags&GEOACCESS)!=0 ?	geoaccess=true : geoaccess=false;

	FILE *fll = fopen("stat.txt", "a");
	fprintf(fll, "\n\n\n\n%s\n", fname.c_str());
	fclose(fll);

	fl = fopen(fname.c_str(), "w+b");
	if(fl==0)
	{
		fnerror(std::string("can't open file for writing"));
		return;
	}

	trgaddby2side=0;
	tot_triangles=0;
	tot_vertices=0;
	straverse = 0.0;

	merge_size = 1.1f;
}


//---------------------------------------------------------------------------------
//add triangle to current object
//---------------------------------------------------------------------------------
void GEOM_STATIC::AddObject(const OBJECT &obj)
{
	//invalid material
	if(obj.mtl.size()==0)
	{

		char er[1024];
		sprintf(er, "no materials in %s, %s", obj.group_name.c_str(), obj.name.c_str());
		fnerror(std::string(er));
		return;
	}

	//link materials
	long *mtlref = new long[obj.mtl.size()];
	for(long m=0; m<obj.mtl.size(); m++)
	{
		//look the same material already added
		long g;
		for(g=0; g<material.size(); g++)
		{
			//naming params
			//if(obj.mtl[m].group_name!=globname[material[g].group_name])	continue;
			//if(obj.mtl[m].name!=globname[material[g].name])	continue;

			//D3D params
			if(obj.mtl[m].diffuse!=material[g].diffuse)	continue;
			if(obj.mtl[m].specular!=material[g].specular)	continue;
			if(obj.mtl[m].gloss!=material[g].gloss)	continue;

			//other optical params
			if(obj.mtl[m].selfIllum!=material[g].selfIllum)	continue;

			long tx;
			for(tx=0; tx<4; tx++)
			{
				if(obj.mtl[m].texture[tx].type!=material[g].texture_type[tx])	break;

				if(obj.mtl[m].texture[tx].type!=TEXTURE_NONE && obj.mtl[m].texture[tx].name.size()>0)
					if(obj.mtl[m].texture[tx].name!=globname[texture[material[g].texture[tx]]])	break;
			}
			if(tx==4)	break;
		}
		//add new material
		if(g==material.size())
		{
			//-----------------fill material properties---------------
			RDF_MATERIAL mt;
			mt.group_name = FindGlobalName(obj.mtl[m].group_name);
			mt.name = FindGlobalName(obj.mtl[m].name);
			mt.diffuse = obj.mtl[m].diffuse;
			mt.specular = obj.mtl[m].specular;
			mt.gloss = obj.mtl[m].gloss;
			mt.selfIllum = obj.mtl[m].selfIllum;

			for(long tm=0; tm<4; tm++)
			{
				mt.texture_type[tm] = (RDF_TEXTURE_TYPE)obj.mtl[m].texture[tm].type;
				if(mt.texture_type[tm]!=TEXTURE_NONE)	mt.texture[tm] = AddTexture(obj.mtl[m].texture[tm]);
				else	mt.texture[tm] = -1;
			}
			//--------------------------------------------------------
			material.push_back(mt);
		}
		mtlref[m] = g;
	}

	//link objects by material
	long *objref = new long[obj.mtl.size()];
	for(long o=0; o<obj.mtl.size(); o++)
	{
		long g;
		//triangles can be linked to an existing object
		if(obj.flags&MERGE)
		{
			for(g=0; g<object.size(); g++)
				if(
					object[g].vertex.size()+3<=65536	&&	//vertex buffer max vertex count is 65536
					(object[g].flags&MERGE) &&
					mtlref[o]==object[g].mtl &&
					(obj.flags&(VISIBLE | STATIC_LIGHT_ENABLE | DINAMIC_LIGHT_ENABLE | CAST_SHADOWS_ENABLE | VERTEX_WEIGHT/* | CULLENABLE*/))==
					(object[g].flags&(VISIBLE | STATIC_LIGHT_ENABLE | DINAMIC_LIGHT_ENABLE | CAST_SHADOWS_ENABLE | VERTEX_WEIGHT/* | CULLENABLE*/))
					)break;
		}
		else	g=object.size();

		//add new object
		if(g==object.size())
		{
			EOBJECT eo;
			eo.flags = obj.flags;
			eo.group_name = FindGlobalName(obj.group_name);
			eo.name = FindGlobalName(obj.name);
			eo.mtl = mtlref[o];
			object.push_back(eo);
		}

		objref[o] = g;
	}
	delete mtlref;

	//now objref contails numbers of objects we will add triangles
	for(long t=0; t<obj.trg.size(); t++)
	{
		if(!animation)
		{
			DVECTOR v0(obj.trg[t].vrt[0].x, obj.trg[t].vrt[0].y, obj.trg[t].vrt[0].z);
			DVECTOR v1(obj.trg[t].vrt[1].x, obj.trg[t].vrt[1].y, obj.trg[t].vrt[1].z);
			DVECTOR v2(obj.trg[t].vrt[2].x, obj.trg[t].vrt[2].y, obj.trg[t].vrt[2].z);
			double square = sqrt(~(v1-v0^v2-v0));
			//do not add the triangle if it is too small
			if(square<MIN_SQUARE)	continue;
		}


		//invalid material
		if(obj.trg[t].material<0 || obj.trg[t].material>= obj.mtl.size())
		{

			char er[1024];
			sprintf(er, "mtl in triangle #:%lu, %s, %s is %d", t, obj.group_name.c_str(), obj.name.c_str(), obj.trg[t].material);
			fnerror(std::string(er));
			continue;
		}

		EOBJECT *eo = &object[objref[obj.trg[t].material]];
		VERTEX0 *ver[3];
		long vb;
		if(material[eo->mtl].texture_type[3]!=TEXTURE_NONE)	vb=3;
		else
			if(material[eo->mtl].texture_type[2]!=TEXTURE_NONE)	vb=2;
			else
				if(material[eo->mtl].texture_type[1]!=TEXTURE_NONE)	vb=1;
				else	if(material[eo->mtl].texture_type[0]!=TEXTURE_NONE)	vb=0;
							else	continue;	//if incorrect material
		if(animation)	vb += 4;

		for(long v=0; v<3; v++)
		{
			switch(vb)
			{
				case 0:	ver[v] = new VERTEX0(obj.trg[t].vrt[v]);	break;
				case 1:	ver[v] = new VERTEX1(obj.trg[t].vrt[v]);	break;
				case 2:	ver[v] = new VERTEX2(obj.trg[t].vrt[v]);	break;
				case 3:	ver[v] = new VERTEX3(obj.trg[t].vrt[v]);	break;
				//animation
				case 4:	ver[v] = new VERTEXA0(obj.trg[t].vrt[v]);	break;
			}
		}


		//add new object if vertex count is 65536 or more
		if(eo->vertex.size()+3>65536)
		{
			//replace all references to this object by last object
			EOBJECT newo;
			newo.flags = eo->flags;
			newo.group_name = eo->group_name;
			newo.name = eo->name;
			newo.mtl = eo->mtl;
			object.push_back(newo);

			objref[obj.trg[t].material] = object.size()-1;
			eo = &object[objref[obj.trg[t].material]];
		}
		eo->AddTriangle(ver);

		//dublicate triangle
		if((obj.flags&CULLENABLE)==0)
		{
			trgaddby2side++;

			for(long v=0; v<3; v++)
			{
				switch(vb)
				{
					case 0:	ver[v] = new VERTEX0(obj.trg[t].vrt[v]);	break;
					case 1:	ver[v] = new VERTEX1(obj.trg[t].vrt[v]);	break;
					case 2:	ver[v] = new VERTEX2(obj.trg[t].vrt[v]);	break;
					case 3:	ver[v] = new VERTEX3(obj.trg[t].vrt[v]);	break;
					//animation
					case 4:	ver[v] = new VERTEXA0(obj.trg[t].vrt[v]);	break;
				}
				ver[v]->norm = -ver[v]->norm;
				ver[v]->pos += 1e-2f*ver[v]->norm;
			}


			//cull mode
			VERTEX0 *dvr = ver[0];
			ver[0] = ver[1];
			ver[1] = dvr;

			//add new object if vertex count is 65536 or more
			if(eo->vertex.size()+3>65536)
			{
				//replace all references to this object by last object
				EOBJECT newo;
				newo.flags = eo->flags;
				newo.group_name = eo->group_name;
				newo.name = eo->name;
				newo.mtl = eo->mtl;
				object.push_back(newo);

				objref[obj.trg[t].material] = object.size()-1;
				eo = &object[objref[obj.trg[t].material]];
			}
			//move this triangle by norm to avoid coplanar
			eo->AddTriangle(ver);
		}

	}

	delete objref;
}

//---------------------------------------------------------------------------------
//add light source
//---------------------------------------------------------------------------------
void GEOM_STATIC::AddLight(const LIGHT &l)
{
	RDF_LIGHT lt;
	lt.pos = CVECTOR(l.x, l.y, l.z);
	lt.dir = CVECTOR(l.nx, l.ny, l.nz);
	lt.atten[0] = l.atten[0];	lt.atten[1] = l.atten[1];	lt.atten[2] = l.atten[2];
	lt.r = l.r;	lt.g = l.g;	lt.b = l.b;
	lt.falloff = l.falloff;
	lt.inner = l.inner;
	lt.outer = l.outer;
	lt.type = (RDF_LIGHT_TYPE)l.type;
	lt.range = l.range;
	lt.flags = l.flags;
	lt.name = FindGlobalName(l.name);
	lt.group_name = FindGlobalName(l.group_name);

	light.push_back(lt);
}

GEOM_STATIC::~GEOM_STATIC()
{
}
//---------------------------------------------------------------------------------
//export procedure
//---------------------------------------------------------------------------------
void GEOM_STATIC::WriteFile()
{
	ssize = 0;

	RDF_HEAD rh;
	fseek(fl, sizeof RDF_HEAD, SEEK_SET);
	rh.version = RDF_VERSION;
	rh.flags = 0;
	rh.name_size = 0;
	rh.ntextures = texture.size();
	rh.nmaterials = material.size();
	rh.nlights = light.size();
	rh.nobjects = 0;
	rh.ntriangles = 0;
	rh.nvrtbuffs = 0;
	rh.nlabels = label.size();
	rh.names = globname.size();
	rh.bbox_size = rh.bbox_center = rh.radius = 0.0f;
	CVECTOR bmin = 1e20;
	CVECTOR bmax = -1e20;


	//write NAMES and build a lookup table
	std::vector<long> lookup;
	for(long n=0; n<globname.size(); n++)
	{
		const char *cstr = globname[n].c_str();
		long len = strlen(cstr);
		fwrite(cstr, len+1, 1, fl);
		lookup.push_back(rh.name_size);
		rh.name_size += len+1;
	}
	//names
	fwrite(&lookup[0], lookup.size(), sizeof(long), fl);

	//write textures
	long t;
	for(t=0; t<texture.size(); t++)
	{
		//export textures
		tfn(texid[t]);
		if(texture[t]<0 || texture[t]>=lookup.size())
		{
			fnerror(std::string("Texture name is invalid"));
			return;
		}
		fwrite((char*)&lookup[texture[t]], sizeof(long), 1, fl);
	}

	//write materials
	for(long m=0; m<material.size(); m++)
	{
		if(material[m].name<0 || material[m].name>=lookup.size())
		{
			fnerror(std::string("Material name is invalid"));
			return;
		}
		if(material[m].group_name<0 || material[m].group_name>=lookup.size())
		{
			fnerror(std::string("Material group_name is invalid"));
			return;
		}
		material[m].name = lookup[material[m].name];
		material[m].group_name = lookup[material[m].group_name];
		fwrite((char*)&material[m], sizeof RDF_MATERIAL, 1, fl);
	}

	//write lights
	for(long l=0; l<light.size(); l++)
	{
		if(light[l].name<0 || light[l].name>=lookup.size())
		{
			fnerror(std::string("Light name is invalid"));
			return;
		}
		if(light[l].group_name<0 || light[l].group_name>=lookup.size())
		{
			fnerror(std::string("Light group_name is invalid"));
			return;
		}
		light[l].name = lookup[light[l].name];
		light[l].group_name = lookup[light[l].group_name];
		fwrite((char*)&light[l], sizeof RDF_LIGHT, 1, fl);
	}

	//write labels
	for(long lb=0; lb<label.size(); lb++)
	{
		if(label[lb].name<0 || label[lb].name>=lookup.size())
		{
			fnerror(std::string("Label name is invalid"));
			return;
		}
		if(label[lb].group_name<0 || label[lb].group_name>=lookup.size())
		{
			fnerror(std::string("Label group_name is invalid"));
			return;
		}
		label[lb].group_name = lookup[label[lb].group_name];
		label[lb].name = lookup[label[lb].name];
		fwrite((char*)&label[lb], sizeof RDF_LABEL, 1, fl);
	}

	COLLIDE col(fnresult);

	//-------------------------------------------------------------------
	//calculate boxsize
	long o;
	for(o=0; o<object.size(); o++)
		for(long v=0; v<object[o].vertex.size(); v++)
		{
			bmin.x = min(bmin.x, object[o].vertex[v]->pos.x);
			bmin.y = min(bmin.y, object[o].vertex[v]->pos.y);
			bmin.z = min(bmin.z, object[o].vertex[v]->pos.z);
			bmax.x = max(bmax.x, object[o].vertex[v]->pos.x);
			bmax.y = max(bmax.y, object[o].vertex[v]->pos.y);
			bmax.z = max(bmax.z, object[o].vertex[v]->pos.z);
		}

	/*if(merge_size<1.0f)
	{
		merge_size *= 0.5f*sqrtf(~(bmax-bmin));
		//split objects
		for(o=0; o<object.size(); o++)
		{
			SplitObject(o);
			object[o].CalculateBoxRadius();
		}
	}*/

	//calculate vertex buffers info
	long start_vrt[8*8];	memset(start_vrt, 0, sizeof(start_vrt));
	long *obj2vb = new long[object.size()];
	for(o=0; o<object.size(); o++)
	{
		if(object[o].vertex.size()==0)	continue;

		long vertex_buff;
		if(material[object[o].mtl].texture_type[3]!=TEXTURE_NONE)	vertex_buff=3;
		else
			if(material[object[o].mtl].texture_type[2]!=TEXTURE_NONE)	vertex_buff=2;
			else
				if(material[object[o].mtl].texture_type[1]!=TEXTURE_NONE)	vertex_buff=1;
				else	vertex_buff=0;
		if(animation)	vertex_buff += 4;

		while(start_vrt[vertex_buff] + object[o].vertex.size()>65536)
			vertex_buff += 8;
		start_vrt[vertex_buff] += object[o].vertex.size();
		obj2vb[o] = vertex_buff;
	}

	rh.bbox_size = bmax-bmin;
	rh.bbox_center = 0.5f*(bmax+bmin);

	//write objects
	long xlat_vbr[8*8];
	long combuf=0;
	for(long vb=0; vb<8*8; vb++)
		if(start_vrt[vb]>0)
			xlat_vbr[vb] = combuf++;

	//-------------------------------------------------
	//write objects and add geometry to BSP
	long bsp_trg_offset = 0;
	bool bsp_compiled = false;
	memset(start_vrt, 0, sizeof(start_vrt));
	for(o=0; o<object.size(); o++)
	{
		if(object[o].vertex.size()==0)	continue;
		rh.nobjects++;
		RDF_OBJECT ro;
		if(object[o].group_name<0 || object[o].group_name>=lookup.size())
		{
			fnerror(std::string("Group name is invalid"));
			return;
		}
		if(object[o].name<0 || object[o].name>=lookup.size())
		{
			fnerror(std::string("Object name is invalid"));
			return;
		}
		ro.group_name = lookup[object[o].group_name];
		ro.name = lookup[object[o].name];
		ro.flags = object[o].flags;
		ro.material = object[o].mtl;
		ro.ntriangles = object[o].trg.size();
		ro.striangle = rh.ntriangles;
		rh.ntriangles += ro.ntriangles;
		object[o].CalculateBoxRadius();
		ro.center = CVECTOR(object[o].center.x, object[o].center.y, object[o].center.z);
		ro.radius = object[o].radius;

		ro.vertex_buff = xlat_vbr[obj2vb[o]];
		ro.nvertices = object[o].vertex.size();
		ro.svertex = start_vrt[obj2vb[o]];
		start_vrt[obj2vb[o]] += ro.nvertices;

		//collision enable
		if(build_bsp==true && (object[o].flags&COLLISION_ENABLE))
		{
			bsp_trg_offset += object[o].trg.size();
			col.AddMesh(o, object[o].vertex, object[o].vertex.size(), &object[o].trg[0], object[o].trg.size());
			bsp_compiled = true;

		}
		ro.atriangles = bsp_trg_offset;

		fwrite((char*)&ro, sizeof RDF_OBJECT, 1, fl);
	}
	build_bsp = bsp_compiled;

	//write triangles
	for(o=0; o<object.size(); o++)
	{
		if(object[o].vertex.size()==0)	continue;
		fwrite((char*)&object[o].trg[0], sizeof RDF_TRIANGLE, object[o].trg.size(), fl);
		tot_triangles += object[o].trg.size();
	}


	//write vertex buffers
	long vrb;
	for(vrb=0; vrb<8*8; vrb++)
		if(start_vrt[vrb]>0)
		{
			RDF_VERTEXBUFF vb;
			vb.type = vrb&7;
			long stride = sizeof(RDF_VERTEX0) + (vrb&3)*8 + ((vrb>>2)&1)*8;
			vb.size = start_vrt[vrb]*stride;
			fwrite((char*)&vb, sizeof RDF_VERTEXBUFF, 1, fl);
			rh.nvrtbuffs++;
		}

	//write vertices
	for(vrb=0; vrb<8*8; vrb++)
		for(o=0; o<object.size(); o++)
		{
			if(object[o].vertex.size()==0)	continue;
			long vb = obj2vb[o];
			if(vb!=vrb)	continue;

			//write portion of vertexbuffer
			for(long v=0; v<object[o].vertex.size(); v++)
			{
				char buf[sizeof(RDF_VERTEX3)];
				long size = object[o].vertex[v]->store(&buf[0]);
				fwrite(&buf[0], 1, size, fl);
				rh.radius = max(rh.radius, sqrtf(~(object[o].vertex[v]->pos - rh.bbox_center)));
			}
			tot_vertices += object[o].vertex.size();
		}
	delete[] obj2vb;

	FILE *fll = fopen("stat.txt", "a");
	fprintf(fll, "objects: %d, textures: %d, materials: %d\n", rh.nobjects, rh.ntextures, rh.nmaterials);
	fprintf(fll, "triangles: %d, vertices: %d\n", tot_triangles, tot_vertices);
	fprintf(fll, "2-sided triangles: %d\n", trgaddby2side);
	fprintf(fll, "-----------------------------------\n");
	fprintf(fll, "bbox_size: %f,%f,%f, radius: %f\n", rh.bbox_size.x, rh.bbox_size.y, rh.bbox_size.z, rh.radius);
	fprintf(fll, "-----------------------------------\n");
	fprintf(fll, "traverses/vertex: %f, max depth: %d\n", float(straverse/(tot_triangles*3.0)), smax_depth);

	fclose(fll);

	if(build_bsp==true)
	{
		rh.flags |= FLAGS_BSP_PRESENT;
		//build BSP
		col.BuildBSP(production);
		//write bsp header
		RDF_BSPHEAD bhead;
		bhead.nnodes = ssize;
		bhead.nvertices = nvrts;
		bhead.ntriangles = ntrgs;
		fwrite((char*)&bhead, sizeof RDF_BSPHEAD, 1, fl);
		fwrite(sroot, sizeof BSP_NODE, bhead.nnodes, fl);
	}
	//write vertices and triangles for BSP and geoaccess
	fwrite(&vrt[0], sizeof RDF_BSPVERTEX, nvrts, fl);
	for(t=0; t<ntrgs; t++)
	{
		RDF_BSPTRIANGLE btr;
		for(long v=0; v<3; v++)
		{
			btr.vindex[v][0] = (trg[t].vindex[v]>>0)&0xFF;
			btr.vindex[v][1] = (trg[t].vindex[v]>>8)&0xFF;
			btr.vindex[v][2] = (trg[t].vindex[v]>>16)&0xFF;
		}
		fwrite(&btr, sizeof RDF_BSPTRIANGLE, 1, fl);
	}


	//write header
	fseek(fl, 0, SEEK_SET);
	fwrite((char*)&rh, sizeof RDF_HEAD, 1, fl);

	fclose(fl);
}


long GEOM_STATIC::FindGlobalName(const std::string &str)
{
	//find existing string
	long s;
	for(s=0; s<globname.size(); s++)
		if(str==globname[s])	break;

	//add new string
	if(s==globname.size())	globname.push_back(str.c_str());
	return s;
}

long GEOM_STATIC::AddTexture(const TEXTURE &tex)
{
	//search texture
	long t;
	for(t=0; t<texture.size(); t++)
		if(tex.name==globname[texture[t]])
			break;
	//add new texture
	if(t==texture.size())
	{
		//find the same name
		long n;
		for(n=0; n<globname.size(); n++)
			if(tex.name==globname[n])
				break;
		//add new global name
		if(n==globname.size())
		{
			globname.push_back(tex.name.c_str());
			texid.push_back(tex.id);
		}
		texture.push_back(n);
	}

	return t;
}

void GEOM_STATIC::AddLabel(const LABEL &l)
{
	RDF_LABEL lt;
	lt.group_name = FindGlobalName(l.group_name);
	lt.name = FindGlobalName(l.name);
	memcpy(&lt.m[0][0], &l.vector[0][0], sizeof(l.vector));
	lt.flags = l.flags;
	for(long w=0; w<4; w++)
	{
		lt.weight[w] = l.weight[w];
		lt.bones[w] = l.bones[w];
	}
	label.push_back(lt);
}

void GEOM_STATIC::SplitObject(long o)
{
	object[o].CalculateBoxRadius();
	if(object[o].radius<=merge_size)	return;
	char *reftrg = new char[object[o].trg.size()];
	for(long t=0; t<object[o].trg.size(); t++)
	{
		//temporary calculate center of object
		CVECTOR bmin = 1e10f;
		CVECTOR bmax = -1e10f;
		long v;
		for(v=0; v<3; v++)
		{
			bmin.x = min(bmin.x, object[o].vertex[object[o].trg[t].vindex[v]]->pos.x);
			bmin.y = min(bmin.y, object[o].vertex[object[o].trg[t].vindex[v]]->pos.y);
			bmin.z = min(bmin.z, object[o].vertex[object[o].trg[t].vindex[v]]->pos.z);
			bmax.x = max(bmax.x, object[o].vertex[object[o].trg[t].vindex[v]]->pos.x);
			bmax.y = max(bmax.y, object[o].vertex[object[o].trg[t].vindex[v]]->pos.y);
			bmax.z = max(bmax.z, object[o].vertex[object[o].trg[t].vindex[v]]->pos.z);
		}
		CVECTOR cn = 0.5f*(bmax + bmin);

		//temporary calculate radius of object
		float rd = 0.0f;
		for(v=0; v<3; v++)
		{
			float rad = sqrtf(~(cn - object[o].vertex[object[o].trg[t].vindex[v]]->pos));
			if(rad>rd)	rd = rad;
		}

		if(rd>merge_size)	reftrg[t]=-1;
		else	reftrg[t]=1;
	}

	long removedtrg=0;
	long added_objs=0;
	for(;;)
	{
		EOBJECT eo;
		eo.flags = object[o].flags;
		eo.group_name = object[o].group_name;
		eo.name = object[o].name;
		eo.mtl = object[o].mtl;
		//add triangles to be in sphere with merge_size
		float radius = 0.0f;
		CVECTOR bn = 1e10f;
		CVECTOR bx = -1e10f;
		for(long t=0; t<object[o].trg.size(); t++)
		{
			if(reftrg[t]==0)	continue;

			//add too large triangle
			if(reftrg[t]==-1)
			{
				VERTEX0 *av[3];
				av[0] = object[o].vertex[object[o].trg[t].vindex[0]];
				av[1] = object[o].vertex[object[o].trg[t].vindex[1]];
				av[2] = object[o].vertex[object[o].trg[t].vindex[2]];
				eo.AddTriangle(av);
				reftrg[t] = 0;
				removedtrg++;
				continue;
			}
			//temporary calculate center of object
			CVECTOR bmin = bn;
			CVECTOR bmax = bx;
			long v;
			for(v=0; v<3; v++)
			{
				bmin.x = min(bmin.x, object[o].vertex[object[o].trg[t].vindex[v]]->pos.x);
				bmin.y = min(bmin.y, object[o].vertex[object[o].trg[t].vindex[v]]->pos.y);
				bmin.z = min(bmin.z, object[o].vertex[object[o].trg[t].vindex[v]]->pos.z);
				bmax.x = max(bmax.x, object[o].vertex[object[o].trg[t].vindex[v]]->pos.x);
				bmax.y = max(bmax.y, object[o].vertex[object[o].trg[t].vindex[v]]->pos.y);
				bmax.z = max(bmax.z, object[o].vertex[object[o].trg[t].vindex[v]]->pos.z);
			}
			CVECTOR cn = 0.5f*(bmax + bmin);

			//temporary calculate radius of object
			float rd = radius;
			for(v=0; v<3; v++)
			{
				float rad = sqrtf(~(cn - object[o].vertex[object[o].trg[t].vindex[v]]->pos));
				if(rad>rd)	rd = rad;
			}

			//if new size less than merge_size
			if(rd<=merge_size)
			{
				VERTEX0 *av[3];
				av[0] = object[o].vertex[object[o].trg[t].vindex[0]];
				av[1] = object[o].vertex[object[o].trg[t].vindex[1]];
				av[2] = object[o].vertex[object[o].trg[t].vindex[2]];
				eo.AddTriangle(av);
				radius = rd;
				bn = bmin;
				bx = bmax;
				reftrg[t] = 0;
				removedtrg++;
			}
		}
		if(removedtrg==object[o].trg.size())
		{
			object[o] = eo;
			break;
		}
		else
			object.push_back(eo);
		added_objs++;
	}

	delete reftrg;
}
