#ifndef _GRAPH_VERTEX_H_
#define _GRAPH_VERTEX_H_


struct GraphVertex
{
	union
	{
		struct 
		{
			float Time;
			float Val;
		};
		struct 
		{
			float x;
			float y;
		};
	};
 
 
};

#endif