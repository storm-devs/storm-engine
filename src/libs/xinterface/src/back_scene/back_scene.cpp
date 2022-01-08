#include "back_scene.h"
#include "../xinterface.h"
#include "geometry.h"
#include "math3d/matrix.h"
#include "model.h"
#include "shared/messages.h"

InterfaceBackScene::LightParam::~LightParam()
{
    bUse = false;
    if (pModel)
    {
        EntityManager::EraseEntity(eiModel);
        pModel = nullptr;
    }
}

void InterfaceBackScene::LightParam::UpdateParams(float fTime)
{
    auto fK = 0.f;
    fColorTimer += fTime;
    int32_t jjj = 0;
    while (fColorTimer > fColorPeriod + fAddPeriod)
    {
        if (fColorTimer > fAddPeriodMax * 10.0f)
        {
            fColorTimer = fAddPeriodMax * 10.0f;
        }

        fColorTimer -= fColorPeriod + fAddPeriod;
        fAddPeriod = FRAND(fAddPeriodMax); //(-0.6f+FRAND(3.0f))*fColorPeriod;
        jjj++;
        if (jjj > 10000)
        {
            core.Trace("jjj: %f, %f", fColorTimer, fColorPeriod);
            __debugbreak();
        }
    }
    const auto fPer = fColorPeriod + fAddPeriod;
    if (fColorTimer <= .5f * fPer)
        fK = 2.f * fColorTimer / fPer;
    else
        fK = 2.f - 2.f * fColorTimer / fPer;
    lightSource.Diffuse.a = colorMin.a + (colorMax.a - colorMin.a) * fK; // 1.f
    lightSource.Diffuse.r = colorMin.r + (colorMax.r - colorMin.r) * fK; // 1.0f;
    lightSource.Diffuse.g = colorMin.g + (colorMax.g - colorMin.g) * fK; // 1.0f;
    lightSource.Diffuse.b = colorMin.b + (colorMax.b - colorMin.b) * fK; // 0.7f;

    dwFlareColor = static_cast<uint32_t>(fMinFlareColor + (fMaxFlareColor - fMinFlareColor) * fK);
    dwFlareColor = dwFlareColor | (dwFlareColor << 24) | (dwFlareColor << 16) | (dwFlareColor << 8);

    auto vPos = vLightPos;
    if (pLightSrcNode)
        vPos = pLightSrcNode->glob_mtx * vLightPos;

    lightSource.Position.x = vPos.x;
    lightSource.Position.y = vPos.y;
    lightSource.Position.z = vPos.z;

    fRangeTimer += fTime;
    if (fRangeTimer > fRangePeriod)
        fRangeTimer -= fRangePeriod;
    if (fRangeTimer <= .5f * fRangePeriod)
        fK = 2.f * fRangeTimer / fRangePeriod;
    else
        fK = 2.f - 2.f * fRangeTimer / fRangePeriod;
    lightSource.Range = fRangeMin + (fRangeMax - fRangeMin) * fK; // 10.f;
}

InterfaceBackScene::MenuDescr::~MenuDescr()
{
    EntityManager::EraseEntity(eiActive);
    pActive = nullptr;
    EntityManager::EraseEntity(eiPassive);
    pPassive = nullptr;
}

void InterfaceBackScene::MenuDescr::Set(CMatrix *pMtx, const char *pcActiveName, const char *pcPassiveName,
                                        const char *pcEvent, const char *pcPathName, const char *pcTechniqueName)
{
    if (!pcTechniqueName)
        pcTechniqueName = "InterfaceBackScene_Menu";
    sEventName = pcEvent;
    auto *pGeo = static_cast<VGEOMETRY *>(core.GetService("Geometry"));
    if (pGeo)
        if (pcPathName && pcPathName[0])
            pGeo->SetTexturePath((std::string("MainMenu\\") + pcPathName + "\\").c_str());
        else
            pGeo->SetTexturePath("MainMenu\\");
    // create active model
    if (pcActiveName)
    {
        eiActive = EntityManager::CreateEntity("MODELR");
        core.Send_Message(eiActive, "ls", MSG_MODEL_LOAD_GEO, pcActiveName);
        pActive = static_cast<MODEL *>(EntityManager::GetEntityPointer(eiActive));
        if (pActive && pMtx)
        {
            pActive->mtx = *pMtx;
            pActive->Update();
            if (pActive->GetNode(0))
                pActive->GetNode(0)->SetTechnique(pcTechniqueName);
        }
        else
        {
            core.Trace("Warning! Interface Back Scene: invalid menu model %s or transform matrix", pcActiveName);
        }
    }
    // create passive model
    if (pcPassiveName)
    {
        eiPassive = EntityManager::CreateEntity("MODELR");
        core.Send_Message(eiPassive, "ls", MSG_MODEL_LOAD_GEO, pcPassiveName);
        pPassive = static_cast<MODEL *>(EntityManager::GetEntityPointer(eiPassive));
        if (pPassive && pMtx)
        {
            pPassive->mtx = *pMtx;
            pPassive->Update();
            if (pPassive->GetNode(0))
                pPassive->GetNode(0)->SetTechnique(pcTechniqueName);
        }
        else
        {
            core.Trace("Warning! Interface Back Scene: invalid menu model %s or transform matrix", pcPassiveName);
        }
    }
    if (pGeo)
        pGeo->SetTexturePath("");
    if (pActive)
        bSelectable = true;
}

InterfaceBackScene::InterfaceBackScene()
    : m_pRS(nullptr), m_eiModel(0), m_eiLocators(0), m_nSelectMenuIndex(0), buffer{}, m_vFlarePos(), m_fFlareSize(0)
{
    m_pModel = nullptr;
    m_pLocators = nullptr;

    m_vCamPos = 0.f;
    m_vCamAng = 0.f;
    m_fCamPerspective = 1.287f;

    m_nFlareTexture = -1;

    // Flies
    numFlys = 0;
    maxFlys = 0;
    numFly = 0;
    flyTex = -1;
}

