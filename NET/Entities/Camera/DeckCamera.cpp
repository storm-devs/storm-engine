#include <stdio.h>
#include "DeckCamera.h"

#define DISCR_F_VAL 0.00001f
#define EQU_FLOAT(x,y) (x)-(y)>DISCR_F_VAL ? false : (y)-(x)>DISCR_F_VAL ? false : true
#define BEF_FLOAT(x,y) (y)-(x)>=DISCR_F_VAL

NetDeckCamera::NetDeckCamera()
{
	vb_id = 0;
	ZeroMemory(&camera_pos,sizeof(camera_pos));
	camera_pos.y = 1.0f;
	ZeroMemory(&camera_ang,sizeof(camera_ang));
	pathNode = null;	

	fDelta = 0.0f;
	iCrosshairTex = -1;
	iTelescopeTex = -1;
	fCurrentPerspective = -1.0f;

	SetZoomed(false);
	SetOn(true);
	SetActive(true);
}

NetDeckCamera::~NetDeckCamera()
{
	Render().TextureRelease(iCrosshairTex); iCrosshairTex = -1;
	Render().TextureRelease(iTelescopeTex); iTelescopeTex = -1;
}

bool NetDeckCamera::Init()
{
	ENTITY_ID	sea_id;
	SEA_BASE	* pSea = null;

	if (NetFindClass(IsServer(), &sea_id, "NetSea")) pSea = (SEA_BASE*)api->GetEntityPointer(&sea_id);

	GUARD(NetDeckCamera::Init())
	api->SystemMessages(GetID(),true);
	SetDevice();

	if (pSea)
	{
		camera_pos.y = 10.0f + pSea->WaveXZ(camera_pos.x, camera_pos.z);
		Render().SetCamera(&camera_pos, &camera_ang, AttributesPointer->GetAttributeAsFloat("Perspective"));
	}

	UNGUARD
	return true;
}

void NetDeckCamera::SetDevice()
{
}

bool NetDeckCamera::CreateState(ENTITY_STATE_GEN * state_gen)
{
	return true;
}

bool NetDeckCamera::LoadState(ENTITY_STATE * state)
{
	SetDevice();
	return true;
}

void NetDeckCamera::Execute(dword Delta_Time)
{
	SetNetID((word)AttributesPointer->GetAttributeAsDword("NetID"));

	if (!isOn()) return;
	if (!FindShip() || !FindPath()) return;

	float fDeltaTime = float(Delta_Time) * 0.001f;

	float fStandartPerspective = AttributesPointer->GetAttributeAsFloat("Perspective");
	float fMinimumPerspective = AttributesPointer->GetAttributeAsFloat("ZoomPerspective");
	if (fCurrentPerspective < 0.0f) fCurrentPerspective = fStandartPerspective;
	
	CONTROL_STATE st;
	api->Controls->GetControlState("TelescopeIn", st);
	fCurrentPerspective += ((st.state == CST_ACTIVE) ? -1.0f : 1.0f) * 2.5f * fDeltaTime;
	fCurrentPerspective = Clampf(fCurrentPerspective, fMinimumPerspective, fStandartPerspective);
	SetZoomed(fabsf(fStandartPerspective - GetPerspective()) > 1e-5f);

	SetPerspective(fCurrentPerspective);

	Move(Delta_Time);
}

