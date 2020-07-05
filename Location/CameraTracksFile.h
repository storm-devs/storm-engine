//===========================================================================================================================
//  Animation temporary file
//  ANT file
//  Ver 1.0
//===========================================================================================================================
//
//  Структура файла:
//
//     AntFileHeader
//     char[AntFileHeader.stringsTableSize]
//	   AntFileBone[AntFileHeader.bonesCount]
//     AntFileTrackElement[AntFileHeader.framesCount]
//
//
//===========================================================================================================================

#ifndef _ANTFILE_H_


#define ANTFILE_ID			(unsigned long)(('A' << 0) | ('N' << 8) | ('T' << 16)  | (' ' << 24))
#define ANTFILE_VER			(unsigned long)(('1' << 0) | ('.' << 8) | ('0' << 16)  | (' ' << 24))
#define ANTFILE_NOPARENT	(-1L)


#pragma pack(push, 1)


//Заголовок файла
struct AntFileHeader
{
	unsigned long id;					//Идентификатор файла
	unsigned long ver;					//Версия файла
	unsigned long bonesCount;			//Количество костей в файле >= 1
	unsigned long framesCount;			//Количество кадров анимации >= 1
	unsigned long stringsTableSize;		//Размер таблицы строк имён костей
	float defaultFPS;					//Скорость проигрывания анимации
};

//Описание кости
struct AntFileBone
{
	long parentIndex;					//Индекс родительской кости, меньший текущего
	unsigned long nameIndex;			//Индекс строки в таблице строк
};

//Описание элемента трека
struct AntFileTrackElement
{
	float qx, qy, qz, qw;					//Кватернион
	float px, py, pz;						//Позиция
	float sx, sy, sz;						//Коэфициенты масштабирования
};


#pragma pack(pop)

#endif