InterfaceBackScene::~InterfaceBackScene()
{
    RestoreLight();
    EntityManager::EraseEntity(m_eiModel);
    EntityManager::EraseEntity(m_eiLocators);
    m_pLocators = nullptr;
    m_pModel = nullptr;
    ReleaseMenuList();

    for (const auto &light : m_aLights)
        delete light;
    for (const auto &model : m_apAniModel)
        delete model;
    // m_apAniModel.DelAllWithPointers();

    if (m_nFlareTexture >= 0)
        m_pRS->TextureRelease(m_nFlareTexture);
    m_nFlareTexture = -1;

    if (flyTex >= 0)
        m_pRS->TextureRelease(flyTex);
    flyTex = -1;
}

bool InterfaceBackScene::Init()
{
    m_pRS = static_cast<VDX9RENDER *>(core.GetService("dx9render"));
    Assert(m_pRS);
    flyTex = m_pRS->TextureCreate("LocEfx\\firefly.tga");
    m_nFlareTexture = m_pRS->TextureCreate("ShipsFlares\\corona.tga");
    return true;
}

void InterfaceBackScene::Execute(uint32_t Delta_Time)
{
    const auto nOldMenuIndex = m_nSelectMenuIndex;

    const auto pntMouse = XINTERFACE::pThis->GetMousePoint();
    if (m_pntOldMouse.x != pntMouse.x || m_pntOldMouse.y != pntMouse.y)
    {
        m_pntOldMouse = pntMouse;
        const auto n = CheckMousePos(pntMouse.x, pntMouse.y);
        if (n >= 0 && n != m_nSelectMenuIndex)
            SetNewMenu(n);
    }

    CONTROL_STATE cs;
    core.Controls->GetControlState("IUp", cs);
    if (cs.state == CST_ACTIVATED)
    {
        ChoosePrevMenu();
    }
    core.Controls->GetControlState("IDown", cs);
    if (cs.state == CST_ACTIVATED)
    {
        ChooseNextMenu();
    }
    core.Controls->GetControlState("ILClick", cs);
    if (cs.state == CST_ACTIVATED)
    {
        ExecuteMenu(CheckMousePos(m_pntOldMouse.x, m_pntOldMouse.y));
    }
    core.Controls->GetControlState("IAction", cs);
    if (cs.state == CST_ACTIVATED)
    {
        ExecuteMenu(m_nSelectMenuIndex);
    }

    if (core.Controls->GetDebugAsyncKeyState(VK_CONTROL) < 0)
    {
        CMatrix mtx;
        mtx.BuildMatrix(m_vCamAng);
        const auto vz = mtx * CVECTOR(0.f, 0.f, 1.f);
        const auto vx = mtx * CVECTOR(1.f, 0.f, 0.f);

        auto fForwardSpeed = 0.01f * Delta_Time;
        if (core.Controls->GetDebugAsyncKeyState(VK_SHIFT) < 0)
            fForwardSpeed *= 10.f;
        if (core.Controls->GetDebugAsyncKeyState(VK_MENU) < 0)
            fForwardSpeed *= 0.1f;
        const auto fSideSpeed = 0.5f * fForwardSpeed;

        if (core.Controls->GetDebugAsyncKeyState('W') < 0)
            m_vCamPos += vz * fForwardSpeed;
        if (core.Controls->GetDebugAsyncKeyState('S') < 0)
            m_vCamPos -= vz * fForwardSpeed;
        if (core.Controls->GetDebugAsyncKeyState('D') < 0)
            m_vCamPos += vx * fSideSpeed;
        if (core.Controls->GetDebugAsyncKeyState('A') < 0)
            m_vCamPos -= vx * fSideSpeed;

        const auto fRotateSpeed = 0.001f * Delta_Time;
        if (core.Controls->GetDebugAsyncKeyState(VK_UP) < 0)
            m_vCamAng.x += fRotateSpeed;
        if (core.Controls->GetDebugAsyncKeyState(VK_DOWN) < 0)
            m_vCamAng.x -= fRotateSpeed;
        if (core.Controls->GetDebugAsyncKeyState(VK_LEFT) < 0)
            m_vCamAng.y -= fRotateSpeed;
        if (core.Controls->GetDebugAsyncKeyState(VK_RIGHT) < 0)
            m_vCamAng.y += fRotateSpeed;
    }

    m_pRS->SetCamera(m_vCamPos, m_vCamAng, m_fCamPerspective);

    if (nOldMenuIndex != m_nSelectMenuIndex)
        core.Event(ISOUND_EVENT, "l", 2); // choosing a new node

    /*    for( int32_t n=0; n<m_apAniModel; n++ )
      {
        if( m_apAniModel[n]->pModel )
          m_apAniModel[n]->pModel->Execute( Delta_Time );
      }*/
}

void InterfaceBackScene::Realize(uint32_t Delta_Time)
{
    int32_t n;

    ProcessedFlys(Delta_Time * .001f);

    // model rendering
    if (m_pModel)
    {
        SetLight();
        m_pRS->SetRenderState(D3DRS_LIGHTING, true);
        m_pModel->ProcessStage(Stage::realize, Delta_Time);
        for (n = 0; n < m_aLights.size(); n++) // show all lights
            if (m_aLights[n]->pModel)
            {
                m_aLights[n]->pModel->ProcessStage(Stage::realize, Delta_Time);
                FlareShow(n);
            }
        for (n = 0; n < m_apAniModel.size(); n++)
        {
            if (m_apAniModel[n]->pModel)
            {
                uint32_t dwTFactor;
                m_pRS->GetRenderState(D3DRS_TEXTUREFACTOR, &dwTFactor);
                if (m_apAniModel[n]->bUseTFactor)
                    m_pRS->SetRenderState(D3DRS_TEXTUREFACTOR, m_apAniModel[n]->dwTFactor);
                m_apAniModel[n]->pModel->ProcessStage(Stage::realize, Delta_Time);
                m_pRS->SetRenderState(D3DRS_TEXTUREFACTOR, dwTFactor);
            }
        }
        m_pRS->SetRenderState(D3DRS_LIGHTING, false);
        RestoreLight();
    }

    // rendering of menu items
    for (n = 0; n < m_aMenuDescr.size(); n++)
    {
        if (n == m_nSelectMenuIndex && m_aMenuDescr[n]->pActive)
            m_aMenuDescr[n]->pActive->ProcessStage(Stage::realize, Delta_Time);
        else if (m_aMenuDescr[n]->pPassive)
            m_aMenuDescr[n]->pPassive->ProcessStage(Stage::realize, Delta_Time);
    }
}