void NetDeckCamera::Realize(dword Delta_Time)
{
	if (!isOn()) return;

	CMatrix m;
	RS_RECT rCam;

	float fDeltaTime = float(Delta_Time) * 0.001f;

	Render().GetTransform(D3DTS_VIEW, m); m.Transposition(); CVECTOR vZ = m.Vz();

	ATTRIBUTES * pACamPos = AttributesPointer->FindAClass(AttributesPointer, "Pos"); Assert(pACamPos);
	ATTRIBUTES * pACamDir = AttributesPointer->FindAClass(AttributesPointer, "Dir"); Assert(pACamDir);

	pACamPos->SetAttributeUseFloat("x", m.Pos().x);
	pACamPos->SetAttributeUseFloat("y", m.Pos().y);
	pACamPos->SetAttributeUseFloat("z", m.Pos().z);

	pACamDir->SetAttributeUseFloat("x", vZ.x);
	pACamDir->SetAttributeUseFloat("y", vZ.y);
	pACamDir->SetAttributeUseFloat("z", vZ.z);

	bool bCanFire = AttributesPointer->GetAttributeAsDword("IsCanFire", 0) != 0;
	dword dwCurrentTargetRelation = AttributesPointer->GetAttributeAsDword("CurrentTargetRelation", 0);

	if (bCanFire && !IsZoomed())
	{
		if (dwCurrentTargetRelation > 0)
		{
			fDelta += fDeltaTime * 4.0f;
			while (fDelta > 2.0f) 
				fDelta -= float(long(fDelta / 2.0f) * 2);
		}
		else
			fDelta = 0.0f;

		rCam.dwColor = Colors[dwCurrentTargetRelation];

		rCam.fAngle = 0.0f;
		float fSizeMultiply = 0.9f + (0.2f * ((fDelta > 1.0f) ? fDelta : (2.0f - fDelta)) );
		//rCam.fSize = fSizeMultiply * fCrosshairSize;
		rCam.fSize = fSizeMultiply * fCrosshairSize;
		rCam.vPos = m * CVECTOR(0.0f, 0.0f, 1.0f);
		rCam.dwSubTexture = 0;

		Render().TextureSet(0, iCrosshairTex);
		Render().DrawRects(&rCam, 1, sCrosshairTechnique.GetBuffer(), dwCrosshairSubTexX, dwCrosshairSubTexY);
	}

	if (IsZoomed())
	{
		CVECTOR vSrc = m.Pos();
		CVECTOR vDst = m.Pos() + 4000.0 * vZ;

		/*ENTITY_ID eid;
		NetShip * pBestShip = null;
		float fDistance = 2.0f;
		if (NetFindClass(IsServer(), &eid, "NetShip")) do
		{
			NetShip * pShip = (NetShip*)eid.pointer;
			float fRes = pShip->Trace(vSrc, vDst);
			if (fRes < fDistance)
			{
				fDistance = fRes;
				pBestShip = pShip;
			}
		} while (NetFindClassNext(IsServer(), &eid));

		if (pBestShip)
		{
			ATTRIBUTES * pA = pBestShip->GetACharacter(); Assert(pA);

			string sName = pA->FindAClass(pA, "NickName")->GetThisAttr();
			float fHP = pA->FindAClass(pA, "Ship.HP")->GetAttributeAsFloat();
			float fCrew = pA->FindAClass(pA, "Ship.Crew.Quantity")->GetAttributeAsFloat();

			D3DVIEWPORT8 vp;
			Render().GetViewport(&vp);

			Render().ExtPrint(FONT_DEFAULT, 0xFFFFFFFF, 0x00000000, ALIGN_CENTER, 0, 0.85f, 0, 0, long(vp.Width) / 2, long(vp.Height) / 2 - vp.Height / 6, "%s, HP: %.1f, Crew: %.1f", sName.GetBuffer(), fHP, fCrew);
		}*/

		// Draw telescope texture
		D3DVIEWPORT8 vp; Render().GetViewport(&vp);
		float w = float(vp.Width);
		float h = float(vp.Height);
		RS_SPRITE spr[4];
		FillSpriteVertex(spr[0], 0, 0, 0.1f, 0xFFFFFFFF, 0.0f, 0.0f);
		FillSpriteVertex(spr[1], 0, h, 0.1f, 0xFFFFFFFF, 0.0f, 1.0f);
		FillSpriteVertex(spr[2], w, h, 0.1f, 0xFFFFFFFF, 1.0f, 1.0f);
		FillSpriteVertex(spr[3], w, 0, 0.1f, 0xFFFFFFFF, 1.0f, 0.0f);
		Render().TextureSet(0, iTelescopeTex);
		Render().DrawSprites(spr, 1, sTelescopeTechnique);
	}
}

