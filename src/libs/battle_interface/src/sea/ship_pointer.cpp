#include "ship_pointer.h"
#include "../bi_defines.h"
#include "entity.h"
#include "core.h"
#include "math_inlines.h"
#include "message.h"
#include "shared/battle_interface/msg_control.h"
#include "ship_base.h"

// define the vertices
#define SPV_FORMAT (D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXTUREFORMAT2)

struct SPV_VERTEX
{
    CVECTOR pos;
    float tu, tv;
};

SHIPPOINTER::SHIPPOINTER()
{
    m_bVisible = false;
    m_idFriendTex = -1;
    m_idEnemyTex = -1;
    m_idVBuf = -1;
}

SHIPPOINTER::~SHIPPOINTER()
{
    m_bVisible = false;
    TEXTURE_RELEASE(rs, m_idFriendTex);
    TEXTURE_RELEASE(rs, m_idEnemyTex);
    VERTEX_BUFFER_RELEASE(rs, m_idVBuf);
}

bool SHIPPOINTER::Init()
{
    if ((rs = static_cast<VDX9RENDER *>(core.GetService("dx9render"))) == nullptr)
    {
        throw std::runtime_error("Can`t create render service");
    }

    m_idVBuf = rs->CreateVertexBuffer(SPV_FORMAT, 4 * sizeof(SPV_VERTEX), D3DUSAGE_WRITEONLY);
    if (m_idVBuf < 0)
        return false;

    auto *pv = static_cast<SPV_VERTEX *>(rs->LockVertexBuffer(m_idVBuf));
    if (pv != nullptr)
    {
        pv[0].tu = 0.f;
        pv[0].tv = 0.f;
        pv[1].tu = 0.f;
        pv[1].tv = 1.f;
        pv[2].tu = 1.f;
        pv[2].tv = 0.f;
        pv[3].tu = 1.f;
        pv[3].tv = 1.f;
        rs->UnLockVertexBuffer(m_idVBuf);
    }

    auto *pA = core.Entity_GetAttributeClass(GetId(), "textures");
    if (pA == nullptr)
    {
        core.Trace("WARNING! object SHIPPOINTER hav`t attribute TEXTURES");
        return false;
    }

    //"battle_interface\\shippointer.tga");
    m_idFriendTex = rs->TextureCreate(pA->GetAttribute("friend"));
    m_idEnemyTex = rs->TextureCreate(pA->GetAttribute("enemy"));

    return true;
}

void SHIPPOINTER::Execute(uint32_t delta_time)
{
    if (!m_bVisible)
        return;

    if (m_pShip != nullptr)
    {
        m_fShiftVal += m_fShiftSpeed * delta_time;
        while (m_fShiftVal > PIm2)
            m_fShiftVal -= PIm2;
        UpdateShipPointer();
    }
}

void SHIPPOINTER::Realize(uint32_t delta_time) const
{
    if (!m_bVisible)
        return;

    CMatrix matw;
    rs->SetTransform(D3DTS_WORLD, matw);

    if (m_bFriend)
        rs->TextureSet(0, m_idFriendTex);
    else
        rs->TextureSet(0, m_idEnemyTex);

    rs->DrawPrimitive(D3DPT_TRIANGLESTRIP, m_idVBuf, sizeof(SPV_VERTEX), 0, 2, "battle_shippointer");
}

uint64_t SHIPPOINTER::ProcessMessage(MESSAGE &message)
{
    switch (message.Long())
    {
    case MSG_SP_CHANGESHIP: {
        const auto chrIdx = message.Long();
        m_pShip = FindShipByChrIndex(chrIdx);
        if (m_pShip == nullptr)
        {
            m_bVisible = false;
            return 0;
        }
        m_bVisible = true;
        m_bFriend = message.Long() != 0;
        m_fShiftVal = 0.f;
        m_fShiftSpeed = message.Float();
        m_fShiftAmp = message.Float();
        m_fShiftWSize = message.Float();
        m_fShiftHSize = message.Float();
        m_fShiftTop = message.Float();

        m_fShiftTop += static_cast<SHIP_BASE *>(m_pShip)->State.vRealBoxSize.y;
    }
    break;
    }

    return 0;
}

void SHIPPOINTER::UpdateShipPointer() const
{
    if (!m_bVisible)
        return;
    if (m_pShip == nullptr)
        return;

    auto *pv = static_cast<SPV_VERTEX *>(rs->LockVertexBuffer(m_idVBuf));
    if (pv != nullptr)
    {
        CVECTOR campos, camang;
        float camper;
        rs->GetCamera(campos, camang, camper);

        auto ourPos = m_pShip->GetPos() + CVECTOR(0.f, m_fShiftTop + m_fShiftAmp * sinf(m_fShiftVal), 0.f);
        auto k = camper * .05f * sqrtf(~(campos - ourPos));
        if (k < 1.f)
            k = 1.f;

        const auto cvhorz = k * .5f * !((campos - ourPos) ^ CVECTOR(0.f, 1.f, 0.f));
        const CVECTOR cvvert = k * CVECTOR(0.f, 1.f, 0.f);

        ourPos.y += k * 1.0f;

        pv[0].pos = ourPos - cvhorz + cvvert;
        pv[1].pos = ourPos - cvhorz;
        pv[2].pos = ourPos + cvhorz + cvvert;
        pv[3].pos = ourPos + cvhorz;

        rs->UnLockVertexBuffer(m_idVBuf);
    }
}

VAI_OBJBASE *SHIPPOINTER::FindShipByChrIndex(int32_t chrIdx) const
{
    if (chrIdx == -1)
        return nullptr;

    auto &&entities = core.GetEntityIds("ship");
    for (auto ship : entities)
    {
        auto ps = static_cast<VAI_OBJBASE *>(core.GetEntityPointer(ship));
        if (ps != nullptr && ps->GetACharacter() != nullptr)
        {
            if (static_cast<int32_t>(ps->GetACharacter()->GetAttributeAsDword("index", -2)) == chrIdx)
                return ps;
        }
    }

    /*if( NetFindClass(false,&ei,"netship") ) do
    {
      VAI_OBJBASE * ps = (VAI_OBJBASE*)core.GetEntityPointer(ei);
      if(ps!= nullptr && ps->GetACharacter()!= nullptr)
      {
        if( (int32_t)ps->GetACharacter()->GetAttributeAsDword("id",-2) == chrIdx )
          return ps;
      }
    } while( NetFindClassNext(false,&ei) );*/

    return nullptr;
}