uint64_t InterfaceBackScene::ProcessMessage(MESSAGE &message)
{
    const auto nMsgCode = message.Long();
    switch (nMsgCode)
    {
    case 0: {
        // load model
        const std::string &param = message.String();
        LoadModel(param.c_str());
        break;
    }

    case 1: {
        // set camera
        const std::string &param = message.String();
        SetCameraPosition(param.c_str());
        break;
    }

    case 2: {
        // set ship position by locator
        const std::string &param = message.String();
        SetShipPosition(param.c_str(), message.AttributePointer());
        break;
    }

    case 3: // create menu list
    {
        const auto nStartIdx = message.Long();
        auto *const pA = message.AttributePointer();
        CreateMenuList(nStartIdx, pA);
    }
    break;

    case 4: // controling of menu list
    {
        const auto nControlCode = message.Long();
        if (nControlCode & 1)
            ChooseNextMenu();
        else
            ChoosePrevMenu();
    }
    break;

    case 5: // get current menu
        return m_nSelectMenuIndex;
        break;

    case 6: // set current menu
    {
        auto n = message.Long();
        if (n < 0 || n >= m_aMenuDescr.size() || !m_aMenuDescr[n]->bSelectable)
            n = -1;
        else
            m_nSelectMenuIndex = n;
    }
    break;

    case 7: // set selectable flag for menu
    {
        const auto num = message.Long();  // menu number
        const auto flag = message.Long(); // selectable state
        SetMenuSelectableState(num, flag != 0);
    }
    break;

    case 8: // set light source
    {
        const std::string &param = message.String();
        InitLight(AttributesPointer ? AttributesPointer->GetAttributeClass(param.c_str()) : nullptr);
    }
    break;

    case 9: // add animation model
    {
        const std::string &param = message.String();
        InitAniModel(AttributesPointer ? AttributesPointer->GetAttributeClass(param.c_str()) : nullptr);
    }
    break;

    case 10: // add model
    {
        const std::string &param = message.String();
        InitStaticModel(AttributesPointer ? AttributesPointer->GetAttributeClass(param.c_str()) : nullptr);
    }
    break;
    }
    return 0;
}

void InterfaceBackScene::LoadModel(const char *pcModelName)
{
    // delete all
    if (m_pModel)
    {
        EntityManager::EraseEntity(m_eiModel);
        m_pModel = nullptr;
    }
    if (m_pLocators)
    {
        EntityManager::EraseEntity(m_eiLocators);
        m_pLocators = nullptr;
    }
    auto *pGeo = static_cast<VGEOMETRY *>(core.GetService("Geometry"));
    if (pGeo)
        pGeo->SetTexturePath(
            (std::string("MainMenu\\") + XINTERFACE::pThis->StringService()->GetLanguage() + "\\").c_str());
    // create model
    m_eiModel = EntityManager::CreateEntity("MODELR");
    core.Send_Message(m_eiModel, "ls", MSG_MODEL_LOAD_GEO, pcModelName);
    m_pModel = static_cast<MODEL *>(EntityManager::GetEntityPointer(m_eiModel));
    if (pGeo)
        pGeo->SetTexturePath("");
    EntityManager::AddToLayer(SUN_TRACE, m_eiModel, 0);
    EntityManager::AddToLayer(RAIN_DROPS, m_eiModel, 100);
    // create locators
    m_eiLocators = EntityManager::CreateEntity("MODELR");
    const auto sLocName = std::string(pcModelName) + "_locators";
    core.Send_Message(m_eiLocators, "ls", MSG_MODEL_LOAD_GEO, sLocName.c_str());
    m_pLocators = static_cast<MODEL *>(EntityManager::GetEntityPointer(m_eiLocators));
}

void InterfaceBackScene::SetCameraPosition(const char *pcLocatorName)
{
    // FindLocator( pcLocatorName, 0, &m_vCamPos, &m_vCamAng.y );
    Matrix mtx;
    FindLocator(pcLocatorName, (CMatrix *)&mtx, &m_vCamPos, nullptr);
    Vector vAddZ;
    mtx.GetAngles(m_vCamAng.x, m_vCamAng.y, m_vCamAng.z);
    vAddZ = mtx.MulNormal(Vector(0.f, 0.f, 1.f));
    vAddZ *= -0.1f;
    m_vCamPos.x += vAddZ.x;
    m_vCamPos.y += vAddZ.y;
    m_vCamPos.z += vAddZ.z;
    // m_vCamAng.x = -4.262f/180.f*PI;
}

void InterfaceBackScene::SetShipPosition(const char *pcLocName, ATTRIBUTES *pAChar) const
{
    if (!pcLocName || !pAChar || !m_pLocators)
        return;

    auto *pAPos = pAChar->FindAClass(pAChar, "Ship.Pos");
    if (!pAPos)
        pAPos = pAChar->CreateSubAClass(pAChar, "Ship.Pos");
    Assert(pAPos);

    auto *pAAng = pAChar->FindAClass(pAChar, "Ship.Ang");
    if (!pAAng)
        pAAng = pAChar->CreateSubAClass(pAChar, "Ship.Ang");
    Assert(pAAng);

    CVECTOR pos;
    float fYAng;
    if (FindLocator(pcLocName, nullptr, &pos, &fYAng))
    {
        pAPos->SetAttributeUseFloat("x", pos.x);
        pAPos->SetAttributeUseFloat("y", pos.y);
        pAPos->SetAttributeUseFloat("z", pos.z);
        pAAng->SetAttributeUseFloat("y", fYAng);
    }
}