void NetDeckCamera::Move(DWORD DeltaTime)
{
	if (!isActive()) return;

	pModel = (MODEL*)GetModelPointer();	Assert(pModel);

    pModel->Update();
	CONTROL_STATE cs;
	api->Controls->GetControlState("DeckCamera_Turn_H", cs);
	camera_ang.y += fSensivityAzimuthAngle * 3.0f * (float)(cs.fValue);

	api->Controls->GetControlState("DeckCamera_Left", cs);
	if(cs.state == CST_ACTIVE) 
		camera_ang.y -= fSensivityAzimuthAngle * 15.f * (float)(cs.fValue);

	api->Controls->GetControlState("DeckCamera_Right", cs);
	if (cs.state == CST_ACTIVE) 
		camera_ang.y += fSensivityAzimuthAngle * 15.f * (float)(cs.fValue);

	api->Controls->GetControlState("DeckCamera_Turn_V", cs);
	camera_ang.x -= fSensivityHeightAngle * 3.0f * (float)(cs.fValue);

    if (camera_ang.x>CAMERA_MAX_X) camera_ang.x=CAMERA_MAX_X;
    else if (camera_ang.x<CAMERA_MIN_X) camera_ang.x=CAMERA_MIN_X;
    if (camera_ang.y < 0.f) camera_ang.y+=PIm2;
    else if (camera_ang.y > PIm2) camera_ang.y-=PIm2;

    // определить реальный угол поворота камеры
	CMatrix glbRotMtx;
	glbRotMtx.BuildMatrix(camera_ang);
    CMatrix rotMtx;
    pathNode->glob_mtx.Get3X3(&rotMtx);
	glbRotMtx *= rotMtx;
    float cx=cosf(camera_ang.x); float sx=sinf(camera_ang.x);
    float cy=cosf(camera_ang.y); float sy=sinf(camera_ang.y);
	float xAng,yAng,zAng;
	CVECTOR v = glbRotMtx*CVECTOR(0,0,1.f);
	yAng = atan2f(v.x,v.z);
	glbRotMtx.RotateY(-yAng);
	v = glbRotMtx*CVECTOR(0,0,1.f);
	xAng = atan2f(-v.y,v.z);
	glbRotMtx.RotateX(-xAng);
	v = glbRotMtx*CVECTOR(0,1.f,0);
	zAng = atan2f(-v.x,v.y);
	CVECTOR s_ang;
	s_ang.x = xAng + (camera_ang.x - xAng) * (1.0f - fRockingX);
	s_ang.y = yAng;
	s_ang.z = zAng * fRockingZ;

    // узнать новую позицию камеры
    CVECTOR prev_pos = camera_pos;
	float speed0 = DeltaTime * fSensivityDistance;
    float speed = 0.f;
    CVECTOR vShift;
    vShift.x = cx * sy;
    vShift.y = 0.f;
    vShift.z = cx * cy;

	api->Controls->GetControlState("DeckCamera_Forward",cs);
	if(cs.state == CST_ACTIVE) speed=speed0;

	api->Controls->GetControlState("DeckCamera_Backward",cs);
    if(cs.state == CST_ACTIVE) 
    {
        speed=speed0;
        vShift.x *= -1.f;
        vShift.z *= -1.f;
    }

    if( speed!=0.f )
    {
        CVECTOR src,dst;
        src.y = 500.f; dst.y = -500.f;
        bool bNoFinded=true;
        CVECTOR vRes=camera_pos;
        float step=MEN_STEP_MIN;
        float len;
        CVECTOR prev_res=vRes;
        CVECTOR p1,p2;
        long trgNum=-1;
        while(1)
        {
            // определяем треугольник на котором мы находимся
            src.x = dst.x = vRes.x;
            src.z = dst.z = vRes.z;
            float htmp;
            htmp=500.f - 1000.f*MultiTrace(src,dst,camera_pos.y);
            if( htmp>=-500.f &&
                vRes.y-htmp<MEN_STEP_UP && htmp-vRes.y<MEN_STEP_UP )
            {
                if(!GetCrossXZ(vRes,vShift,(p1=g_gv0),(p2=g_gv1),vRes))
                    if(!GetCrossXZ(vRes,vShift,(p1=g_gv1),(p2=g_gv2),vRes))
                        if(!GetCrossXZ(vRes,vShift,(p1=g_gv2),(p2=g_gv0),vRes))
                         // нет пересечения со сторонами треугольника
                         break;

                len=sqrtf(~(vRes-prev_res));
                if(len>speed)
                {
                    vRes=prev_res+vShift*speed;
                    // определить высоту камеры
                    src.x=dst.x=vRes.x;
                    src.z=dst.z=vRes.z;
                    float tmp = MultiTrace(src,dst,camera_pos.y);//pathNode->geo->Trace(src,dst);
                    if(tmp<=1.f)
                    {
                        vRes.y = 500.f - 1000.f*tmp;
                        bNoFinded=false;
                    }
                    else
                        vRes=prev_res;
                    bNoFinded=false;
                    break;
                }
                else if(len>0.f)
                {
                    prev_res=vRes-vShift*.0001f;
                    vRes+=vShift*MEN_STEP_MIN;
                    bNoFinded=false;
                    speed-=len;
                }
                else
                {
                    vRes+=vShift*MEN_STEP_MIN;
                    bNoFinded=false;
                    break;
                }
            }
            else
            {
                // скольжение по треугольнику
                if(bNoFinded) break; // стоим там, где нет пола

                CVECTOR ep;
                CVECTOR dp;
                if(((p2-p1)|vShift)<0.f)
                {
                    ep=p1;
                    dp=p1-p2;
                }
                else
                {
                    ep=p2;
                    dp=p2-p1;
                }

                len= sqrtf(SQR(ep.x-prev_res.x)+SQR(ep.z-prev_res.z));
                if( len>0.f )
                    if( speed>=len )
                    {
                        vRes=ep+(!dp)*MEN_STEP_MIN;
                        src.x = dst.x = vRes.x;
                        src.z = dst.z = vRes.z;
                        speed-=len;
                        if(MultiTrace(src,dst,camera_pos.y)<=1.f)
                        {
                            prev_res=vRes;
                        }
                        else
                        {
                            vRes-=vShift*MEN_STEP_MIN;
                            src.x = dst.x = vRes.x;
                            src.z = dst.z = vRes.z;
                            if(MultiTrace(src,dst,camera_pos.y)>1.f)
                                break;
                        }
                    }
                    else
                    {
                        vRes=prev_res + (ep-prev_res)*(speed/len);
                        break;
                    }
                else  break;
            }
        }

        if(!bNoFinded)
        {
            // проверим на попадание камеры в приемлимое место
            src.x = dst.x = vRes.x;
            src.z = dst.z = vRes.z;
			float fTmp = MultiTrace(src,dst,camera_pos.y);
			//float fDist = 1000.f;
			//if(fTmp<=1.f) fDist = src.y + fTmp*(dst.y-src.y) - camera_pos.y;
            if(fTmp<=1.f)
                camera_pos = vRes;
            else
            {
                src.x = dst.x = prev_res.x;
                src.z = dst.z = prev_res.z;
                if(MultiTrace(src,dst,camera_pos.y)<=1.f)
                    camera_pos = prev_res;
            }
        }
    }

    // расчитать позицию камеры
    CVECTOR s_pos;
    s_pos = pathNode->glob_mtx*(camera_pos+CVECTOR(0.f,h_eye,0.f));

    // установить камеру
	Render().SetCamera(&s_pos,&s_ang,GetPerspective());
}

