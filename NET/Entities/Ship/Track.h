#ifndef __NETSHIP_TRACKS_HPP
#define __NETSHIP_TRACKS_HPP

#include "..\..\common.h"

class NetShipTracks : public ENTITY
{
public:
	NetShipTracks();
	~NetShipTracks();

	bool Init();

	void Execute(dword dwDeltaTime);
	void Realize(dword dwDeltaTime);

	void AddShip(SHIP_BASE * pShip);
	void DelShip(SHIP_BASE * pShip);
	void ResetTrack(SHIP_BASE * pShip);

	dword AttributeChanged(ATTRIBUTES * pA);

private:
	class ShipTrack
	{
	public:
		static VDX8RENDER * pRS;
		static SEA_BASE * pSea;
		static long iVTmpBuffer1, iVTmpBuffer2;
		static long iITmpBuffer1, iITmpBuffer2;
		static long iRefCount;
		static dword dwMaxBufferSize1, dwMaxBufferSize2;

		SHIP_BASE * pShip;

		ShipTrack();
		~ShipTrack();

		bool Update(SHIP_BASE * pShip);
		void Reset();

		void Execute(float fDeltaTime);
		void Realize(float fDeltaTime);

	private:
		struct Track
		{
			CVECTOR vPos;
			float fCos, fSin;
			float fTime;
			float fTV;
			float fAlpha, fInitialAlpha;
			float fWidth;
			float fSpeed;
		};

		struct TrackVertex
		{
			CVECTOR vPos;
			dword dwColor;
			float tu, tv;
		};

		bool bFirstExecute;

		float fCurTV;
		CVECTOR vLastPos, vLastAng;

		array<Track> aTrack1, aTrack2;
		long iTrackTexture1, iTrackTexture2;
		float fWidth11, fWidth12, fWidth21, fWidth22;
		float fSpeed11, fSpeed12, fSpeed21, fSpeed22;
		float fLifeTime1, fLifeTime2;
		float fZStart1, fZStart2;
		float fTrackStep1, fTrackStep2;
		dword dwTrackStep1, dwTrackStep2;
		float fUP1, fUP2;

		float fTrackDistance;

		bool Reserve1(dword dwSize);
		bool Reserve2(dword dwSize);
	};

	array<ShipTrack*> aShips;
};

#endif