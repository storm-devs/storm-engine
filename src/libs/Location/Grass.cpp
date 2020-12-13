//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//	Grass
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#include "Grass.h"
#include "Character.h"
#include "EntityManager.h"
#include "defines.h"

//============================================================================================

#define GRASS_BLK_LOD 0.8f //Степень при kLod - меньше, лоды переключаються ближе
#define GRASS_MAX_DIST 50.0f //Максимальная дистанция видимости
#define GRASS_MIN_DIST 10.0f //Минимальная дистанция непереключения лодов
#define GRASS_MAX_W 1.0f     //Наибольшая ширина
#define GRASS_MAX_H 0.2f     //Наибольшая высота
#define GRASS_VEIW ((long)(m_fMaxVisibleDist / GRASS_BLK_DST / GRASS_BLK_DST + 0.5f) + 1) //Дальность видимости в блоках
#define GRASS_FRM_W 4 //Количество кадров по ширене M = 2^N
#define GRASS_FRM_H 4 //Количество кадров по высоте M = 2^N
#define GRASS_TEX_W (1.0f / GRASS_FRM_W)
#define GRASS_TEX_H (1.0f / GRASS_FRM_H)
#define GRASS_TEX_DELTA (1.0f / 256.0f)
#define GRASS_MAX_POINTS 8192

#define GRASS_DEFTEXTURE "Grass\\grass.tga"

//============================================================================================
//Конструирование, деструктурирование
//============================================================================================

Grass::Grass()
{
    miniMap = nullptr;
    block = nullptr;
    numElements = 0;
    texture = -1;
    phase[0] = 0.1f;
    phase[1] = 0.51f;
    phase[2] = 0.01f;
    phase[3] = -0.2f;
    phase[4] = 0.03f;
    phase[5] = 0.04f;
    phase[6] = 0.08f;

    lodSelect = GRASS_BLK_LOD;
    winForce = 0.3f;
    winDir = !CVECTOR(0.0f, 0.0f, 1.0f);
    numCharacters = 0;

    strcpy_s(textureName, GRASS_DEFTEXTURE);

    vb = ib = -1;
    vbuffer = nullptr;

    quality = rq_full;
    windAng = 0.0f;
    initForce = 0;

    m_fDataScale = 1.0f;
    m_fMaxWidth = GRASS_MAX_W;
    m_fMaxHeight = GRASS_MAX_H;
    m_fMinVisibleDist = GRASS_MIN_DIST;
    m_fMaxVisibleDist = GRASS_MAX_DIST;
    m_fMinGrassLod = 0.f;
}

Grass::~Grass()
{
    delete miniMap;
    delete block;
    if (rs)
    {
        if (texture >= 0)
            rs->TextureRelease(texture);
        if (vb >= 0)
        {
            if (vbuffer)
                rs->UnLockVertexBuffer(vb);
            vbuffer = nullptr;
            rs->ReleaseVertexBuffer(vb);
        }
        if (ib >= 0)
            rs->ReleaseIndexBuffer(ib);
    }
}

//Инициализация
bool Grass::Init()
{
    // Layers
    // api->LayerCreate("execute", true, false);
    EntityManager::SetLayerType(EXECUTE, EntityManager::Layer::Type::execute);
    EntityManager::AddToLayer(EXECUTE, GetId(), 1000);
    // api->LayerCreate("realize", true, false);
    EntityManager::SetLayerType(REALIZE, EntityManager::Layer::Type::realize);
    EntityManager::AddToLayer(REALIZE, GetId(), 1000);

    // boal выбор шайдера -->
    isGrassLightsOn = 1;
    if (auto *param = api->Event("GOpt_isGrassLightsOn", nullptr))
    {
        param->Get(isGrassLightsOn);
    }
    // boal выбор шайдера <--

    // DX9 render
    rs = static_cast<VDX9RENDER *>(api->CreateService("dx9render"));
    if (!rs)
        throw std::exception("No service: dx9render");
    // Vertex declaration
    CreateVertexDeclaration();
    //Буфер для динамических данных
    vb = rs->CreateVertexBuffer(0, GRASS_MAX_POINTS * 4 * sizeof(Vertex), D3DUSAGE_DYNAMIC);
    if (vb < 0)
        return false;
    //Индексы, адресующие вершины
    ib = rs->CreateIndexBuffer(GRASS_MAX_POINTS * 6 * sizeof(uint16_t));
    if (ib < 0)
        return false;
    auto *index = static_cast<uint16_t *>(rs->LockIndexBuffer(ib));
    if (!index)
        return false;
    for (long i = 0, point = 0; i < GRASS_MAX_POINTS; i++, index += 6, point += 4)
    {
        index[0] = static_cast<uint16_t>(point + 0);
        index[1] = static_cast<uint16_t>(point + 1);
        index[2] = static_cast<uint16_t>(point + 2);
        index[3] = static_cast<uint16_t>(point + 2);
        index[4] = static_cast<uint16_t>(point + 1);
        index[5] = static_cast<uint16_t>(point + 3);
    }
    rs->UnLockIndexBuffer(ib);

    //Константы
    static const auto pi2 = 2.0f * 3.141592653f;
    for (size_t i = 0; i < 16; i++)
    {
        //Таблица углов
        aAngles[i] = {sinf(i * pi2 / 16.0f), cosf(i * pi2 / 16.0f), 0.0f};

        //Таблица uv
        aUV[i] = {static_cast<float>(i & 3) * (1.0f / 4.0f), static_cast<float>((i >> 2) & 3) * (1.0f / 4.0f)};
    }

    return true;
}