bool InterfaceBackScene::FindLocator(const char *pcLocName, CMatrix *pMtx, CVECTOR *pPos, float *pYAng) const
{
    if (!pcLocName || !m_pLocators)
        return false;
    for (int32_t n = 0; n < 100; n++)
    {
        auto *pNod = m_pLocators->GetNode(n);
        if (!pNod)
            break;
        GEOS::INFO ginf;
        pNod->geo->GetInfo(ginf);
        GEOS::LABEL lbl;
        for (int32_t l = 0; l < ginf.nlabels; l++)
        {
            pNod->geo->GetLabel(l, lbl);
            if (lbl.name && storm::iEquals(pcLocName, lbl.name))
            {
                if (pMtx)
                {
                    memcpy(pMtx, &lbl.m, sizeof(float) * 4 * 4);
                }
                if (pPos)
                {
                    pPos->x = lbl.m[3][0] + pNod->glob_mtx.matrix[3];
                    pPos->y = lbl.m[3][1] + pNod->glob_mtx.matrix[7];
                    pPos->z = lbl.m[3][2] + pNod->glob_mtx.matrix[11];
                }
                if (pYAng)
                {
                    *pYAng = atan2f(lbl.m[2][0], lbl.m[2][2]);
                }
                return true;
            }
        }
    }
    return false;
}

void InterfaceBackScene::SetLocatorPosition(MODEL *pModel, const char *pcLocName, CVECTOR &pos, NODE *&pNodPtr)
{
    pos = 0.f;
    if (pModel && pcLocName)
    {
        for (int32_t n = 0; n < 100; n++)
        {
            auto *const pNod = pModel->GetNode(n);
            if (!pNod)
                break;
            GEOS::INFO ginf;
            pNod->geo->GetInfo(ginf);
            GEOS::LABEL lbl;
            for (int32_t l = 0; l < ginf.nlabels; l++)
            {
                pNod->geo->GetLabel(l, lbl);
                if (lbl.name && storm::iEquals(pcLocName, lbl.name))
                {
                    pos.x = lbl.m[3][0];
                    pos.y = lbl.m[3][1];
                    pos.z = lbl.m[3][2];
                    pNodPtr = pNod;
                    return;
                }
            }
        }
    }
}

void InterfaceBackScene::ReleaseMenuList()
{
    // m_aMenuDescr.DelAllWithPointers();
    for (const auto &descr : m_aMenuDescr)
        delete descr;
    m_aMenuDescr.clear();
}

void InterfaceBackScene::CreateMenuList(int32_t nStartIndex, ATTRIBUTES *pAMenu)
{
    ReleaseMenuList();
    if (!pAMenu)
        return;

    ATTRIBUTES *pA;
    CMatrix mtx;
    const int32_t q = pAMenu->GetAttributesNum();
    for (int32_t n = 0; n < q; n++)
    {
        pA = pAMenu->GetAttributeClass(n);
        if (!pA)
            continue;
        if (!FindLocator(pA->GetAttribute("locname"), &mtx, nullptr, nullptr))
        {
            core.Trace("Warning! Interface Back scene: Can`t find locator %s", pA->GetAttribute("locname"));
        }
        auto *pMD = new MenuDescr;
        Assert(pMD);
        pMD->Set(&mtx, pA->GetAttribute("sel"), pA->GetAttribute("norm"), pA->GetAttribute("event"),
                 pA->GetAttribute("path"), pA->GetAttribute("technique"));
        m_aMenuDescr.push_back(pMD);
    }
    if (nStartIndex >= 0 && nStartIndex < m_aMenuDescr.size() && m_aMenuDescr[nStartIndex]->bSelectable)
        m_nSelectMenuIndex = nStartIndex;
    else
        m_nSelectMenuIndex = -1;
}

void InterfaceBackScene::ChooseNextMenu()
{
    int32_t n;
    for (n = m_nSelectMenuIndex + 1; n < m_aMenuDescr.size(); n++)
        if (m_aMenuDescr[n]->bSelectable)
            break;
    if (n < m_aMenuDescr.size())
        m_nSelectMenuIndex = n;
}

void InterfaceBackScene::ChoosePrevMenu()
{
    int32_t n;
    for (n = m_nSelectMenuIndex - 1; n >= 0; n--)
        if (m_aMenuDescr[n]->bSelectable)
            break;
    if (n >= 0)
        m_nSelectMenuIndex = n;
}

void InterfaceBackScene::SetNewMenu(int32_t nNewSelectIndex)
{
    if (nNewSelectIndex < 0 || nNewSelectIndex >= m_aMenuDescr.size() || !m_aMenuDescr[nNewSelectIndex]->bSelectable)
        return;
    m_nSelectMenuIndex = nNewSelectIndex;
}

void InterfaceBackScene::SetMenuSelectableState(int32_t nMenuIndex, bool bSelectable)
{
    if (nMenuIndex < 0 || nMenuIndex >= m_aMenuDescr.size())
        return;
    if (m_aMenuDescr[nMenuIndex]->bSelectable == bSelectable)
        return;
    if (bSelectable && m_aMenuDescr[nMenuIndex]->pActive && m_aMenuDescr[nMenuIndex]->pPassive)
        m_aMenuDescr[nMenuIndex]->bSelectable = true;
    else
        m_aMenuDescr[nMenuIndex]->bSelectable = false;
}