bool NetDeckCamera::FindPath()
{
	pModel = GetModelPointer(); Assert(pModel);

	NODE * pNewPathNode = pModel->FindNode("path"); Assert(pNewPathNode);
	
	if (pathNode != pNewPathNode)
	{
		pathNode = pNewPathNode;
		SetStartPos();
	}

	return true;
}

void NetDeckCamera::SetStartPos()
{
    if(pModel)
    {
        GEOS::VERTEX src,dst;

        src.y=500.f; dst.y=-500.f;

        camera_ang.x=0.f;
        camera_ang.y=0.f;
        camera_ang.z=0.f;

        camera_pos.x=0.f;
        camera_pos.z=0.f;
        camera_pos.y=-500.f;

		for(int i=0; i<1000; i++)
		{
			NODE* root = pModel->GetNode(i);
			if(root==NULL) break;
			GEOS::INFO gi;
			root->geo->GetInfo(gi);
			for(int j=0; j<gi.nlabels; j++)
			{
				GEOS::LABEL gl;
				root->geo->GetLabel(j,gl);
				if(!strncmp(gl.name,"camera",6))
				{
					camera_pos.x = gl.m[3][0];
					camera_pos.y = gl.m[3][1];
					camera_pos.z = gl.m[3][2];
					break;
				}
			}
			if(j<gi.nlabels)
			{
				pathNode->glob_mtx.MulToInv( root->glob_mtx*camera_pos, camera_pos );
				break;
			}
		}
        src.x=dst.x=camera_pos.x;
        src.z=dst.z=camera_pos.z;

        float tmp = pathNode->geo->Trace(src,dst);
        if( tmp<=1.f )
        {
            tmp = 500.f - tmp*1000.f;
            if(camera_pos.y==-500.f)
                h_eye=DEFAULT_HEIGHT;
            else
                h_eye = camera_pos.y - tmp;
            camera_pos.y = tmp;
            if( h_eye>HEIGHT_MAX )
                h_eye=HEIGHT_MAX;
            else if( h_eye<HEIGHT_MIN )
                h_eye=HEIGHT_MIN;
        }
		else
		{
			api->Trace("WARNING! Bad camera locator on ship %s", pModel->GetNode(0)->GetName() );
		}

        //SetCursorPos(lock_x,lock_y);
    }
}