//Загрузить данные для травы из подготовленного файла
bool Grass::LoadData(const char *patchName)
{
    //Текстура травы
    texture = rs->TextureCreate(textureName);
    //Уталим старое
    delete miniMap;
    miniMap = nullptr;
    delete block;
    block = nullptr;
    //Загружаем файл с данными
    uint8_t *load = nullptr;
    uint32_t size = 0;
    if (!fio->LoadFile(patchName, (char **)&load, &size))
        return false;
    try
    {
        //Проверим данные
        if (size < sizeof(GRSHeader))
            throw "invalide file size";
        auto &hdr = *(GRSHeader *)load;
        if (hdr.id != GRASS_ID)
            throw "invalide file id";
        if (hdr.ver != GRASS_VER)
            throw "invalide file version";
        const auto minisize = hdr.miniX * hdr.miniZ;
        const auto elements = hdr.numElements;
        if (size != sizeof(GRSHeader) + minisize * sizeof(GRSMiniMapElement) + elements * sizeof(GRSMapElement))
            throw "incorrect file data -> file size";
        if (hdr.miniX <= 0 || hdr.miniX > 100000 || hdr.miniZ <= 0 || hdr.miniZ > 100000)
            throw "incorrect file data -> miniX, miniZ";
        //Создаём миникарту
        miniMap = new GRSMiniMapElement[minisize];
        memcpy(miniMap, load + sizeof(GRSHeader), minisize * sizeof(GRSMiniMapElement));
        miniX = hdr.miniX;
        miniZ = hdr.miniZ;
        //Последняя проверка
        for (long i = 0, pnt = 0; i < minisize; i++)
        {
            if (pnt != miniMap[i].start)
                throw "incorrect file data -> minimap";
            pnt += miniMap[i].num[0];
        }
        //Создаём блоки
        uint8_t translate[16];
        for (long i = 0; i < 16; i++)
        {
            translate[i] = static_cast<uint8_t>((i * 255) / 15);
        }
        block = new GRSMapElementEx[elements];
        auto *const src = (GRSMapElement *)(load + sizeof(GRSHeader) + minisize * sizeof(GRSMiniMapElement));
        for (long i = 0; i < elements; i++)
        {
            auto &sb = src[i];
            GRSMapElementEx &b = block[i];
            b.x = sb.x * GRASS_STEP;
            b.y = sb.y;
            b.z = sb.z * GRASS_STEP;
            b.frame = translate[sb.frame];
            b.h = translate[sb.h];
            b.w = translate[sb.w];
            b.ang = translate[sb.ang];
        }
        startX = hdr.startX;
        startZ = hdr.startZ;
        numElements = elements;
        //Корректируем позицию травинок из локальной в мировую
        for (long z = 0; z < miniZ; z++)
        {
            GRSMiniMapElement *line = &miniMap[z * miniX];
            const float cz = startZ + z * GRASS_BLK_DST;
            for (long x = 0; x < miniX; x++)
            {
                const float cx = startX + x * GRASS_BLK_DST;
                GRSMapElementEx *el = block + line[x].start;
                const long count = line[x].num[0];
                for (long i = 0; i < count; i++)
                {
                    el[i].x += cx;
                    el[i].z += cz;
                }
            }
        }
    }
    catch (const char *error)
    {
        api->Trace("Grass: incorrect grs file %s (%s)", patchName, error);
        delete miniMap;
        miniMap = nullptr;
        delete block;
        block = nullptr;
    }
    delete load;
    return true;
}

//Установить текстуру
void Grass::SetTexture(const char *texName)
{
    if (!texName || !texName[0] || strlen(texName) > 63)
    {
        strcpy_s(textureName, GRASS_DEFTEXTURE);
    }
    else
    {
        strcpy_s(textureName, texName);
    }
}

