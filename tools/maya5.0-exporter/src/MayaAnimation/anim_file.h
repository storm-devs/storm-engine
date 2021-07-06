#pragma once

//ANIMATION_HEADER
//long parentIndex[nJoints]
//CVECTOR jointPos[nJoints]

//Frames:
//CVECTOR rootPos[nFrames]
//CVECTOR jointAngle[nJoints][nFrames]

namespace ANIFILE
{
	struct HEADER
	{
		long nFrames;
		long nJoints;
		float framesPerSec;
	};

};