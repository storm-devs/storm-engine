#ifndef _TSDARRAY_H_
#define _TSDARRAY_H_

#ifndef _XBOX
	#include <windows.h>
#else
	#include <xtl.h>
#endif

typedef DWORD TSD_ID;
#define TSD_ID_TO_INDEX(_id)    (0x7FFFFFFF & _id)
#define TSD_INDEX_TO_ID(_index) (0x80000000 | _index)

template <class RecordType>
class TSDArray  
{
public:
	inline TSDArray(DWORD _length)
		:array(0)
		,length(_length)
		,recordsCount(0)
	{
		array = NEW RecordType * [length];
		ZeroMemory(array, length << 2);
	}

	inline ~TSDArray()
	{
		if (array)
			DELETE_ARRAY(array);
	}

	inline TSD_ID AddRecord(RecordType *_record)
	{
		RecordType **ptr = array;
		DWORD counter = 0;
		while (*ptr)
		{
			if (counter++ == length)
				return 0;
			++ptr;
		}
		*ptr = _record;
		++recordsCount;
		return TSD_INDEX_TO_ID(counter);
	}

	inline void RemoveRecord (TSD_ID _n)
	{
		*(array+TSD_ID_TO_INDEX(_n)) = 0;
		--recordsCount;
	}

	inline RecordType *GetRecord (DWORD _n)
	{
		return *(array+_n);
	}

	inline RecordType *GetRecordByID (TSD_ID _n)
	{
		return *(array+TSD_ID_TO_INDEX(_n));
	}

	inline bool Full()
	{
		return (recordsCount >= length);
	}

	inline DWORD GetRecordsCount()
	{
		return recordsCount;
	}

private:
	DWORD length;
	DWORD recordsCount;
	RecordType **array;
};

#endif // !defined
