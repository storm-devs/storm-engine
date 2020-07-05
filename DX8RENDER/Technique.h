#ifndef TECHNIQUE_HPP
#define TECHNIQUE_HPP

#include <stdio.h>
#include "..\common_h\vmodule_api.h"
#include "..\common_h\dx8render.h"
#include "..\common_h\templates\string.h"
#include "..\common_h\templates\htable.h"

struct SRSPARAM
{
	char		* cName;
	dword		dwCode;
};

struct pass_t
{
	bool	isValidate;		// check for validate this pass for current device
	dword	dwSize;			// size of full pass decode buffer
	dword	*pPass;			// pass decode buffer
};

struct technique_t
{
	dword	dwNumPasses;		// number of passes
	pass_t	*pPasses;
};

struct block_t
{
	dword			dwHashBlockName;	// hash code for block name
	char			*pBlockName;		// block name
	
	// parameters section
	dword			dwNumParams;		// number of parameters
	dword			*pParams;			// parameters block

	// techniques section
	dword			dwNumTechniques;	// number of techniques
	technique_t		*pTechniques;
};

struct shader_t		// pixel/vertex shader structure
{
	char	*pName;				// shader name
	dword	dwHashName;			// hash code for shader name
	dword	dwShaderType;

	dword	*pDecl;				// declarations for vertex shader
	dword	dwDeclSize;
	dword	dwShaderHandle;		// shader handle
};

struct define_t
{
	char	*pName;
	dword	dwNameLen;
	char	*pValue;
};

struct shader_find_t
{
	char	*pName;
	dword	*pDest;
};

class CTechnique
{
protected:
	VDX8RENDER			* pRS;
	IDirect3DDevice8	* d3d8;

	// shaders
	dword			dwNumShaders;
	shader_t		*pShaders;

	// blocks
	dword			dwNumBlocks;
	block_t			* pBlocks;
	htable<dword>	htBlocks;

// temporary used
	dword		dwNumParams;
	SRSPARAM	* pParams;
	dword		* pPassStorage;
	
// executed technique section

	dword		*pCurParams;
	dword		dwCurParamsMax;		//

	dword		dwHashCode;			// current block name
	dword		dwCurNumParams;		// current parameters count

	dword		dwCurBlock;			// current(executed) block
	dword		dwCurNumPass;		// number of passes in current technique
	dword		dwCurTechnique;		// current technique
	dword		dwCurPass;			// current pass of current technique
	dword		dwCurPassSize;		// current pass size
	dword		dwCurPassPos;		// current pass position

	char		sCurrentBlockName[256];	// current block name
	char		sCurrentFileName[256];	// current block name
	char		sCurrentDir[256];		// current directory 
	char		sDelimTable[256];

// saved states section

	dword		dwNumSavedStates;		//
	dword		dwCurSavedStatesPos;	//
	dword		dwCurMaxSavedSize;		//
	dword		*pSavedStates;			// saved states
	
	dword		ProcessTechnique(char *pFile, dword dwSize, char **pStr);
	dword		ProcessBlock(char *pFile, dword dwSize, char **pStr);
	dword		ProcessPass(char *pFile, dword dwSize, char **pStr);

	dword		ProcessVertexShader(char *pFile, dword dwSize, char **pStr);
	dword		ProcessVertexDeclaration(shader_t *pS, char *pFile, dword dwSize, char **pStr);
	dword		ProcessPixelShader(char *pFile, dword dwSize, char **pStr);
	dword		ProcessShaderAsm(shader_t *pS, char *pFile, dword dwSize, char **pStr, dword dwShaderType);

	dword		AddShader(char *pShaderName);
	char		*Preprocessor(char *pBuffer, dword & dwSize);
	
	dword		GetPassParameter(dword dwParam, dword dwFlags);

	void		RestoreSavedStates();
	void		ClearSavedStates();
	void		AddState2Restore(dword dwState);
	void		AddState2Restore2(dword dwState, dword dw1);
	void		AddState2Restore3(dword dwState, dword dw1, dword dw2);

	bool		ExecutePassClose();

	dword		GetCode(char *pStr, SRSPARAM *pParam, dword dwNumParam, dword *pPassCode = 0, bool bCanBeNumber = false);
	dword		GetSRSIndex(char *pStr);
	dword		GetSTSSIndex(char *pStr);
	dword		GetIndex(char *pStr, SRSPARAM *pParam, dword dwNumParam, bool bCanBeNumber);

	char		*GetToken(char *pToken, char *pResult, bool & bToken);
	void		InnerDecodeFiles(char *sub_dir = 0);

	void		ClearSRS_STSS_bUse();

public:
	
	void		SetCurrentBlock(const char *name, dword _dwNumParams, void *pParams);

	bool		DecodeFile(char *sname);
	void		DecodeFiles(char *sub_dir = 0);

	bool		ExecutePassStart();
	bool		ExecutePassNext();
	bool		ExecutePass(bool bStart);

	CTechnique(VDX8RENDER * _pRS);
	~CTechnique();

};

#endif