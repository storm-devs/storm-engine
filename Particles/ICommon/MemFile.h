//Класс для работы с памятью как с файлом...

#ifndef _MEMORY_FILE_
#define _MEMORY_FILE_

#include "..\..\common_h\d_types.h"


#define VFSEEK_SET          (0x00000000)
#define VFSEEK_CURRENT      (0x00000001)
#define VFSEEK_END          (0x00000002)


class MemFile
{
	bool DataIsMy;

	byte* Data;
	dword MaxLen;
	dword CurPos;
	dword BiggestWritePos;

public:

	MemFile::MemFile ()
	{
		DataIsMy = false;
		Data = NULL;
		MaxLen = 0;
		CurPos = 0;
		BiggestWritePos = 0;
	}

	MemFile::~MemFile ()
	{
		Close ();
	}

	inline bool Compare (MemFile* pMemfile)
	{
		if (GetLength () != pMemfile->GetLength()) return false;

		for (DWORD i = 0; i < GetLength(); i++)
		{
			BYTE* Buffer1 = (BYTE*)GetBuffer();
			BYTE* Buffer2 = (BYTE*)pMemfile->GetBuffer();

			if (Buffer1[i] != Buffer2[i]) return false;
		}

		return true;
	}

	inline void OpenRead (void* Data, dword DataSize)
	{
		DataIsMy = false;
		this->Data = (byte*)Data;
		MaxLen = DataSize;
		CurPos = 0;
	}

	inline void OpenWrite (dword MaxSize)
	{
		DataIsMy = true;
		Data = NEW byte[MaxSize];
		memset (Data, 0, MaxSize);
		MaxLen = MaxSize;
		CurPos = 0;
	}


	inline dword MemFile::Tell ()
	{
		return CurPos;
	}

	inline dword MemFile::GetLength ()
	{
		if (DataIsMy) return BiggestWritePos;
		return MaxLen;
	}

	inline void MemFile::Seek (int NewPos, dword flags)
	{
		switch (flags)
		{
		case VFSEEK_SET:
			CurPos = NewPos;
			break;
		case VFSEEK_CURRENT:
			CurPos +=	NewPos;
			break;
		case VFSEEK_END:
			CurPos = GetLength() + NewPos;
			break;
		}
	}

	inline void MemFile::Close ()
	{
		if (DataIsMy)
		{
			delete Data;
			Data = NULL;
		}
	}


	inline dword MemFile::Read (void* Buffer, dword size)
	{
		if (!Data) return 0;
		dword real_size = CurPos+size;
		if (real_size > MaxLen)	size = size - (real_size - MaxLen);
		if (size <= 0) return 0;
		memcpy (Buffer, (Data + CurPos), size);
		CurPos+=size;
		return size;
	}

	inline dword MemFile::WriteZeroByte ()
	{
		BYTE Zero = 0;
		return Write (&Zero, 1);
	}


	inline dword MemFile::Write (const void* Buffer, dword size)
	{
		if (!DataIsMy) return 0;
		if (!Data) return 0;

		dword real_size = CurPos+size;
		if (real_size > MaxLen)	size = size - (real_size - MaxLen);
		if (size <= 0) return 0;

		memcpy ((Data + CurPos), Buffer, size);
		CurPos+=size;
		if (CurPos > BiggestWritePos) BiggestWritePos = CurPos;
		return size;
	}


	template <class TYPE>
	inline dword WriteType(TYPE &Val)
	{
		return Write(&Val, sizeof(TYPE));
	}

	template <class TYPE>
	inline dword ReadType(TYPE &Val)
	{
		return Read(&Val, sizeof(TYPE));
	}

	inline void* GetBuffer ()
	{
		return Data;
	}
 
};







#endif