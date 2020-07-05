#ifndef _THASHDATAMAP_H_
#define _THASHDATAMAP_H_

#include <stdlib.h>
#include <string.h>
#include "..\common_h\memop.h"

#define MAX_VALUES		  500
#define MAX_STRING_LENGTH 128
#define INVALID_I         -1

template<class T> class THashDataMapTemplate  
{
public:
/*	
	THashDataMapTemplate();
	virtual ~THashDataMapTemplate();

	long Add (const char *_string, T **_data);
	T * operator[] (const char *_string);
	T * operator[] (long _i);
	void Remove (const char *_string);
	void Remove (long _i);
*/	
private:
	char strings[MAX_VALUES][MAX_STRING_LENGTH];
	T* values[MAX_VALUES];

	inline unsigned long hash_string(const char *str)
	{
	  unsigned long hval = 0;
	  while(*str != '\0')
	  {
		hval = (hval<<4) + (unsigned long int) *str++;
		unsigned long g = hval & ((unsigned long int) 0xf << (32 - 4));
		if(g != 0)
			{
				hval ^= g >> (32 - 8);
				hval ^= g;
			}
	  }
	  return hval;
	}

public:
	THashDataMapTemplate()
	{
		for (int i = 0; i < MAX_VALUES; i++)
		{
			strings[i][0] = 0;
			values[i] = 0;
		}
	}

	~THashDataMapTemplate()
	{
		for (int i = 0; i < MAX_VALUES; i++)
		{
			if (values[i])
			{
				delete values[i];
				values[i] = 0;
			}
		}
	}

	long Add (const char *_string, T **_data)
	{
		unsigned long hashValue = hash_string(_string) % MAX_VALUES;

		*_data = NEW T;

		if (!strings[hashValue][0]) 
		{ //free
			strcpy(&strings[hashValue][0], _string);
			values[hashValue] = *_data;
			return hashValue;
		}
		else
		{ //used, continue search
			for (int i=0; i<MAX_VALUES; i++)
			{
				if (!strings[i][0])
				{
					strcpy(&strings[i][0], _string);
					values[i] = *_data;
					return i;
				}
			}

			return INVALID_I;
		}
	}

	T * operator[] (const char *_string)
	{
		unsigned long hashValue = hash_string(_string) % MAX_VALUES;

		if (!strcmp(&strings[hashValue][0], _string))
		{ //hashHit
			return values[hashValue];
		}
		else
		{ //hashMiss
			for (int i=0; i<MAX_VALUES; i++)
				if (!strcmp(&strings[i][0], _string))
					return values[i];

			return 0;
		}
	}

	T * operator[] (long _i)
	{
		return values[_i];
	}

	void Remove (const char *_string)
	{
		unsigned long hashValue = hash_string(_string) % MAX_VALUES;

		if (!strcmp(&strings[hashValue][0], _string))
		{ //hashHit
			delete values[hashValue];
			values[hashValue] = 0;
			strings[hashValue][0] = 0;
		}
		else
		{ //hashMiss
			for (int i=0; i<MAX_VALUES; i++)
				if (!strcmp(&strings[i][0], _string))
				{
					delete values[i];
					values[i] = 0;
					strings[i][0] = 0;
				}
		}
	}

	void Remove (long _i)
	{
		if (values[_i])
		{
			delete values[_i];
			values[_i] = 0;
			strings[_i][0] = 0;
		}
	}

};

#endif
