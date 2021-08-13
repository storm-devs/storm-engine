#include "teleport.h"

#include "core.h"

#include "defines.h"
#include "pcs_controls.h"
#include "vfile_service.h"

CREATE_CLASS(TMPTELEPORT)

CREATE_CLASS(FINDFILESINTODIRECTORY)

CREATE_CLASS(FINDDIALOGNODES)

#define DELETE_PTR(x)                                                                                                  \
    if (x)                                                                                                             \
        delete x;                                                                                                      \
    x = 0;

bool GetStringLine(char *&pStr, char *bufer, long bufSize)
{
    if (pStr == nullptr || bufer == nullptr || bufSize == 0)
        return false;
    bufer[0] = 0;

    auto *ps = pStr;
    while (*ps && (*ps == 32 || *ps == 9 || *ps == 10 || *ps == 13))
        ps++;
    auto *const pStart = ps;

    while (*ps && *ps != 10 && *ps != 13)
        ps++;
    auto *pEnd = ps;
    pStr = pEnd;

    if (pEnd == pStart && *ps == 0)
        return false;
    if (static_cast<long>(pEnd - pStart) > bufSize - 1)
        pEnd = pStart + bufSize - 1;
    else
        bufSize = pEnd - pStart;

    if (bufSize > 0)
    {
        strcpy_s(bufer, bufSize, pStart);
        bufer[bufSize] = 0;
    }
    return true;
}

void GetQuotedString(char *inBuf, char *outBuf, long bufSize)
{
    if (outBuf == nullptr || bufSize <= 0)
        return;
    *outBuf = 0;
    if (inBuf == nullptr)
        return;

    while (*inBuf && *inBuf != '\"')
        inBuf++;
    if (*inBuf)
        inBuf++;

    auto bufIdx = 0;
    while (*inBuf && *inBuf != '\"' && bufIdx < bufSize - 1)
    {
        *outBuf = *inBuf;
        outBuf++;
        inBuf++;
        bufIdx++;
    }
    *outBuf = 0;
}

TMPTELEPORT::TMPTELEPORT()
    : m_leftPos(0), m_topPos(0), m_deltaPos(0), m_showStrQuantity(0), m_nShowType(0)
{
    rs = nullptr;
    m_descrArray = nullptr;
    m_nStrQuantity = m_nCurStr = m_nCurShowPos = 0;
}

TMPTELEPORT::~TMPTELEPORT()
{
    ReleaseAll();
}

bool TMPTELEPORT::Init()
{
    rs = static_cast<VDX9RENDER *>(core.CreateService("dx9render"));
    if (!rs)
        throw std::runtime_error("No service: dx9render");

    m_leftPos = 20;
    m_topPos = 80;
    m_deltaPos = 30;
    m_showStrQuantity = 10;

    m_nShowType = 0;

    return true;
}

void TMPTELEPORT::Execute(uint32_t Delta_Time)
{
    CONTROL_STATE cs;
    if (static_cast<PCS_CONTROLS *>(core.Controls)->m_bIsOffDebugKeys)
        return;
    core.Controls->GetControlState("TeleportActive", cs);
    if (cs.state == CST_ACTIVATED)
    {
        if (m_nShowType == 0)
        {
            core.Event("TeleportStart", "");
            m_nShowType = 1;
        }
        else
        {
            ReleaseAll();
            m_nShowType = 0;
        }
    }
    long csVal;
    if (core.Controls->GetDebugAsyncKeyState(VK_SHIFT) < 0)
        csVal = CST_ACTIVE;
    else
        csVal = CST_ACTIVATED;
    core.Controls->GetControlState("TeleportUp", cs);
    if (cs.state == csVal)
    {
        if (m_nStrQuantity > 0)
        {
            if (m_nCurShowPos > 0)
                m_nCurShowPos--;
            else if (m_nCurStr > 0)
                m_nCurStr--;
        }
    }
    core.Controls->GetControlState("TeleportDown", cs);
    if (cs.state == csVal)
    {
        if (m_nStrQuantity > 0)
        {
            if (m_nCurStr + m_nCurShowPos < m_nStrQuantity - 1)
                if (m_nCurShowPos < m_showStrQuantity - 1)
                    m_nCurShowPos++;
                else
                    m_nCurStr++;
        }
    }
    core.Controls->GetControlState("TeleportSelect", cs);
    if (cs.state == CST_ACTIVATED)
    {
        if (m_nStrQuantity > 0)
        {
            const long n = m_descrArray[m_nCurStr + m_nCurShowPos].num;
            ReleaseAll();
            core.Event("TeleportChoose", "l", n);
        }
    }
}

void TMPTELEPORT::Realize(uint32_t Delta_Time)
{
    if (m_nStrQuantity > 0)
    {
        auto j = 0;
        auto ftop = m_topPos;
        for (int i = m_nCurStr; i < m_nStrQuantity; i++)
        {
            if (j >= m_showStrQuantity)
                break;
            if (j == m_nCurShowPos)
                rs->Print(FONT_DEFAULT, ARGB(255, 155, 155, 55), m_leftPos, ftop, "%s", m_descrArray[i].name);
            else
                rs->Print(FONT_DEFAULT, ARGB(255, 255, 255, 255), m_leftPos, ftop, "%s", m_descrArray[i].name);
            ftop += m_deltaPos;
            j++;
        }
    }
}

void TMPTELEPORT::ReleaseAll()
{
    if (m_descrArray != nullptr)
    {
        for (auto i = 0; i < m_nStrQuantity; i++)
        {
            DELETE_PTR(m_descrArray[i].name);
        }
    }
    DELETE_PTR(m_descrArray);
    m_nStrQuantity = 0;
    m_nCurStr = m_nCurShowPos = 0;
    m_nShowType = 0;
}