void InterfaceBackScene::ExecuteMenu(int32_t nMenuIndex)
{
    if (nMenuIndex < 0 || nMenuIndex >= m_aMenuDescr.size())
        return;
    core.PostEvent("backgroundcommand", 1, "s", m_aMenuDescr[nMenuIndex]->sEventName.c_str());
}

int32_t InterfaceBackScene::CheckMousePos(float fX, float fY)
{
    float fW = static_cast<float>(XINTERFACE::pThis->GetScreenWidth());
    auto fH = static_cast<float>(XINTERFACE::pThis->GetScreenHeight());
    float fRelX = 2.f * fX / fW - 1.f;
    float fRelY = 2.f * fY / fH - 1.f;

    CMatrix mtxProj;
    m_pRS->GetTransform(D3DTS_PROJECTION, (D3DXMATRIX *)&mtxProj);
    CVECTOR v;
    v.x = fRelX / mtxProj.m[0][0];
    v.y = -fRelY / mtxProj.m[1][1];
    v.z = 1.0f;

    CMatrix mtxView;
    m_pRS->GetTransform(D3DTS_VIEW, (D3DXMATRIX *)&mtxView);
    CVECTOR vDir;
    mtxView.MulToInvNorm(v, vDir);
    mtxView.Transposition();
    CVECTOR vStart = mtxView.Pos();
    CVECTOR vEnd = vStart + vDir * 300.f;

    for (int32_t n = 0; n < m_aMenuDescr.size(); n++)
        if (m_aMenuDescr[n]->bSelectable && m_aMenuDescr[n]->pActive &&
            m_aMenuDescr[n]->pActive->Trace(vStart, vEnd) <= 1.f)
            return n;
    return -1;
}

void InterfaceBackScene::InitLight(ATTRIBUTES *pAParam)
{
    if (!pAParam)
        return;

    auto *pLight = new LightParam();
    Assert(pLight);

    m_aLights.push_back(pLight);

    ZERO(pLight->lightSource);
    pLight->lightSource.Type = D3DLIGHT_POINT;
    pLight->lightSource.Attenuation0 = 0.0f;
    pLight->lightSource.Attenuation1 = 0.0f;
    pLight->lightSource.Attenuation2 = 1.0f;
    pLight->indexLight = -1;

    const float fDiv = 1.f / 255.f;
    uint32_t dwTmp = pAParam->GetAttributeAsDword("lightcolormin", 0xFFFFFFFF);
    pLight->colorMin.a = ALPHA(dwTmp) * fDiv;
    pLight->colorMin.r = RED(dwTmp) * fDiv;
    pLight->colorMin.g = GREEN(dwTmp) * fDiv;
    pLight->colorMin.b = BLUE(dwTmp) * fDiv;
    dwTmp = pAParam->GetAttributeAsDword("lightcolormax", 0xFFFFFFFF);
    pLight->colorMax.a = ALPHA(dwTmp) * fDiv;
    pLight->colorMax.r = RED(dwTmp) * fDiv;
    pLight->colorMax.g = GREEN(dwTmp) * fDiv;
    pLight->colorMax.b = BLUE(dwTmp) * fDiv;
    pLight->fColorPeriod = pAParam->GetAttributeAsFloat("colorperiod", 1.f);
    pLight->fAddPeriodMax = pAParam->GetAttributeAsFloat("addcolorperiod", 1.f);
    pLight->fColorTimer = 0.f;

    pLight->fRangeMin = pAParam->GetAttributeAsFloat("rangemin", 5.f);
    pLight->fRangeMax = pAParam->GetAttributeAsFloat("rangemax", 10.f);
    pLight->fRangePeriod = pAParam->GetAttributeAsFloat("rangeperiod", 1.f);
    pLight->fRangeTimer = 0.f;

    pLight->fMinFlareColor = pAParam->GetAttributeAsFloat("minflarecolor", 200.f);
    pLight->fMaxFlareColor = pAParam->GetAttributeAsFloat("maxflarecolor", 255.f);

    pLight->bUse = pAParam->GetAttributeAsDword("turnon", 0) != 0;

    // find transform from locator
    CMatrix locMtx;
    FindLocator(pAParam->GetAttribute("locator"), &locMtx, nullptr, nullptr);
    pLight->vLightPos = locMtx.Pos();

    // load model
    char *pcFonarModel = pAParam->GetAttribute("model");
    if (pcFonarModel)
    {
        auto pGeo = static_cast<VGEOMETRY *>(core.GetService("Geometry"));
        if (pGeo)
            pGeo->SetTexturePath("MainMenu\\");
        // create model
        pLight->eiModel = EntityManager::CreateEntity("MODELR");
        core.Send_Message(pLight->eiModel, "ls", MSG_MODEL_LOAD_GEO, pcFonarModel);
        pLight->pModel = static_cast<MODEL *>(EntityManager::GetEntityPointer(pLight->eiModel));
        if (pGeo)
            pGeo->SetTexturePath("");
        if (pLight->pModel)
        {
            pLight->pModel->mtx = locMtx;
            pLight->pModel->Update();

            pLight->pLightSrcNode = pLight->pModel->GetNode(0);
            SetLocatorPosition(pLight->pModel, pAParam->GetAttribute("lightlocator"), pLight->vLightPos,
                               pLight->pLightSrcNode);
            if (m_aLights.size() > 0 && m_aLights[0]->bUse)
            {
                CVECTOR vFlarePos = pLight->vLightPos;
                SetLocatorPosition(pLight->pModel, pAParam->GetAttribute("flarelocator"), vFlarePos,
                                   pLight->pLightSrcNode);
                m_vFlarePos = pLight->pLightSrcNode->glob_mtx * vFlarePos;
                m_fFlareSize = pAParam->GetAttributeAsFloat("flaresize", 0.2f);
                AddLampFlys(m_vFlarePos);

                ATTRIBUTES *pA = nullptr;
                if (AttributesPointer)
                    pA = AttributesPointer->CreateSubAClass(AttributesPointer, "lightpos");
                if (pA)
                {
                    pA->SetAttributeUseFloat("x", m_vFlarePos.x);
                    pA->SetAttributeUseFloat("y", m_vFlarePos.y);
                    pA->SetAttributeUseFloat("z", m_vFlarePos.z);
                }
            }
        }
        else
        {
            core.Trace("Warning! Interface Back Scene: invalid torchlight model %s", pcFonarModel);
        }
    }
}

