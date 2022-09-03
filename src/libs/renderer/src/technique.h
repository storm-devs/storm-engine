#ifndef _WIN32 // Effects
#pragma once

#include <unordered_map>

#include "dx9render.h"

struct SRSPARAM
{
    char *cName;
    uint32_t dwCode;

    SRSPARAM(const char *cName, uint32_t dwCode)
    {
        this->cName = strdup(cName);
        this->dwCode = dwCode;
    }
};

struct pass_t
{
    bool isValidate; // check for validate this pass for current device
    uint32_t dwSize; // size of full pass decode buffer
    uint32_t *pPass; // pass decode buffer
};

struct technique_t
{
    uint32_t dwNumPasses; // number of passes
    pass_t *pPasses;
};

struct block_t
{
    uint32_t dwHashBlockName; // hash code for block name
    char *pBlockName;         // block name

    // parameters section
    uint32_t dwNumParams; // number of parameters
    uint32_t *pParams;    // parameters block

    // techniques section
    uint32_t dwNumTechniques; // number of techniques
    technique_t *pTechniques;
};

struct shader_t // pixel/vertex shader structure
{
    char *pName;         // shader name
    uint32_t dwHashName; // hash code for shader name
    uint32_t dwShaderType;

    D3DVERTEXELEMENT9 *pDecl; // declarations for vertex shader
    IDirect3DVertexDeclaration9 *pVertexDecl;
    IDirect3DVertexShader9 *pVertexShader;
    IDirect3DPixelShader9 *pPixelShader;
    uint32_t dwDeclSize;
    // uint32_t	dwShaderHandle;		// shader handle
};

struct define_t
{
    char *pName;
    uint32_t dwNameLen;
    char *pValue;
};

struct shader_find_t
{
    char *pName;
    uint32_t *pDest;
};

class CTechnique
{
  protected:
    VDX9RENDER *pRS;
    IDirect3DDevice9 *d3d9;

    // shaders
    uint32_t dwNumShaders;
    shader_t *pShaders;

    // blocks
    uint32_t dwNumBlocks;
    block_t *pBlocks;
    std::unordered_map<std::string, uint32_t> htBlocks;

    // temporary used
    uint32_t dwNumParams;
    SRSPARAM *pParams;
    uint32_t *pPassStorage;

    // executed technique section

    uint32_t *pCurParams;
    uint32_t dwCurParamsMax; //

    uint32_t dwHashCode;     // current block name
    uint32_t dwCurNumParams; // current parameters count

    uint32_t dwCurBlock;     // current(executed) block
    uint32_t dwCurNumPass;   // number of passes in current technique
    uint32_t dwCurTechnique; // current technique
    uint32_t dwCurPass;      // current pass of current technique
    uint32_t dwCurPassSize;  // current pass size
    uint32_t dwCurPassPos;   // current pass position

    char sCurrentBlockName[256]; // current block name
    char sCurrentFileName[256];  // current block name
    char sCurrentDir[256];       // current directory
    char sDelimTable[256];

    // saved states section

    uint32_t dwNumSavedStates;    //
    uint32_t dwCurSavedStatesPos; //
    uint32_t dwCurMaxSavedSize;   //
    uint32_t *pSavedStates;       // saved states

    uint32_t ProcessTechnique(char *pFile, uint32_t dwSize, char **pStr);
    uint32_t ProcessBlock(char *pFile, uint32_t dwSize, char **pStr);
    uint32_t ProcessPass(char *pFile, uint32_t dwSize, char **pStr);

    uint32_t ProcessVertexShader(char *pFile, uint32_t dwSize, char **pStr);
    uint32_t ProcessVertexDeclaration(shader_t *pS, char *pFile, uint32_t dwSize, char **pStr);
    uint32_t ProcessPixelShader(char *pFile, uint32_t dwSize, char **pStr);
    uint32_t ProcessShaderAsm(shader_t *pS, char *pFile, uint32_t dwSize, char **pStr, uint32_t dwShaderType,
                              bool HLSL = false);
    void GetShaderBinPath(char *pShaderStr, uint32_t dwShaderType, char *dest);
    uint32_t ProcessShaderBin(shader_t *pS, char *pFile, uint32_t dwShaderType);

    uint32_t AddShader(char *pShaderName);
    char *Preprocessor(char *pBuffer, uint32_t &dwSize);

    uint32_t GetPassParameter(uint32_t dwParam, uint32_t dwFlags);

    void RestoreSavedStates();
    void ClearSavedStates();
    void AddState2Restore(uint32_t dwState);
    void AddState2Restore2(uint32_t dwState, uint32_t dw1);
    void AddState2Restore3(uint32_t dwState, uint32_t dw1, uint32_t dw2);

    bool ExecutePassClose();

    uint32_t GetCode(char *pStr, SRSPARAM *pParam, uint32_t dwNumParam, uint32_t *pPassCode = nullptr,
                     bool bCanBeNumber = false);
    uint32_t GetSRSIndex(char *pStr);
    uint32_t GetSTSSIndex(char *pStr);
    uint32_t GetSAMPIndex(char *pStr);
    uint32_t GetIndex(char *pStr, SRSPARAM *pParam, uint32_t dwNumParam, bool bCanBeNumber);

    char *GetToken(char *pToken, char *pResult, bool &bToken);
    void InnerDecodeFiles(char *sub_dir = nullptr);

    void ClearSRS_STSS_bUse();

  public:
    void SetCurrentBlock(const char *name, uint32_t _dwNumParams, void *pParams);

    bool DecodeFile(std::string sname);
    void DecodeFiles(char *sub_dir = nullptr);

    bool ExecutePassStart();
    bool ExecutePassNext();
    bool ExecutePass(bool bStart);

    CTechnique(VDX9RENDER *_pRS);
    ~CTechnique();
};

#endif // _WIN32
