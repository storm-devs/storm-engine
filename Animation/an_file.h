
#ifndef _AN_FILE_H_
#define _AN_FILE_H_

//ANIMATION_HEADER
//long parentIndex[nJoints]
//CVECTOR jointPos[nJoints]

//Frames:
//CVECTOR rootPos[nFrames]
//CVECTOR jointAngle[nJoints][nFrames]

#pragma pack(push, 1)

namespace ANFILE
{
	struct HEADER
	{
		long nFrames;
		long nJoints;
		float framesPerSec;
	};

};

#pragma pack(pop)

#endif