bool NetDeckCamera::GetCrossXZ(CVECTOR &spos,CVECTOR &dv, CVECTOR &p1,CVECTOR &p2, CVECTOR &res)
{
    bool bNoCross=false;

    bool bXset=false;
    bool bZset=false;

    float xRes=0.f;
    float zRes=0.f;

    // проверка на dx=0 для направляющего вектора
    if( !bNoCross )
        if(EQU_FLOAT(dv.x,0.f))
            if(bXset) { if(spos.x!=xRes) bNoCross=true; }
            else {bXset=true; xRes=spos.x;}
    // проверка на dz=0 для направляющего вектора
    if( !bNoCross )
        if(EQU_FLOAT(dv.z,0.f))
            if(bZset) { if(spos.z!=zRes) bNoCross=true; }
            else {bZset=true; zRes=spos.z;}

    // проверка на dx=0 для отрезка
    if( !bNoCross )
        if(EQU_FLOAT(p1.x,p2.x))
            if(bXset) { if(p1.x!=xRes) bNoCross=true; }
            else {bXset=true; xRes=p1.x;}
    // проверка на dz=0 для отрезка
    if( !bNoCross )
        if(EQU_FLOAT(p1.z,p2.z))
            if(bZset) { if(p1.z!=zRes) bNoCross=true; }
            else {bZset=true; zRes=p1.z;}


    if( bNoCross ) return false; // нет пересечения
    if( bXset && bZset )
        if( (spos.x-p1.x)*(p2.z-p1.z)!=(p2.x-p1.x)*(spos.z-p1.z) )
            return false; // нет пересечения

    if(bXset)
        if(!bZset)
            if(dv.x!=0.f)
            {
                bZset=true; zRes=(xRes-spos.x)*dv.z/dv.x + spos.z;
            }
            else if(p1.x!=p2.x)
            {
                bZset=true; zRes=(xRes-p1.x)*(p2.z-p1.z)/(p2.x-p1.x) + p1.z;
            }
            else
            {
                bZset=true;
                zRes = dv.z<0.f ? p1.z<p2.z ? p1.z:p2.z : p1.z<p2.z ? p2.z:p1.z;
            }

    if(bZset)
        if(!bXset)
            if(dv.z!=0.f)
            {
                bXset=true; xRes=(zRes-spos.z)*dv.x/dv.z + spos.x;
            }
            else if(p1.z!=p2.z)
            {
                bXset=true; xRes=(zRes-p1.z)*(p2.x-p1.x)/(p2.z-p1.z) + p1.x;
            }
            else
            {
                bXset=true;
                xRes = dv.x<0.f ? p1.x<p2.x ? p1.x:p2.x : p1.x<p2.x ? p2.x:p1.x;
            }

    if(!bZset && !bXset)
    {
        xRes = ( p1.z-spos.z + spos.x*dv.z/dv.x - p1.x*(p2.z-p1.z)/(p2.x-p1.x) ) /
               ( dv.z/dv.x - (p2.z-p1.z)/(p2.x-p1.x) );
        zRes = spos.z + (xRes-spos.x)*dv.z/dv.x;
    }

    // проверка на нахождение точки пересечения на заданном отрезке
    // по X
    if(p1.x<p2.x) {if(xRes<p1.x || xRes>p2.x) return false;} // нет пересечения на отрезке
    else if(xRes<p2.x || xRes>p1.x) return false;
    // по Z
    if(p1.z<p2.z) {if(zRes<p1.z || zRes>p2.z) return false;}
    else if(zRes<p2.z || zRes>p1.z) return false;

    // вычислим координату Y из отрезка [P1,P2]
    float yRes;
    if(p1.x==p2.x)
        if(p1.z==p2.z)
            yRes = p1.y>p2.y ? p1.y : p2.y;
        else
            yRes = p1.y + (zRes-p1.z)*(p2.y-p1.y)/(p2.z-p1.z);
    else
        yRes = p1.y + (xRes-p1.x)*(p2.y-p1.y)/(p2.x-p1.x);

    if( (xRes>spos.x && dv.x<0.f) ||
        (xRes<spos.x && dv.x>0.f) ||
        (zRes>spos.z && dv.z<0.f) ||
        (zRes<spos.z && dv.z>0.f)  )
        return false;
    else
    {
        res.x=xRes; res.y=yRes; res.z=zRes;
        return true;
    }
}