//Работа
void Grass::Execute(uint32_t delta_time)
{
    if (initForce < 20)
    {
        initForce++;
    }

    VDATA *param = api->Event("GOpt_GetGrassQuality", nullptr);
    long res = rq_full;
    if (param && param->Get(res))
    {
        if (res < rq_full)
            res = rq_full;
        if (res > rq_off)
            res = rq_off;
        quality = static_cast<RenderQuality>(res);
    }
    else
    {
        quality = rq_full;
    }
    if (quality == rq_off)
        return;
    //Параметры верта
    param = api->Event("EWhr_GetWindAngle", nullptr);
    if (param)
    {
        float ang;
        if (!param->Get(ang))
            ang = 0.0f;
        if (initForce < 20)
        {
            windAng = ang;
        }
        else
        {
            float k = delta_time * 0.001f * 0.001f;
            if (k > 1.0f)
                k = 1.0f;
            windAng += (ang - windAng) * k;
        }
        winDir.x = sinf(ang);
        winDir.z = cosf(ang);
    }
    param = api->Event("EWhr_GetWindSpeed", nullptr);
    if (param)
    {
        float spd;
        if (!param->Get(spd))
            spd = 0.0f;
        if (spd < 0.0f)
            spd = 0.0f;
        if (spd > 30.0f)
            spd = 30.0f;
        const float wf = powf(spd * (1.0f / 30.0f), 0.5f);
        if (initForce < 20)
        {
            initForce++;
            winForce = wf;
        }
        else
        {
            float k = delta_time * 0.001f * 0.001f;
            if (k > 1.0f)
                k = 1.0f;
            winForce += (wf - winForce) * k;
        }
        if (winForce < 0.0f)
            winForce = 0.0f;
        if (winForce > 1.0f)
            winForce = 1.0f;
    }
    //Фазы
    float dltTime = delta_time * 0.001f;
    if (dltTime > 0.05f)
        dltTime = 0.05f;
    phase[0] += dltTime * (0.3f + powf(winForce, 10.0f) * 0.0f);
    phase[1] += dltTime * 0.21f;
    phase[2] += dltTime * 0.01f;
    phase[3] += winDir.x * dltTime * (0.52f + powf(winForce, 3.0f) * 0.0f);
    phase[4] += winDir.z * dltTime * (0.55f + powf(winForce, 2.5f) * 0.0f);
    phase[5] += dltTime * 0.019f;
    phase[6] += dltTime * 0.057f;
    const float pi2 = 2.0f * 3.141592653f;
    for (long i = 0; i < sizeof(phase) / sizeof(phase[0]); i++)
        if (phase[i] > pi2 * 256.0f)
            phase[i] -= pi2 * 256.0f;
    //Коэфициенты для расчёта отклонений
    cosPh1 = 0.4f + 0.15f * cosf(phase[1]);
    sinPh2 = 0.51f + 0.09f * sinf(phase[2]);
    sinPh5 = 0.1f * sinf(phase[5]);
    sinPh6 = 0.01f * sinf(phase[6]);
    winPow = 5.0f - powf(winForce, 4.0f) * 4.5f;
    winF10 = powf(winForce, 10.0f) * 0.5f;
    kAmpWF = powf(winForce, 2.0f) * 0.8f;
    kDirWF = 0.8f * (1.0f - powf(winForce, 10.0f) * 0.999f);
    kLitWF = 0.7f * (1.0f - powf(winForce, 4.0f) * 0.7f);
}

