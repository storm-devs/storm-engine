/******************************************************************************
File:  texture.h

Author:   Nick Chirkov
Copyright (C) 2000-2002 Nick Chirkov

Comments:
******************************************************************************/
#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#pragma pack(push)
#pragma pack(1)


struct RDF_TEXTURE
{
	long flags;
	long width, height;
	long nmips;
	long format;
	long texel_size;
	//palette;
	//data;
};

#pragma pack(pop)

#endif