dword NetDeckCamera::AttributeChanged(ATTRIBUTES * pAttr)
{
	ATTRIBUTES * pAParent = pAttr->GetParent();

	if (pAParent && *pAParent == "Telescope")
	{
		if (*pAttr == "Technique")	{ sTelescopeTechnique = pAttr->GetThisAttr(); return 0; }
		if (*pAttr == "Texture")	
		{ 
			if (iTelescopeTex >= 0) Render().TextureRelease(iTelescopeTex);
			iTelescopeTex = Render().TextureCreate(pAttr->GetThisAttr());
		}
	}

	if (pAParent && *pAParent == "Crosshair")
	{
		if (*pAttr == "Technique")	{ sCrosshairTechnique = pAttr->GetThisAttr(); return 0; }
		if (*pAttr == "SubTexX")	{ dwCrosshairSubTexX = pAttr->GetAttributeAsDword(); return 0; }
		if (*pAttr == "SubTexY")	{ dwCrosshairSubTexY = pAttr->GetAttributeAsDword(); return 0; }
		if (*pAttr == "Size")		{ fCrosshairSize = pAttr->GetAttributeAsFloat(); return 0; }
		if (*pAttr == "Texture")	
		{ 
			if (iCrosshairTex >= 0) Render().TextureRelease(iCrosshairTex);
			iCrosshairTex = Render().TextureCreate(pAttr->GetThisAttr());
			return 0; 
		}
		return 0;
	}

	if (pAParent && *pAParent == "Colors")
	{
		if (*pAttr == "Default")	{ Colors[0] = pAttr->GetAttributeAsDword(); return 0; }
		if (*pAttr == "Friend")		{ Colors[1] = pAttr->GetAttributeAsDword(); return 0; }
		if (*pAttr == "Enemy")		{ Colors[2] = pAttr->GetAttributeAsDword(); return 0; }
	}

	if (*pAttr == "SensivityDistance")		fSensivityDistance = pAttr->GetAttributeAsFloat();
	if (*pAttr == "SensivityHeightAngle")	fSensivityHeightAngle = pAttr->GetAttributeAsFloat();
	if (*pAttr == "SensivityAzimuthAngle")	fSensivityAzimuthAngle = pAttr->GetAttributeAsFloat();
	if (*pAttr == "StepUp")					MEN_STEP_UP = pAttr->GetAttributeAsFloat();
	if (*pAttr == "StepMin")				MEN_STEP_MIN = pAttr->GetAttributeAsFloat();
	if (*pAttr == "h_max")					HEIGHT_MAX = pAttr->GetAttributeAsFloat();
	if (*pAttr == "h_min")					HEIGHT_MIN = pAttr->GetAttributeAsFloat();
	if (*pAttr == "h_step")					HEIGHT_STEP = pAttr->GetAttributeAsFloat();
	if (*pAttr == "h_def")					DEFAULT_HEIGHT = pAttr->GetAttributeAsFloat();
	if (*pAttr == "MaxAngleX")				CAMERA_MAX_X = pAttr->GetAttributeAsFloat();
	if (*pAttr == "MinAngleX")				CAMERA_MIN_X = pAttr->GetAttributeAsFloat();
	if (*pAttr == "RockingX")				fRockingX = pAttr->GetAttributeAsFloat();
	if (*pAttr == "RockingZ")				fRockingZ = pAttr->GetAttributeAsFloat();

	if (*pAttr == "On")						SetOn(pAttr->GetAttributeAsDword() != 0);
	if (*pAttr == "Active")					SetActive(pAttr->GetAttributeAsDword() != 0);

	return 0;
}

