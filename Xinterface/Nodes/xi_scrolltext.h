#ifndef __XI_SCROLLTEXT_H_
#define __XI_SCROLLTEXT_H_

#include "..\inode.h"

#define PICE_TYPE_STRING		0 // описатель строки
#define PICE_TYPE_FONTCHANGE	1 // описатель нового фонта
#define PICE_TYPE_COLORCHANGE	2 // описатель нового цвета
#define PICE_TYPE_NEXTLINE		3 // описатель перевода строки
#define PICE_TYPE_NEXTTABLINE	4 // описатель перевода строки с красной строки

struct STRING_PICE
{
	long type;
	union
	{
		struct{
			long startOffset;
			long charQuantity;
		} strDescr;
		long fontID;
		DWORD color;
	} data;
};

class CXI_SCROLLTEXT : public CINODE
{
public:
	CXI_SCROLLTEXT();
	~CXI_SCROLLTEXT();

	void	Draw(bool bSelected,dword Delta_Time);
	bool	Init(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2, VDX8RENDER *rs, XYRECT &hostRect, XYPOINT &ScreenSize);
	void	ReleaseAll();
	int		CommandExecute(int wActCode);
	bool	IsClick(int buttonID,long xPos,long yPos);
	void	MouseThis(float fX, float fY) {}
	void	ChangePosition( XYRECT &rNewPos );
	void	SaveParametersToIni();

	void	SetText(char *newText);

protected:
	void    ClearText();
	long    FillPices(char *pt, size_t beg, size_t size, long &idx, STRING_PICE * spl, long wid);

   	void	LoadIni(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2);

    void    TextDownShift(float );
    void    TextUpShift();

protected:
    CINODE *m_pScroller; // ссылка на скроллер (!!! обязан быть прописан перед этим)
    long    m_nMaxStringes; // число строк в выводимом окне

	DWORD   m_dwFontColor; // текущий цвет
	long    m_idFont; // текущий шрифт

	char   *m_pText;
	long    m_nPiceQuantity;
	STRING_PICE * m_pStrList;

	long	GetStringWord(char *pstr, char* buff, size_t size);
};

#endif