void InterfaceBackScene::SetLight()
{
    int32_t nFreeLightIndex = 0;
    D3DCAPS9 d3dcaps;
    m_pRS->GetDeviceCaps(&d3dcaps);

    for (int32_t n = 0; n < m_aLights.size(); n++)
    {
        if (m_aLights[n]->bUse)
        {
            BOOL bTmp;
            for (; nFreeLightIndex < static_cast<int32_t>(d3dcaps.MaxActiveLights); nFreeLightIndex++)
                if (m_pRS->GetLightEnable(nFreeLightIndex, &bTmp) && bTmp == false)
                    break;
            if (nFreeLightIndex < static_cast<int32_t>(d3dcaps.MaxActiveLights))
            {
                // found a free source
                m_aLights[n]->indexLight = nFreeLightIndex;
                m_pRS->GetLight(nFreeLightIndex, &m_aLights[n]->lightOldSource);
                m_pRS->LightEnable(nFreeLightIndex, true);
                m_aLights[n]->UpdateParams(core.GetDeltaTime() * .001f);
                m_pRS->SetLight(nFreeLightIndex, &m_aLights[n]->lightSource);
            }
            else
                m_aLights[n]->indexLight = -1;
        }
    }
}

void InterfaceBackScene::RestoreLight()
{
    for (int32_t n = 0; n < m_aLights.size(); n++)
    {
        if (m_aLights[n]->bUse)
        {
            if (m_aLights[n]->indexLight >= 0)
            {
                m_pRS->SetLight(m_aLights[n]->indexLight, &m_aLights[n]->lightOldSource);
                m_pRS->LightEnable(m_aLights[n]->indexLight, false);
                m_aLights[n]->indexLight = -1;
            }
        }
    }
}

void InterfaceBackScene::FlareShow(int32_t idx)
{
    CVECTOR pos, ang;
    m_pRS->GetCamera(pos, ang, ang.x);
    CMatrix camMtx;
    m_pRS->GetTransform(D3DTS_VIEW, camMtx);

    float dx = m_vFlarePos.x - pos.x;
    float dy = m_vFlarePos.y - pos.y;
    float dz = m_vFlarePos.z - pos.z;

    /*    float d = dx*dx + dy*dy + dz*dz;

    // Distance
    float dist = sqrtf(d);
    d = dist / m_fFlareRange;
    if(d > 1.0f) d = 1.0f;
    float alpha = 1.0f;
    if(d < 0.3f) alpha *= 0.2f + 0.8f*d/0.3f;
    if(d > 0.4f){ alpha *= 1.0f - (d - 0.4f)/0.6f; alpha *= alpha; }
    alpha * = 255.f; // ls.corona * 255.0f;
    // Deviation coefficient
    d = 0.f;// ls.i*0.4f;
    if(d < -0.1f) d = -0.1f;
    if(d > 0.1f) d = 0.1f;
    d += 1.0f;
    // Current size
    float size = d*m_fFlareSize;
    // Transparency
    alpha * = d;
    if(alpha < 0.0f) alpha = 0.0f;
    if(alpha > 255.0f) alpha = 255.0f;
    // Position
    pos = camMtx * m_vFlarePos;
    // Colour
    uint32_t c = uint32_t(alpha); c |= (c << 24) | (c << 16) | (c << 8);*/
    // Angle of rotation
    float cs, sn;
    float _cs = (dx * camMtx.Vx().z + dz * camMtx.Vz().z);
    float _sn = (dx * camMtx.Vz().z - dz * camMtx.Vx().z);
    float kn = _cs * _cs + _sn * _sn;
    if (kn > 0.0f)
    {
        kn = 1.0f / sqrtf(kn);
        _cs *= kn;
        _sn *= kn;
        cs = (_cs * _cs - _sn * _sn);
        sn = 2.0f * _cs * _sn;
    }
    else
    {
        cs = 1.0f;
        sn = 0.0f;
    }

    // Position
    pos = m_vFlarePos;

    uint32_t c = m_aLights[idx]->dwFlareColor;

    buffer[0].pos = pos + CVECTOR(m_fFlareSize * (-cs + sn), m_fFlareSize * (sn + cs), 0.0f);
    buffer[0].color = c;
    buffer[0].u = 0.0f;
    buffer[0].v = 0.0f;
    buffer[1].pos = pos + CVECTOR(m_fFlareSize * (-cs - sn), m_fFlareSize * (sn - cs), 0.0f);
    buffer[1].color = c;
    buffer[1].u = 0.0f;
    buffer[1].v = 1.0f;
    buffer[2].pos = pos + CVECTOR(m_fFlareSize * (cs + sn), m_fFlareSize * (-sn + cs), 0.0f);
    buffer[2].color = c;
    buffer[2].u = 1.0f;
    buffer[2].v = 0.0f;
    buffer[3].pos = buffer[2].pos;
    buffer[3].color = c;
    buffer[3].u = 1.0f;
    buffer[3].v = 0.0f;
    buffer[4].pos = buffer[1].pos;
    buffer[4].color = c;
    buffer[4].u = 0.0f;
    buffer[4].v = 1.0f;
    buffer[5].pos = pos + CVECTOR(m_fFlareSize * (cs - sn), m_fFlareSize * (-sn - cs), 0.0f);
    buffer[5].color = c;
    buffer[5].u = 1.0f;
    buffer[5].v = 1.0f;
    m_pRS->TextureSet(0, m_nFlareTexture);
    CMatrix mtx;
    mtx.SetIdentity();
    m_pRS->SetWorld(mtx);
    m_pRS->DrawPrimitiveUP(D3DPT_TRIANGLELIST, D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1, 2, buffer, sizeof(Vertex),
                           "Coronas");
}