void Grass::Realize(uint32_t delta_time)
{
    if (quality == rq_off || fx_ == nullptr)
        return;
    rs->SetTransform(D3DTS_WORLD, CMatrix());
    //Уберём текстуры
    rs->TextureSet(0, -1);
    rs->TextureSet(1, -1);
    //Стейты
    rs->SetRenderState(D3DRS_ZENABLE, false);
    rs->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    rs->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    rs->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    rs->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    rs->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    //Туман
    uint32_t dwOldFogDensity;
    rs->GetRenderState(D3DRS_FOGDENSITY, &dwOldFogDensity);
    entid_t eidIsland = EntityManager::GetEntityId("ISLAND");
    if (eidIsland)
    {
        auto fIslandFogDensity = static_cast<float>(dwOldFogDensity);
        ATTRIBUTES *pA = api->Entity_GetAttributePointer(eidIsland);
        if (pA)
            fIslandFogDensity = pA->GetAttributeAsFloat("FogDensity", 0.0f);
        rs->SetRenderState(D3DRS_FOGDENSITY, F2DW(fIslandFogDensity));
    }
    //Настроим стадии
    uint32_t cop, carg1, cop1, aop, aarg1;
    rs->GetTextureStageState(0, D3DTSS_COLOROP, &cop);
    rs->GetTextureStageState(1, D3DTSS_COLOROP, &cop1);
    rs->GetTextureStageState(0, D3DTSS_COLORARG1, &carg1);
    rs->GetTextureStageState(0, D3DTSS_ALPHAOP, &aop);
    rs->GetTextureStageState(0, D3DTSS_ALPHAARG1, &aarg1);

    rs->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    rs->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
    rs->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
    rs->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    rs->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);

    struct SphVertex
    {
        CVECTOR v;
        uint32_t c;
    };

    SphVertex lineVertex[2];
    lineVertex[0].c = 0xff009f00;
    lineVertex[1].c = 0xff00ffff;

    //Востановим стадии
    rs->SetTextureStageState(0, D3DTSS_COLOROP, cop);
    rs->SetTextureStageState(0, D3DTSS_COLORARG1, carg1);
    rs->SetTextureStageState(1, D3DTSS_COLOROP, cop1);
    rs->SetTextureStageState(0, D3DTSS_ALPHAOP, aop);
    rs->SetTextureStageState(0, D3DTSS_ALPHAARG1, aarg1);
    //Востановим стейты
    rs->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    rs->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    rs->SetRenderState(D3DRS_ZENABLE, TRUE);

    // if(api->Controls->GetDebugAsyncKeyState('H') < 0) return;

    //Если нет карты, то нет и рисования
    if (!block)
        return;
    //Параметры источника освещения
    BOOL isLight = FALSE;
    rs->GetLightEnable(0, &isLight);
    D3DLIGHT9 light;
    if (isLight && rs->GetLight(0, &light))
    {
        //Установленный источник
        //Направление
        lDir.x = light.Direction.x;
        lDir.y = 0.0f;
        lDir.z = light.Direction.z;
        float len = ~lDir;
        if (len > 0.0f)
            lDir *= 1.0f / sqrtf(len);
        else
            lDir = !CVECTOR(0.9f, 0.0f, 0.5f);
        //Цвет направления
        lColor.z = light.Diffuse.r;
        lColor.y = light.Diffuse.g;
        lColor.x = light.Diffuse.b;
        if (lColor.x < 0.0f)
            lColor.x = 0.0f;
        if (lColor.x > 1.0f)
            lColor.x = 1.0f;
        if (lColor.y < 0.0f)
            lColor.y = 0.0f;
        if (lColor.y > 1.0f)
            lColor.y = 1.0f;
        if (lColor.z < 0.0f)
            lColor.z = 0.0f;
        if (lColor.z > 1.0f)
            lColor.z = 1.0f;
        lColor.x = powf(lColor.x, 0.2f);
        lColor.y = powf(lColor.y, 0.2f);
        lColor.z = powf(lColor.z, 0.2f);
        //Рассеяный свет
        uint32_t aclr;
        if (rs->GetRenderState(D3DRS_AMBIENT, &aclr) != D3D_OK)
            aclr = 0xffffffff;
        aColor.z = static_cast<uint8_t>(aclr >> 16) * 1.0f / 255.0f;
        aColor.y = static_cast<uint8_t>(aclr >> 8) * 1.0f / 255.0f;
        aColor.x = static_cast<uint8_t>(aclr >> 0) * 1.0f / 255.0f;
        if (aColor.x > 1.0f)
            aColor.x = 1.0f;
        if (aColor.y > 1.0f)
            aColor.y = 1.0f;
        if (aColor.z > 1.0f)
            aColor.z = 1.0f;
        float y = aColor.z * 0.299f + aColor.y * 0.587f + aColor.x * 0.114f;
        aColor = y * 0.7f + aColor * 0.3f;
        aColor *= 0.8f;
    }
    else
    {
        //Источник по умолчанию
        //Рассеяный свет
        uint32_t aclr;
        if (rs->GetRenderState(D3DRS_AMBIENT, &aclr) != D3D_OK)
            aclr = 0xffffffff;
        aColor.z = static_cast<uint8_t>(aclr >> 16) * 1.0f / 255.0f;
        aColor.y = static_cast<uint8_t>(aclr >> 8) * 1.0f / 255.0f;
        aColor.x = static_cast<uint8_t>(aclr >> 0) * 1.0f / 255.0f;
        if (aColor.x > 1.0f)
            aColor.x = 1.0f;
        if (aColor.y > 1.0f)
            aColor.y = 1.0f;
        if (aColor.z > 1.0f)
            aColor.z = 1.0f;
        //Направление
        lDir = !CVECTOR(0.9f, 0.0f, 0.5f);
        //Цвет направления
        lColor = 0.3f;
    }

    //Пересчитаем параметры углов
    for (long i = 0; i < 16; i++)
    {
        aAngles[i].z = fabsf(-aAngles[i].y * lDir.x + aAngles[i].x * lDir.z);
        if (aAngles[i].z < 0.0f)
            aAngles[i].z = 0.0f;
        if (aAngles[i].z > 1.0f)
            aAngles[i].z = 1.0f;
    }

    //Матрица
    CMatrix view, prj;
    rs->GetTransform(D3DTS_VIEW, view);
    rs->GetTransform(D3DTS_PROJECTION, prj);
    CMatrix cmtx;
    cmtx.EqMultiply(view, prj);

    //Позиция камеры
    CVECTOR pos, ang;
    float prs;
    rs->GetCamera(pos, ang, prs);
    //Планы отсечения
    PLANE *pln = rs->GetPlanes();
    PLANE plane[5];
    plane[0].Nx = plane[0].Ny = plane[0].Nz = 0.0f;
    for (long i = 0; i < 4; i++)
    {
        plane[i + 1] = pln[i];
        plane[0].Nx += pln[i].Nx;
        plane[0].Ny += pln[i].Ny;
        plane[0].Nz += pln[i].Nz;
    }
    plane[0].Nx *= 0.25f;
    plane[0].Ny *= 0.25f;
    plane[0].Nz *= 0.25f;
    plane[0].D = pos.x * plane[0].Nx + pos.y * plane[0].Ny + pos.z * plane[0].Nz;
    long numPlanes = 5;
    //Ставим текстуру
    rs->TextureSet(0, texture);
    rs->TextureSet(1, texture);
    //Ставим константы
    fx_->SetMatrix(hgVP_, cmtx);
    fx_->SetValue(haAngles_, &aAngles[0], sizeof(D3DXVECTOR3) * 16);
    fx_->SetValue(haUV_, &aUV[0], sizeof(D3DXVECTOR2) * 16);
    fx_->SetValue(hlDir_, D3DXVECTOR2(lDir.x, lDir.z), sizeof(D3DXVECTOR2));
    fx_->SetValue(haColor_, D3DXVECTOR3(aColor.x, aColor.y, aColor.z), sizeof(D3DXVECTOR3));
    fx_->SetValue(hlColor_, D3DXVECTOR3(lColor.x, lColor.y, lColor.z), sizeof(D3DXVECTOR3));
    fx_->SetFloat(hkLitWF_, kLitWF);
    fx_->SetFloat(hfDataScale_, m_fDataScale);
    fx_->SetValue(haSize_, D3DXVECTOR2(m_fMaxWidth, m_fMaxHeight), sizeof(D3DXVECTOR2));

    //Позиция камеры на карте
    long camx = static_cast<long>((pos.x / m_fDataScale - startX) / GRASS_BLK_DST);
    long camz = static_cast<long>((pos.z / m_fDataScale - startZ) / GRASS_BLK_DST);
    //Квадрат, охватывающий зону видимости на карте
    long left = camx - GRASS_VEIW, right = camx + GRASS_VEIW;
    long top = camz - GRASS_VEIW, bottom = camz + GRASS_VEIW;
    //Клипуем размерами карты
    if (right < 0 || left >= miniX)
        return;
    if (bottom < 0 || top >= miniZ)
        return;
    if (left < 0)
        left = 0;
    if (right >= miniX)
        right = miniX - 1;
    if (top < 0)
        top = 0;
    if (bottom >= miniZ)
        bottom = miniZ - 1;

    if (camx < 0)
        camx = 0;
    if (camx >= miniX)
        camx = miniX - 1;
    if (camz < 0)
        camz = 0;
    if (camz >= miniZ)
        camz = miniZ - 1;
    //Готовим блоки для рендера
    numPoints = 0;
    rs->SetTransform(D3DTS_WORLD, CMatrix());

    // api->Trace("%d %d %d %d %d %d", left, top, bottom, right, camx, camz);
    /*for (long mx = left, mz; mx <= camx; mx++)
    {
      for (mz = top; mz <= camz; mz++) render(mz, mx);
      for (mz = bottom; mz > camz; mz--) render(mz, mx);
    }
    for (long mx = right, mz; mx > camx; mx--)
    {
      for (mz = top; mz <= camz; mz++) render(mz, mx);
      for (mz = bottom; mz > camz; mz--) render(mz, mx);
    }*/
    for (auto mx = left; mx < right; mx++)
    {
        for (auto mz = top; mz < bottom; mz++)
        {
            GRSMiniMapElement &mm = miniMap[mz * miniX + mx];

            //Проверяем наличие блока
            if (mm.num[0] != 0)
            {
                RenderBlock(pos, plane, numPlanes, mx, mz);
            }
        }
    }

    //Рисуем буфер
    DrawBuffer();

    rs->SetRenderState(D3DRS_FOGDENSITY, dwOldFogDensity);

    for (long i = 0; i < numCharacters; i++)
    {
        if (characters[i].useCounter > 2)
        {
            characters[i].chr->SetGrassSound();
        }
    }
}