void NetDeckCamera::SetViewPoint(CVECTOR & cViewPoint)
{
	if(pathNode==null) return;

	// Точка взгляда в локальные координаты
	CVECTOR e_pos;
	pathNode->glob_mtx.MulToInv(cViewPoint, e_pos);

	e_pos -= camera_pos;

	camera_ang.x = -atan2f(e_pos.y, sqrtf(e_pos.x * e_pos.x + e_pos.z * e_pos.z));
	camera_ang.y = atan2f(e_pos.x, e_pos.z);
}

float NetDeckCamera::MultiTrace(const CVECTOR & cvUp, const CVECTOR & cvDown, float fHBase)
{
	if(pathNode==null) return 2.f;

	float fRet = 2.f;

	float fTmp;
	GEOS::VERTEX curUp, curDown;
	curUp.x = cvUp.x;		curUp.y = cvUp.y;		curUp.z = cvUp.z;
	curDown.x = cvDown.x;	curDown.y = cvDown.y;	curDown.z = cvDown.z;

	float fDist = MEN_STEP_UP;

	while( (fTmp = pathNode->geo->Trace(curUp, curDown)) <= 1.f )
	{
		fTmp += 0.00001f;
		curUp.y += fTmp*(cvDown.y-curUp.y);
		if( (curUp.y-fHBase <= fDist) && (fHBase-curUp.y <= fDist) ) {
			if(curUp.y > fHBase) fDist = curUp.y-fHBase;
			else fDist = fHBase-curUp.y;
			fRet = (curUp.y - cvUp.y)/(cvDown.y-cvUp.y);

			GEOS::TRACE_INFO ti;
            pathNode->geo->GetCollisionDetails(ti);
			memcpy(&g_gv0,&ti.vrt[0],sizeof(g_gv0));
			memcpy(&g_gv1,&ti.vrt[1],sizeof(g_gv1));
			memcpy(&g_gv2,&ti.vrt[2],sizeof(g_gv2));
		}
	}

	return fRet;
}