void InterfaceBackScene::InitAniModel(ATTRIBUTES *pAParam)
{
    if (!pAParam)
        return;

    const char *pcMdlName = pAParam->GetAttribute("model");
    const char *pcAniName = pAParam->GetAttribute("animation");
    const char *pcAniActionName = pAParam->GetAttribute("aniaction");
    if (!pcMdlName)
    {
        core.Trace("Warning! Bad model name parameter for ani model into InterfaceBackScene.");
        return;
    }

    CMatrix mtx;
    if (!FindLocator(pAParam->GetAttribute("locator"), &mtx, nullptr, nullptr))
        mtx.SetIdentity();

    auto *pObj = new AniModelDescr;
    Assert(pObj);

    auto *pAniService = static_cast<ANIMATION *>(core.GetService("AnimationServiceImp"));
    auto *pGeo = static_cast<VGEOMETRY *>(core.GetService("Geometry"));
    if (pGeo)
        pGeo->SetTexturePath("MainMenu\\");
    // create model
    pObj->ei = EntityManager::CreateEntity("MODELR");
    core.Send_Message(pObj->ei, "ls", MSG_MODEL_LOAD_GEO, pcMdlName);
    pObj->pModel = static_cast<MODEL *>(EntityManager::GetEntityPointer(pObj->ei));
    if (pGeo)
        pGeo->SetTexturePath("");

    if (pObj->pModel)
    {
        pObj->pModel->mtx = mtx;
        // pObj->pModel->Update();
        if (pcAniName)
        {
            core.Send_Message(pObj->ei, "ls", MSG_MODEL_LOAD_ANI, pcAniName);
            if (pcAniActionName)
            {
                pObj->pModel->GetAnimation()->Player(0).SetAction(pcAniActionName);
                pObj->pModel->GetAnimation()->Player(0).Play();
            }
        }
    }

    m_apAniModel.push_back(pObj);

    // pObj->pModel->GetAnimation()-
}

void InterfaceBackScene::InitStaticModel(ATTRIBUTES *pAParam)
{
    if (!pAParam)
        return;

    const char *pcMdlName = pAParam->GetAttribute("model");
    const char *pcTechniqueName = pAParam->GetAttribute("technique");
    if (!pcMdlName)
    {
        core.Trace("Warning! Bad model name parameter for static model into InterfaceBackScene.");
        return;
    }

    CMatrix mtx;
    if (!FindLocator(pAParam->GetAttribute("locator"), &mtx, nullptr, nullptr))
        mtx.SetIdentity();

    auto *pObj = new AniModelDescr;
    Assert(pObj);

    auto *pGeo = static_cast<VGEOMETRY *>(core.GetService("Geometry"));
    if (pGeo)
        pGeo->SetTexturePath("MainMenu\\");
    // create model
    pObj->ei = EntityManager::CreateEntity("MODELR");
    core.Send_Message(pObj->ei, "ls", MSG_MODEL_LOAD_GEO, pcMdlName);
    pObj->pModel = static_cast<MODEL *>(EntityManager::GetEntityPointer(pObj->ei));
    if (pGeo)
        pGeo->SetTexturePath("");

    if (pAParam->GetAttribute("tfactor"))
    {
        pObj->bUseTFactor = true;
        pObj->dwTFactor = pAParam->GetAttributeAsDword("tfactor");
    }
    else
    {
        pObj->bUseTFactor = false;
    }

    if (pObj->pModel)
    {
        pObj->pModel->mtx = mtx;
        if (pcTechniqueName)
        {
            pObj->pModel->GetNode(0)->SetTechnique(pcTechniqueName);
        }
    }

    m_apAniModel.push_back(pObj);
}

// ---------------------------------------------------
// Flies at the lamps
// ---------------------------------------------------
void InterfaceBackScene::AddLampFlys(CVECTOR &pos)
{
    // resize the array
    if (numFlys >= maxFlys)
    {
        maxFlys += 8;
        flys.resize(maxFlys);
    }
    // Fill in the parameters
    // common
    flys[numFlys].pos = pos;
    flys[numFlys].radius = 0.2f; // 0.6f;
    flys[numFlys].start = numFly;
    flys[numFlys].num = 4 + (rand() & 4); // 1 + (rand() & 7);
    numFly += flys[numFlys].num;
    fly.resize(numFly);
    // of each fly
    for (int32_t i = 0; i < flys[numFlys].num; i++)
    {
        ParticleFly &f = fly[flys[numFlys].start + i];
        f.ax = rand() * PIm2 / RAND_MAX;
        f.ay = rand() * PIm2 / RAND_MAX;
        f.kx = 0.8f + rand() * (0.4f / RAND_MAX);
        if (rand() & 1)
            f.kx = -f.kx;
        f.ky = 0.8f + rand() * (0.4f / RAND_MAX);
        if (rand() & 1)
            f.ky = -f.ky;
        f.a = rand() * PIm2 / RAND_MAX;
        f.k = 0.8f + rand() * (0.4f / RAND_MAX);
        if (rand() & 1)
            f.k = -f.k;
        f.angle = 0.0f;
        f.size = 0.03f;
        f.alpha = 1.0f;
        f.frame = static_cast<float>(rand() & 3);
    }
    numFlys++;
}