uint64_t Grass::ProcessMessage(MESSAGE &message)
{
    char ctmp[MAX_PATH];
    switch (message.Long())
    {
    case MSG_GRASS_LOAD_DATA:
        message.String(sizeof(ctmp), ctmp);
        return LoadData(ctmp);
        break;
    case MSG_GRASS_SET_TEXTURE:
        message.String(sizeof(ctmp), ctmp);
        SetTexture(ctmp);
        break;
    case MSG_GRASS_SET_PARAM:
        m_fDataScale = message.Float();

        m_fMaxWidth = message.Float();
        m_fMaxHeight = message.Float();

        m_fMinVisibleDist = message.Float();
        m_fMaxVisibleDist = message.Float();

        m_fMinGrassLod = message.Float();
        return 0;
        break;
    }
    return 0;
}

//Отрисовать блок с координатами на миникарте
void Grass::RenderBlock(const CVECTOR &camPos, const PLANE *plane, long numPlanes, long mx, long mz)
{
    //Рисуем буфер, если переполнен
    if (numPoints >= GRASS_MAX_POINTS - (GRASS_CNT_MIN + GRASS_CNT_DLT) * 3)
    {
        DrawBuffer();
    }
    CVECTOR min, max;
    //Блок, который рисуем
    GRSMiniMapElement &mm = miniMap[mz * miniX + mx];
    //Дистанция от центра кластера (бокса) до камеры в 2D
    const float cx = m_fDataScale * (startX + (mx + 0.5f) * GRASS_BLK_DST);
    const float cz = m_fDataScale * (startZ + (mz + 0.5f) * GRASS_BLK_DST);
    const float dist = (cx - camPos.x) * (cx - camPos.x) + (cz - camPos.z) * (cz - camPos.z);
    //Ограничение по дальности
    if (dist >= m_fMaxVisibleDist * m_fMaxVisibleDist)
        return;
    //Определение видимости
    min.x = cx - 0.5f * GRASS_BLK_DST * m_fDataScale - 2.0f * m_fMaxWidth;
    min.y = mm.minHeight * m_fDataScale;
    min.z = cz - 0.5f * GRASS_BLK_DST * m_fDataScale - 2.0f * m_fMaxWidth;
    max.x = cx + 0.5f * GRASS_BLK_DST * m_fDataScale + 2.0f * m_fMaxWidth;
    max.y = mm.maxHeight * m_fDataScale;
    max.z = cz + 0.5f * GRASS_BLK_DST * m_fDataScale + 2.0f * m_fMaxWidth;
    if (!VisibleTest(plane, numPlanes, min, max))
        return;
    //Определение лода
    float kLod = (sqrtf(dist) - m_fMinVisibleDist) / (m_fMaxVisibleDist - m_fMinVisibleDist);
    // if(kLod < 0.0f) kLod = 0.0f;
    if (kLod < m_fMinGrassLod)
        kLod = m_fMinGrassLod;
    //Определим персонажей попадающих в текущий блок
    numBlockChr = 0;
    for (long i = 0; i < numCharacters; i++)
    {
        //Пропостим сильно выпадающих персонажей
        if (characters[i].pos.x + 0.9f < min.x)
            continue;
        if (characters[i].pos.x - 0.9f > max.x)
            continue;
        if (characters[i].pos.y + 0.9f < min.y)
            continue;
        if (characters[i].pos.y - 0.9f > max.y)
            continue;
        if (characters[i].pos.z + 0.9f < min.z)
            continue;
        if (characters[i].pos.z - 0.9f > max.z)
            continue;
        //Добавляем индекс
        blockChrs[numBlockChr++] = i;
    }
    //Рендер блока
    RenderBlock(mm, kLod);
}