uint64_t TMPTELEPORT::ProcessMessage(MESSAGE &message)
{
    switch (message.Long())
    {
    case 42222: {
        auto *const pA = message.AttributePointer();
        SetShowData(pA);
        if (m_nStrQuantity == 0)
            m_nShowType = 0;
        else
            m_nShowType = 1;
    }
    break;
    }
    return 0;
}

void TMPTELEPORT::SetShowData(ATTRIBUTES *pA)
{
    ReleaseAll();
    m_nStrQuantity = 0;
    if (pA == nullptr)
        return;
    m_nStrQuantity = pA->GetAttributesNum();
    if (m_nStrQuantity == 0)
        return;
    if ((m_descrArray = new TELEPORT_DESCR[m_nStrQuantity]) == nullptr)
    {
        throw std::runtime_error("Allocate memory error");
    }

    for (auto i = 0; i < m_nStrQuantity; i++)
    {
        auto *const tmpStr = pA->GetAttribute(i);
        m_descrArray[i].name = nullptr;
        m_descrArray[i].num = i;
        if (tmpStr == nullptr)
            continue;
        const auto len = strlen(tmpStr) + 1;
        if ((m_descrArray[i].name = new char[len]) == nullptr)
        {
            throw std::runtime_error("Allocate memory error");
        }
        memcpy(m_descrArray[i].name, tmpStr, len);
    }

    SortShowData();
}

void TMPTELEPORT::SortShowData()
{
    if (m_nStrQuantity == 0)
        return;
    auto bContinueSort = true;
    do
    {
        bContinueSort = false;
        for (auto i = 1; i < m_nStrQuantity; i++)
        {
            if (m_descrArray[i - 1].name == nullptr)
                continue;
            if (m_descrArray[i].name == nullptr)
            {
                XChange(m_descrArray[i - 1], m_descrArray[i]);
                bContinueSort = true;
                continue;
            }
            if (strcmp(m_descrArray[i].name, m_descrArray[i - 1].name) < 0)
            {
                XChange(m_descrArray[i - 1], m_descrArray[i]);
                bContinueSort = true;
            }
        }
    } while (bContinueSort);
}

void TMPTELEPORT::XChange(TELEPORT_DESCR &d1, TELEPORT_DESCR &d2)
{
    const auto n = d1.num;
    d1.num = d2.num;
    d2.num = n;

    auto *const nm = d1.name;
    d1.name = d2.name;
    d2.name = nm;
}

bool FINDFILESINTODIRECTORY::Init()
{
    if (AttributesPointer)
    {
        auto *const dirName = AttributesPointer->GetAttribute("dir");
        auto *const maskName = AttributesPointer->GetAttribute("mask");
        const char *curMask;
        if (maskName)
        {
            curMask = maskName;
        }
        else
        {
            curMask = "*.*";
        }
        auto file_idx = 0;
        auto *pA = AttributesPointer->CreateSubAClass(AttributesPointer, "filelist");
        const auto vFilenames = fio->_GetPathsOrFilenamesByMask(dirName, curMask, false);
        for (std::string filename : vFilenames)
        {
            char sname[32];
            sprintf(sname, "id%d", file_idx);
            pA->SetAttribute(sname, filename.c_str());
            file_idx++;
        }
        return true;
    }
    core.Trace("Attributes Pointer into class FINDFILESINTODIRECTORY = NULL");
    return false;
}

bool FINDDIALOGNODES::Init()
{
    if (AttributesPointer)
    {
        auto *const fileName = AttributesPointer->GetAttribute("file");
        auto *pA = AttributesPointer->CreateSubAClass(AttributesPointer, "nodelist");
        if (fileName && pA)
        {
            auto fileS = fio->_CreateFile(fileName, std::ios::binary | std::ios::in);
            if (!fileS.is_open())
            {
                core.Trace("WARNING! Can`t dialog file %s", fileName);
                return false;
            }

            const long filesize = fio->_GetFileSize(fileName);
            if (filesize == 0)
            {
                core.Trace("Empty dialog file %s", fileName);
                fio->_CloseFile(fileS);
                return false;
            }

            auto *const fileBuf = new char[filesize + 1];
            if (fileBuf == nullptr)
            {
                core.Trace("Can`t create buffer for read dialog file %s", fileName);
                fio->_CloseFile(fileS);
                return false;
            }

            if (!fio->_ReadFile(fileS, fileBuf, filesize))
            {
                core.Trace("Can`t read dialog file: %s", fileName);
                fio->_CloseFile(fileS);
                delete[] fileBuf;
                return false;
            }
            fio->_CloseFile(fileS);
            fileBuf[filesize] = 0;

            // now there is a buffer - start analyzing it
            auto *pStr = fileBuf;
            char param[1024];

            auto nodIdx = 0;
            while (GetStringLine(pStr, param, sizeof(param) - 1))
            {
                if (strlen(param) < 5 || _strnicmp(param, "case", 4))
                    continue;
                char param2[512];
                GetQuotedString(param, param2, sizeof(param2) - 1);
                if (strlen(param2) > 0)
                {
                    sprintf_s(param, "id%d", nodIdx);
                    nodIdx++;
                    pA->SetAttribute(param, param2);
                }
            }

            delete[] fileBuf;
            return true;
        }
    }
    core.Trace("Attributes Pointer into class FINDDIALOGNODES = NULL");
    return false;
}