void InterfaceBackScene::ProcessedFlys(float dltTime)
{
    CMatrix view;
    m_pRS->GetTransform(D3DTS_VIEW, view);
    view.Transposition();
    const CVECTOR cam = view.Pos();
    const float dax = dltTime * 1.3f;
    const float day = dltTime * 1.4f;
    const float da = dltTime * 5.6f;
    // calculate
    for (int32_t i = 0; i < numFlys; i++)
    {
        // Coefficient of visibility
        CVECTOR dir = cam - flys[i].pos;
        float k = ~dir;
        if (k > 400.0f)
            continue;
        k = sqrtf(k);
        if (k > 0.0f)
            dir *= 1.0f / k;
        k = k / 20.0f;
        k = 3.0f * (1.0f - k);
        if (k > 1.0f)
            k = 1.0f;
        // Updating flies
        ParticleFly *fl = &fly[flys[i].start];
        for (int32_t j = 0; j < flys[i].num; j++)
        {
            ParticleFly &f = fl[j];
            // Angles
            f.ax += dax * f.kx;
            f.ay += day * f.ky;
            f.a += da * f.k;
            // Radius
            float r = 1.0f + 0.5f * sinf(f.a) + 0.2f * cosf(f.a * f.k * 2.1f);
            r *= flys[i].radius;
            // Position
            f.pos.x = flys[i].pos.x + r * sinf(f.ax) * sinf(f.ay);
            f.pos.y = flys[i].pos.y + r * cosf(f.ax) * cosf(f.ay);
            f.pos.z = flys[i].pos.z + r * sinf(f.ax) * cosf(f.ay);
            // Transparency
            f.alpha = k * 255.0f;
            // Colour
            CVECTOR tmp = f.pos - flys[i].pos;
            float dst = sqrtf(~tmp);
            if (dst > 0.0f)
                tmp *= 1.0f / dst;
            float cs = -(tmp | dir) + 0.4f;
            if (dst > flys[i].radius)
            {
                dst = 3.0f * (dst - flys[i].radius) / flys[i].radius;
                if (dst > 1.0f)
                    dst = 1.0f;
                cs *= 1.0f - dst;
            }
            if (cs < 0.0f)
                cs = 0.0f;
            if (cs > 1.0f)
                cs = 1.0f;
            f.color = static_cast<int32_t>(cs * 255.0f);
            f.color |= (f.color << 16) | (f.color << 8);
            // Frame
            f.frame += dltTime * f.k * 25.0f;
            if (f.frame >= 4.0f)
                f.frame -= 4.0f;
            // Angle
            f.angle += dltTime * f.k * 3.0f;
        }
    }
    // Draw
    DrawParticles(fly.data(), numFly, sizeof(ParticleFly), flyTex, "LocFly", true, 4);
}

void InterfaceBackScene::DrawParticles(void *prts, int32_t num, int32_t size, int32_t texture, const char *tech, bool isEx,
                                       int32_t numU)
{
    CMatrix camMtx;
    m_pRS->GetTransform(D3DTS_VIEW, camMtx);
    m_pRS->SetTransform(D3DTS_VIEW, CMatrix());
    m_pRS->SetTransform(D3DTS_WORLD, CMatrix());
    m_pRS->TextureSet(0, texture);
    int32_t n = 0;
    for (int32_t i = 0; i < num; i++)
    {
        auto *parts = static_cast<Particle *>(prts);
        prts = static_cast<char *>(prts) + size;
        CVECTOR pos = camMtx * parts->pos;
        const float size = parts->size * 0.5f;
        const float sn = sinf(parts->angle);
        const float cs = cosf(parts->angle);
        int32_t color = (static_cast<int32_t>(parts->alpha) << 24);
        if (!isEx)
            color |= 0x00ffffff;
        else
            color |= 0x00ffffff & static_cast<ParticleEx *>(parts)->color;
        float u1 = 0.0f;
        float u2 = 1.0f;
        if (isEx && numU)
        {
            u2 = 1.0f / static_cast<float>(numU);
            u1 = static_cast<int32_t>(static_cast<ParticleEx *>(parts)->frame) * u2;
            u2 += u1;
        }
        buffer[n * 6 + 0].pos = pos + CVECTOR(size * (-cs + sn), size * (sn + cs), 0.0f);
        buffer[n * 6 + 0].color = color;
        buffer[n * 6 + 0].u = u1;
        buffer[n * 6 + 0].v = 0.0f;
        buffer[n * 6 + 1].pos = pos + CVECTOR(size * (-cs - sn), size * (sn - cs), 0.0f);
        buffer[n * 6 + 1].color = color;
        buffer[n * 6 + 1].u = u1;
        buffer[n * 6 + 1].v = 1.0f;
        buffer[n * 6 + 2].pos = pos + CVECTOR(size * (cs + sn), size * (-sn + cs), 0.0f);
        buffer[n * 6 + 2].color = color;
        buffer[n * 6 + 2].u = u2;
        buffer[n * 6 + 2].v = 0.0f;
        buffer[n * 6 + 3].pos = buffer[n * 6 + 2].pos;
        buffer[n * 6 + 3].color = color;
        buffer[n * 6 + 3].u = u2;
        buffer[n * 6 + 3].v = 0.0f;
        buffer[n * 6 + 4].pos = buffer[n * 6 + 1].pos;
        buffer[n * 6 + 4].color = color;
        buffer[n * 6 + 4].u = u1;
        buffer[n * 6 + 4].v = 1.0f;
        buffer[n * 6 + 5].pos = pos + CVECTOR(size * (cs - sn), size * (-sn - cs), 0.0f);
        buffer[n * 6 + 5].color = color;
        buffer[n * 6 + 5].u = u2;
        buffer[n * 6 + 5].v = 1.0f;
        n++;
        if (n * 2 == 256)
        {
            m_pRS->DrawPrimitiveUP(D3DPT_TRIANGLELIST, D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1, n * 2, buffer,
                                   sizeof(Vertex), (char *)tech);
            n = 0;
        }
    }
    if (n > 0)
    {
        m_pRS->DrawPrimitiveUP(D3DPT_TRIANGLELIST, D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1, n * 2, buffer,
                               sizeof(Vertex), (char *)tech);
    }
    m_pRS->SetTransform(D3DTS_VIEW, camMtx);
}