//Проверка на видимость бокса
inline bool Grass::VisibleTest(const PLANE *plane, long numPlanes, const CVECTOR &min, const CVECTOR &max)
{
    for (long i = 0; i < numPlanes; i++)
    {
        const float d = plane[i].D;
        const float minX = min.x * plane[i].Nx;
        const float minY = min.y * plane[i].Ny;
        const float minZ = min.z * plane[i].Nz;
        const float maxX = max.x * plane[i].Nx;
        const float maxY = max.y * plane[i].Ny;
        const float maxZ = max.z * plane[i].Nz;
        if (minX + minY + minZ >= d)
            continue;
        if (minX + maxY + minZ >= d)
            continue;
        if (maxX + maxY + minZ >= d)
            continue;
        if (maxX + minY + minZ >= d)
            continue;
        if (minX + minY + maxZ >= d)
            continue;
        if (minX + maxY + maxZ >= d)
            continue;
        if (maxX + maxY + maxZ >= d)
            continue;
        if (maxX + minY + maxZ >= d)
            continue;
        return false;
    }
    return true;
}

//Нарисовать блок
inline void Grass::RenderBlock(GRSMiniMapElement &mme, float kLod)
{
    //Защитимся от себя
    if (!vbuffer)
    {
        vbuffer = static_cast<Vertex *>(rs->LockVertexBuffer(vb));
        if (!vbuffer)
            return;
    }
    //Позиция в массиве вершин
    Vertex *v = vbuffer + numPoints * 4;
    //Блоки
    GRSMapElementEx *b = block + mme.start;
    //Определяем параметры лода
    kLod = kLod * 3.9999f;
    long lod = static_cast<long>(kLod);
    if (lod < quality)
        lod = quality;
    Assert(lod >= 0 && lod < 4);
    float kBlend = 1.0f - (kLod - lod);
    if (kBlend > 1.0f)
        kBlend = 1.0f;
    if (kBlend < 0.0f)
        kBlend = 0.0f;
    //Количество травинок всего
    const long num = mme.num[lod];
    Assert(num <= GRASS_CNT_MIN + GRASS_CNT_DLT);
    //Количество рисуемое без лодирования
    const long lodNum = lod < 3 ? mme.num[lod + 1] : 0;
    //Ветренное дополнение
    float wAddX, wAddZ, kwDirX, kwDirZ;
    if (quality <= rq_middle)
    {
        wAddX = winDir.x * winForce * (1.0f + cosf(phase[1] + sinPh5)) * 0.25f;
        wAddZ = winDir.z * winForce * (1.0f + cosf(phase[1] + sinPh5)) * 0.25f;
        kwDirX = winDir.x * kDirWF;
        kwDirZ = winDir.z * kDirWF;
    }
    else
    {
        wAddX = 0.01f * winForce + 0.1f * winForce * winForce + 2.0f * winF10;
        if (wAddX > 1.0f)
            wAddX = 1.0f;
    }
    //Цикл по травинкам
    float alpha;
    for (long i = 0; i < num; i++)
    {
        //Альфа
        if (i < lodNum)
        {
            alpha = 1.0f;
        }
        else
        {
            alpha = kBlend;
        }
        //Колыхание травы
        float winx = sinf(b[i].x * cosPh1 + b[i].z * 0.06f + phase[0]);
        float winz = cosf(b[i].x * 0.11f + b[i].z * sinPh2 + phase[0]);
        if (quality <= rq_middle)
        {
            //Позиция
            const float x = b[i].x;
            const float y = b[i].y;
            const float z = b[i].z;
            //Волны ветра
            const float dx = winDir.x * x * 0.5f + phase[3];
            const float dz = winDir.z * z * 0.5f + phase[4];
            const float k1 = sinf(dx + dz);
            const float a1 = (0.001f + sinPh5 * sinf((dx + dz) * 0.5f));
            const float k2 = cosf(winDir.z * x * (a1 + sinPh6) - winDir.x * z * a1);
            float kamp = powf(k1 * k2 * 0.5f + 0.5f, winPow) + winF10 + winForce * 0.7f;
            if (kamp > 1.0f)
                kamp = 1.0f;
            //Результирующий вектор смещения
            kamp *= kAmpWF;
            winx = (0.9f * winx + kwDirX) * kamp + wAddX;
            winz = (0.9f * winz + kwDirZ) * kamp + wAddZ;
            //Учтём персонажей
            for (long chr = 0; chr < numBlockChr; chr++)
            {
                CharacterPos &cp = characters[blockChrs[chr]];
                if (fabsf(cp.pos.y - y) < 0.7f)
                {
                    float pldx = ((x - cp.pos.x) + (x - cp.lastPos.x)) * 0.5f;
                    float pldz = ((z - cp.pos.z) + (z - cp.lastPos.z)) * 0.5f;
                    float dst = pldx * pldx + pldz * pldz;
                    if (dst < 0.8f * 0.8f && dst > 0.0f)
                    {
                        //Воздействуем
                        if (dst > 0.0f)
                        {
                            dst = sqrtf(dst);
                            const float k = 0.5f / dst;
                            pldx *= k;
                            pldz *= k;
                        }
                        dst *= 1.0f / 0.8f;
                        dst = 1.0f - dst;
                        dst *= dst;
                        winx += (pldx - winx) * dst;
                        winz += (pldz - winz) * dst;
                        cp.useCounter++;
                    }
                }
            }
            float wLen = sqrtf(winx * winx + winz * winz);
            if (wLen > 1.2f)
            {
                wLen = 1.2f / wLen;
                winx *= wLen;
                winz *= wLen;
            }
            winx *= 0.4f;
            winz *= 0.4f;
        }
        else
        {
            winx *= wAddX;
            winz *= wAddX;
        }
        //Исходные данные
        GRSMapElementEx &el = b[i];
        //Координаты
        v[0].x = el.x;
        v[0].y = el.y;
        v[0].z = el.z;
        v[0].data = el.data;
        v[0].offset = 0x00000000;
        v[0].wx = winx;
        v[0].wz = winz;
        v[0].alpha = alpha;

        v[1].x = el.x;
        v[1].y = el.y;
        v[1].z = el.z;
        v[1].data = el.data;
        v[1].offset = 0x00ff0000;
        v[1].wx = winx;
        v[1].wz = winz;
        v[1].alpha = alpha;

        v[2].x = el.x;
        v[2].y = el.y;
        v[2].z = el.z;
        v[2].data = el.data;
        v[2].offset = 0x0000ff00;
        v[2].wx = winx;
        v[2].wz = winz;
        v[2].alpha = alpha;

        v[3].x = el.x;
        v[3].y = el.y;
        v[3].z = el.z;
        v[3].data = el.data;
        v[3].offset = 0x00ffff00;
        v[3].wx = winx;
        v[3].wz = winz;
        v[3].alpha = alpha;

        v += 4;
        numPoints++;
    }
}

//Нарисовать содержимое буфера
void Grass::DrawBuffer()
{
    if (vbuffer)
    {
        rs->UnLockVertexBuffer(vb);
        vbuffer = nullptr;
    }
    // boal выбор шайдера -->
    if (numPoints > 0)
    {
        rs->SetVertexDeclaration(vertexDecl_);
        if (isGrassLightsOn == 1)
        {
            rs->DrawBuffer(vb, sizeof(Vertex), ib, 0, numPoints * 4, 0, numPoints * 2, "Grass");
        }
        else
        {
            rs->DrawBuffer(vb, sizeof(Vertex), ib, 0, numPoints * 4, 0, numPoints * 2, "GrassDark");
        }
        // boal выбор шайдера <--
        numPoints = 0;
    }
}

long Grass::GetColor(CVECTOR color)
{
    if (color.x > 1.0f)
        color.x = 1.0f;
    if (color.x < 0.0f)
        color.x = 0.0f;
    if (color.y > 1.0f)
        color.y = 1.0f;
    if (color.y < 0.0f)
        color.y = 0.0f;
    if (color.z > 1.0f)
        color.z = 1.0f;
    if (color.z < 0.0f)
        color.z = 0.0f;
    const long r = static_cast<long>(color.z * 255.0f);
    const long g = static_cast<long>(color.y * 255.0f);
    const long b = static_cast<long>(color.x * 255.0f);
    return (r << 16) | (g << 8) | b;
}

void Grass::CreateVertexDeclaration() const
{
    if (vertexDecl_ != nullptr)
    {
        return;
    }

    const D3DVERTEXELEMENT9 VertexElements[] = {
        {0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
        {0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
        {0, 16, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 1},
        {0, 20, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
        D3DDECL_END()};

    rs->CreateVertexDeclaration(VertexElements, &vertexDecl_);

    fx_ = rs->GetEffectPointer("Grass");
    if (fx_ != nullptr)
    {
        hgVP_ = fx_->GetParameterByName(nullptr, "gVP");
        haAngles_ = fx_->GetParameterByName(nullptr, "aAngles");
        haUV_ = fx_->GetParameterByName(nullptr, "aUV");
        hlDir_ = fx_->GetParameterByName(nullptr, "lDir");
        hkLitWF_ = fx_->GetParameterByName(nullptr, "kLitWF");
        haColor_ = fx_->GetParameterByName(nullptr, "aColor");
        hlColor_ = fx_->GetParameterByName(nullptr, "lColor");
        hfDataScale_ = fx_->GetParameterByName(nullptr, "fDataScale");
        haSize_ = fx_->GetParameterByName(nullptr, "aSize");
    }
}
